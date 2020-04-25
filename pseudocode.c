#include<stdio.h>
#include<semaphore.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
sem_t mutex;
void writeMessage(shared_mem_segment,char *msg)
{
	// insert producer code here
	// the process becomes the producer
}
char* readMessage(shared_mem_segment)
{
	// insert consumer code here
	// the process becomes the consumer
}

main()
{
	// create shared memory segment
	// if memory is not full, allow writing
	// if memory is full, extend the memory segment
}
