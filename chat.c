#include <stdio.h>
#include <assert.h>
//Semaphores
#include <semaphore.h>

//Shared Memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

/** * Allocates a shared memory segment. *
 * @param n Size (in bytes) of chunk to allocate. 
 * @return Id of shared memory chunk. */

int AllocateSharedMemory(int n)
{
	assert(n > 0);
	printf("Allocated Shared memory of %d bytes\n", n);
	return shmget(IPC_PRIVATE, n, IPC_CREAT | SHM_R | SHM_W);
}

/** 
 * Maps a shared memory segment onto our address space. 
 *  
 *  @param id Shared memory block to map.
 *  @return Address of mapped block. */

void *MapSharedMemory(int id)
{
	void *addr;
	assert(id != 0);
	addr = shmat(id, NULL, 0);
	shmctl(id, IPC_RMID, NULL); /*mark segment to be destroyed.When all processes are detached from the memory segment, it will be freed */
	printf("Mapped shared memory to address %p\n", addr);
	return addr;
}

sem_t mutex;
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

int main()
{
	// create shared memory segment
	int shm_size = 1024;
	int shm_id = AllocateSharedMemory(shm_size);
	void *sharedMemory = MapSharedMemory(shm_id);
	// if memory is not full, allow writing
	// if memory is full, extend the memory segment

	return 0;
}
