#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//Semaphores
#include "semaphore_header.h"

//Shared Memory
#include "shm_header.h"

void writeMessage(/*shared_mem_segment,*/ char *msg)
{
	// insert producer code here
	// the process becomes the producer
}
char *readMessage(/*shared_mem_segment*/)
{
	// insert consumer code here
	// the process becomes the consumer
}

int main(int argc, char *argv[])
{
	//Address of our shared memory buffer
	char *buffer = NULL;
	int shm_id = 0;
	FILE *history = NULL;

	char User1[20], User2[20];

	//below code segment is for the setup
	if (argc != 2)
	{
		// create shared memory segment
		int shm_size = 1024;
		shm_id = AllocateSharedMemory(shm_size);
		buffer = (char *)MapSharedMemory(shm_id);
		puts("You are User:1 Enter Your Name:");
		scanf("%s", User1);
		strcpy(buffer, User1);

		printf("Use id: %d to get another user connected\n", shm_id);

		//Wait for Another user to get connected
		while (strcmp(buffer, User1) == 0)
			;

		strcpy(User2, buffer);
		history = fopen("chathistory.txt", "w");
		if (history == NULL)
		{
			printf("File not opened\n");
		}

		time_t now;
		time(&now);
		fprintf(history, "Chat History:");
		fprintf(history, "Conversation between %s and %s\n", User1, User2);
		//debugging goes down here
	}
	else
	{
		//Shared memory created by other user just map it
		shm_id = atoi(argv[1]);
		buffer = (char *)MapSharedMemory(shm_id);
		puts("You are User:2 Enter your Name:");
		scanf("%s", User2);
		strcpy(User1, buffer);
		strcpy(buffer, User2);

		//open the file in append mode
		//history = fopen("chathistory.txt", "a");

		//debugging goes down here
	}

	printf("%s and %s are now connected\n", User1, User2);
	while (1)
	{
	}

	// if memory is not full, allow writing
	// if memory is full, extend the memory segment

	//Release all the resources
	fclose(history);
	shmdt(buffer);

	return 0;
}
