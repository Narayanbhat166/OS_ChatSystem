#include <stdio.h>
#include <assert.h>

#include <sys/ipc.h>
#include <sys/shm.h>

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
    puts("Mapped shared memory");
    return addr;
}
