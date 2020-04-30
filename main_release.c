#include <stdlib.h>
#include <string.h>
// semaphores
#include "semaphore_header.h"
//Shared Memory
#include "shm_header.h"

int main(int argc, char *argv[])
{
	//Address of our shared memory buffer
	errno = 0;
	char *buffer = NULL;
	int shm_id = 0;
	int lock_value = -1000;

	//file to store the chat history
	FILE *history = NULL;

	char myName[20], myFriendName[20];
	int my_id,friend_id;
	char s[3];	// dummy to consume the newline character 
	
	printf("Welcome to OSChat - A Simplistic Chat Application\nType \\exit to quit the program\n");

	//below code segment is for the setup
	if (argc != 2)
	{
		// create shared memory segment
		int shm_size = 1024;
		shm_id = AllocateSharedMemory(shm_size);
		buffer = (char *)MapSharedMemory(shm_id);
		sem_unlink(RWMUTEX1);
		sem_unlink(RWMUTEX2);
		lock1 = sem_open(RWMUTEX1, O_CREAT, 0666, 0);
		lock2 = sem_open(RWMUTEX2, O_CREAT, 0666, 0);
		puts("You are User:1 Enter Your Name:");
		scanf("%s", myName);
		strcpy(buffer, myName);
		my_id = 1;
		friend_id = 2;

		printf("Use id: %d to get another user connected\n", shm_id);

		//Wait for Another user to get connected
		printf("Waiting for other user...\n");
		while (strcmp(buffer, myName) == 0)
			;

		strcpy(myFriendName, buffer);
		fgets(s,3,stdin);
		history = fopen("chathistory.txt", "w");
		if (history == NULL)
		{
			printf("File not opened\n");
		}

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
		lock2 = sem_open(RWMUTEX2, O_CREAT, 0666, 0);	// attach the semaphores to second process
		lock1 = sem_open(RWMUTEX1, O_CREAT, 0666, 0);
		puts("You are User:2 Enter your Name:");
		scanf("%s", myName);

		strcpy(myFriendName, buffer);
		strcpy(buffer, myName);
		fgets(s,3,stdin);
		my_id = 2;
		friend_id = 1;
		releaseLock(friend_id);
		// uncomment below line to see lock values
		//printLockValues('r');
		//debugging goes down here
		// open the file in append mode
		// history = fopen("chathistory.txt", "a");
	}

	printf("%s and %s are now connected\n", myName, myFriendName);

	//clear the buffer
	strcpy(buffer, "");

	for(;;)
	{
		// uncomment to see lock values
		//printLockValues('w');
		waitForLock(my_id);
		if(strcmp(buffer,"\\exit\n") == 0)	// has the other user quit
			break;
		//printf("%s has acquired the lock\n", myName);
		//read the reply if its length is greater than zero
		if (strlen(buffer) > 0)
		{
			printf("%s: ", myFriendName);
			printf("%s", buffer);

			//clear the buffer
			strcpy(buffer, "");
		}

		//write my reply for my friends message
		printf("%s: ", myName);
		fgets(buffer,1024,stdin);
		//printf("%s has released the lock\n", myName);
		releaseLock(friend_id);
		// uncomment to see lock values
		//printLockValues('r');
		if(strcmp(buffer,"\\exit\n") == 0)	// has the other user quit
			break;
	}

	//First user has to release all the resources
	if(my_id == 1)
	{
		fclose(history);
		shmdt(buffer);
		sem_unlink(RWMUTEX1);
		sem_unlink(RWMUTEX2);
	}
	return 0;
}
