#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Semaphores
#include <semaphore.h>
#include <fcntl.h>
#include<errno.h>

#define RWMUTEX1 "/read-write_mutex1"
#define RWMUTEX2 "/read-write_mutex2"
//Shared Memory
#include "shm_header.h"
sem_t *lock1,*lock2;

void releaseLock(int uid)
{
	int test;
	assert(uid<3);
	if(uid == 1)
	{
		test = sem_post(lock1);
		if(test == -1)
		{
			perror("sem_post(lock1) ");
		}
	}
	else
	{
		test = sem_post(lock2);
		if(test == -1)
		{
			perror("sem_post(lock2)");
		}
	}
}

void waitForLock(int uid)
{
	int test;
	assert(uid<3);
	if(uid == 1)
	{
		test = sem_wait(lock1);
		if(test == -1)
		{
			perror("sem_wait(lock1) ");
		}
	}
	else
	{
		test = sem_wait(lock2);
		if(test == -1)
		{
			perror("sem_wait(lock2)");
		}
	}
}
	
void printLockValues(char c)
{
	int lock1_value,lock2_value;
	sem_getvalue(lock1,&lock1_value);
	sem_getvalue(lock2,&lock2_value);
	if(c == 'w')
	{
		printf("Before first wait, value of lock1 = %d\n",lock1_value);
		printf("Before first wait, value of lock2 = %d\n",lock2_value);
	}
	else
	{
		printf("After release, value of lock1 = %d\n",lock1_value);
		printf("After release, value of lock2 = %d\n",lock2_value);
	}
}

int main(int argc, char *argv[])
{
	//Address of our shared memory buffer
	errno = 0;
	char *buffer = NULL;
	int shm_id = 0;
	int lock_value = -1000;

	//binary semaphore
	/*sem_t *lock = sem_open(RWMUTEX, O_CREAT, 0666, 1);
	if(lock == SEM_FAILED)
	{
		printf("sem_open: Error number = %d\n",errno);
		perror("Error is:");
		printf("Creating new semaphore and destroying old one...\n");
		sem_unlink(RWMUTEX);
		lock = sem_open(RWMUTEX, O_CREAT, 0666, 1);
	}*/

	//file to store the chat history
	FILE *history = NULL;

	char myName[20], myFriendName[20];
	int my_id,friend_id;

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
		lock2 = sem_open(RWMUTEX2, O_CREAT, 0666, 0);
		lock1 = sem_open(RWMUTEX1, O_CREAT, 0666, 0);
		puts("You are User:2 Enter your Name:");
		scanf("%s", myName);

		strcpy(myFriendName, buffer);
		strcpy(buffer, myName);
		my_id = 2;
		friend_id = 1;
		releaseLock(friend_id);
		printLockValues('r');
		//debugging goes down here
		// open the file in append mode
		// history = fopen("chathistory.txt", "a");
	}

	printf("%s and %s are now connected\n", myName, myFriendName);

	//clear the buffer
	strcpy(buffer, "");

	for(;;)
	{
		printLockValues('w');
		waitForLock(my_id);
		if(strcmp(buffer,"\\exit\n") == 0)	// has the other user quit
			break;
		printf("%s has acquired the lock\n", myName);
		//read the reply if its length is greater than zero
		if (strlen(buffer) > 0)
		{
			printf("%s: ", myFriendName);
			printf("%s", buffer);

			//clear the buffer
			strcpy(buffer, "");
		}
		//releaseLock(lock);
		//sem_getvalue(lock,&lock_value);
		//printf("After first release, value of lock = %d\n",lock_value);

		//write my reply for my friends message
		printf("%s: ", myName);
		char message[1024];
		//fflush(stdin);
		fgets(buffer,1024,stdin);
		//strcpy(buffer, message);
		printf("%s has released the lock\n", myName);
		releaseLock(friend_id);
		printLockValues('r');
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

