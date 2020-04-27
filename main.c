#if !defined(__GNU_LIBRARY__) || defined(_SEM_SEMUN_UNDEFINED)
union semun {
	int val;
	// value for SETVAL
	struct semid_ds *buf;
	// buffer for IPC_STAT, IPC_SET
	unsigned short *array; // array for GETALL, SETALL
	struct seminfo *__buf; // buffer for IPC_INFO
};
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/sem.h>

//Semaphores
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

//Shared Memory
#include "shm_header.h"

int CreateSemaphoreSet(int n, short *vals);
void DeleteSemaphoreSet(int id);
void LockSemaphore(int id, int i);
void UnlockSemaphore(int id, int i);
// The various semaphores used in the program.
enum
{
	SEM_USER_1, // Indicates it’s the first person’s turn.
	SEM_USER_2	// Indicates it’s the second person’s turn.
};

int main(int argc, char *argv[])
{
	//Address of our shared memory buffer
	char *buffer = NULL;
	int shm_id = 0;

	//binary semaphore
	int idSem;
	short vals[2]; // Values for initialising the semaphores.
	int mySem;	   // Semaphore indicating our user.
	int yourSem;   // Semaphore indicating the other user.

	//file to store the chat history
	FILE *history = NULL;

	char myName[20], myFriendName[20];

	//below code segment is for the setup
	if (argc != 2)
	{
		// create shared memory segment
		int shm_size = 1024;
		shm_id = AllocateSharedMemory(shm_size);
		buffer = (char *)MapSharedMemory(shm_id);

		vals[SEM_USER_1] = 0;
		vals[SEM_USER_2] = 0;
		idSem = CreateSemaphoreSet(2, vals);

		// Record which semaphores we need to wait one and signal.
		mySem = SEM_USER_1;
		yourSem = SEM_USER_2;

		// Save the semaphore id in our shared memory so the other
		// user can get it.
		*((int *)buffer) = idSem;
		printf("Use id: %d to get another user connected\n", shm_id);

		//Wait for Another user to get connected
		while (*((int *)buffer) == idSem)
			;

		puts("You are User:1 Enter Your Name:");
		scanf("%s", myName);
		strcpy(buffer, myName);

		while (strcmp(buffer, myName) == 0)
			;

		strcpy(myFriendName, buffer);
		strcpy(buffer, "");

		// history = fopen("chathistory.txt", "w");
		// if (history == NULL)
		// {
		// 	printf("File not opened\n");
		// }

		//debugging goes down here
		// time_t now;
		// time(&now);
		// fprintf(history, "Chat History:");
		// fprintf(history, "Conversation between %s and %s\n", User1, User2);
	}
	else
	{
		//Shared memory created by other user just map it
		shm_id = atoi(argv[1]);
		buffer = (char *)MapSharedMemory(shm_id);

		idSem = *((int *)buffer);

		//signal my friend that i have read the sem id
		strcpy(buffer, "");

		puts("You are User:2 Enter your Name:");
		scanf("%s", myName);

		strcpy(myFriendName, buffer);
		strcpy(buffer, myName);

		while (strcmp(buffer, myName) == 0)
			;

		strcpy(buffer, "");
		mySem = SEM_USER_2;
		yourSem = SEM_USER_1;

		UnlockSemaphore(idSem, yourSem);
		//debugging goes down here
		// open the file in append mode
		// history = fopen("chathistory.txt", "a");
	}

	printf("%s and %s are now connected\n", myName, myFriendName);

	//clear the buffer

	do
	{
		LockSemaphore(idSem, mySem);
		{
			if(strcmp(buffer,"\\exit\n") == 0)
				break;
			//read the reply if its length is greater than zero
			if (strlen(buffer) > 0)
			{
				printf("%s: ", myFriendName);
				printf("%s", buffer);

				//clear the buffer
				strcpy(buffer, "");
			}

			//write my reply for my friends message
			printf("%s:", myName);
			char message[1024];
			fgets(message, 1024, stdin);
			strcpy(buffer, message);
		}
		UnlockSemaphore(idSem, yourSem);
		if(strcmp(buffer,"\\exit\n") == 0)
			break;
	} while (1);

	//Release all the resources
	fclose(history);
	shmdt(buffer);

	if(mySem == SEM_USER_1)
		DeleteSemaphoreSet(idSem);

	return 0;
}

int CreateSemaphoreSet(int n, short *vals)
{
	union semun arg;
	int id;
	assert(n > 0);
	/* You need at least one! */
	assert(vals != NULL); /* And they need initialising! */
	id = semget(IPC_PRIVATE, n, SHM_R | SHM_W);
	arg.array = vals;
	semctl(id, 0, SETALL, arg);
	return id;
}
/**
* Frees up the given semaphore set.
*
* @param id Id of the semaphore group.
*/
void DeleteSemaphoreSet(int id)
{
	if (semctl(id, 0, IPC_RMID, NULL) == -1)
	{
		perror("Error releasing semaphore!");
		exit(EXIT_FAILURE);
	}
}
/**
* Locks a semaphore within a semaphore set.
*
* @param id Semaphore set it belongs to.
* @param i
Actual semaphore to lock.
*
* @note If it’s already locked, you’re put to sleep.
*/
void LockSemaphore(int id, int i)
{
	struct sembuf sb;
	sb.sem_num = i;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
	semop(id, &sb, 1);
}
/**
* Unlocks a semaphore within a semaphore set.
*
* @param id Semaphore set it belongs to.
* @param i
Actual semaphore to unlock.
*/
void UnlockSemaphore(int id, int i)
{
	struct sembuf sb;
	sb.sem_num = i;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;
	semop(id, &sb, 1);
}
