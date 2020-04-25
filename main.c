#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Semaphores
#include "semaphore_header.h"
//Shared Memory
#include "shm_header.h"

int main(int argc, char *argv[])
{
	//Address of our shared memory buffer
	char *buffer = NULL;
	int shm_id = 0;

	char User1[20], User2[20];
	createSemaphores();

	if (argc != 2)
	{
		// create shared memory segment
		int shm_size = 1024;
		shm_id = AllocateSharedMemory(shm_size);
		buffer = (char *)MapSharedMemory(shm_id);
		puts("You are User:1 Enter Your Name:");
		scanf("%s", User1);
		sprintf(buffer, User1);

		printf("Use id: %d to get another user connected\n", shm_id);
		while (strcmp(buffer, User1) == 0)
			;

		strcpy(User2, buffer);
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
		sprintf(buffer, User2);

		//debugging goes down here
	}

	printf("%s and %s are now connected\n", User1, User2);
	while (1)
	{
	}

	// if memory is not full, allow writing
	// if memory is full, extend the memory segment

	return 0;
}
