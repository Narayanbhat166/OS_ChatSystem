#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Semaphores
#include <semaphore.h>
#include <fcntl.h>

#define waitForLock sem_wait
#define releaseLock sem_post

//Shared Memory
#include "shm_header.h"

void createSemaphores(sem_t *lock)
{
	lock = sem_open("/read-write_mutex", O_CREAT, 644, 1);
}

int main(int argc, char *argv[])
{
	//Address of our shared memory buffer
	char *buffer = NULL;
	int shm_id = 0;

	//binary semaphore
	sem_t *lock = sem_open("/read-write_mutex", O_CREAT, 644, 1);

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
		puts("You are User:1 Enter Your Name:");
		scanf("%s", myName);
		strcpy(buffer, myName);

		printf("Use id: %d to get another user connected\n", shm_id);

		//Wait for Another user to get connected
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
		puts("You are User:2 Enter your Name:");
		scanf("%s", myName);

		strcpy(myFriendName, buffer);
		strcpy(buffer, myName);

		//debugging goes down here
		// open the file in append mode
		// history = fopen("chathistory.txt", "a");
	}

	printf("%s and %s are now connected\n", myName, myFriendName);

	//clear the buffer
	strcpy(buffer, "");

	while (1)
	{
		waitForLock(lock);
		{
			printf("%s has acquired the lock\n", myName);
			//read the reply if its length is greater than zero
			if (strlen(buffer) > 0)
			{
				printf("%s: ", myFriendName);
				printf("%s\n", buffer);

				//clear the buffer
				strcpy(buffer, "");
			}

			//write my reply for my friends message
			printf("%s:", myName);
			char message[1024];
			scanf("%s", message);
			strcpy(buffer, message);
			printf("%s has released the lock\n", myName);
		}
		releaseLock(lock);
	}

	//Release all the resources
	fclose(history);
	shmdt(buffer);

	return 0;
}
