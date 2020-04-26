#if !defined(__GNU_LIBRARY__) || defined(_SEM_SEMUN_UNDEFINED)
union semun
{
int val;
// value for SETVAL
struct semid_ds* buf;
// buffer for IPC_STAT, IPC_SET
unsigned short* array; // array for GETALL, SETALL
struct seminfo* __buf; // buffer for IPC_INFO
};
#endif
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<assert.h>
#include<syslog.h>
#include<sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
// Declarations for wrapper functions...
int AllocateSharedMemory(int n);
void* MapSharedMemory(int id);
int CreateSemaphoreSet(int n, short* vals);
void DeleteSemaphoreSet(int id);
void LockSemaphore(int id, int i);
void UnlockSemaphore(int id, int i);
// The various semaphores used in the program.
enum
{
	SEM_USER_1, // Indicates it’s the first person’s turn.
	SEM_USER_2 // Indicates it’s the second person’s turn.
};
int main(int argc,char* argv[])
{
	int idShMem;
	int idSem;
	char* buf;
	// Shared memory handle.
	// Semaphore set handle.
	// Shared memory buffer address.
	short vals[2]; // Values for initialising the semaphores.
	int mySem;
	// Semaphore indicating our user.
	int yourSem; // Semaphore indicating the other user.
	puts("Welcome to CrapChat! Type ‘\\quit’ to exit.\n");
	// Get shared memory segment id off the command line.
	if (argc < 2)
	{
		// No segment id passed in, so we’ve got to create it.
		idShMem = AllocateSharedMemory(BUFSIZ);
		buf = (char*) MapSharedMemory(idShMem);
		// We want each of the users to be blocked straight off
		// when they try to lock the shared memory area. When the
		// second user starts up, they’ll unlock the first so that
		// they can type. That’s what the zeros are for.
		vals[SEM_USER_1] = 0;
		vals[SEM_USER_2] = 0;
		idSem = CreateSemaphoreSet(2, vals);
		// Save the semaphore id in our shared memory so the other
		// user can get it.
		*((int*) buf) = idSem;
		// Record which semaphores we need to wait one and signal.
		mySem = SEM_USER_1;
		yourSem = SEM_USER_2;
		// Write out the shared memory segment id so the other who
		// wants to chat with us can know.
		printf("You’re user one. Shared memory id is: %d\n",
				idShMem);
		puts("Waiting for user two...");
	}
	else
	{
		// We’ve a value! That means we’re the second user.
		idShMem = atoi(argv[1]);
		buf = (char*) MapSharedMemory(idShMem);
		// Get the semaphore set id from shared memory.
		idSem = *((int*) buf);
		// Record which semaphores we need to wait one and signal.
		mySem = SEM_USER_2;
		yourSem = SEM_USER_1;
		// Put an empty string in the shared memory.
		sprintf(buf, "");
		// Unlock the other user to signal they can talk.
		puts("You’re user two. Signalling to user one...");
		UnlockSemaphore(idSem, yourSem);
	}
	for (;;)
	{
		// Wait until it’s my turn to talk.
		LockSemaphore(idSem, mySem);
		// Did the other user exit?
		if (strcmp(buf, "\\quit\n") == 0)
			break;
		// Print the reply, if any.
		if (strlen(buf) > 0)
			printf("Reply: %s", buf);
		// Get your response.
		printf("> ");
		fgets(buf, BUFSIZ, stdin);
		// Hand over to the other user.
		UnlockSemaphore(idSem, yourSem);
		// Do you want to exit?
		if (strcmp(buf, "\\quit\n") == 0)
			break;
	}
	// First user has to deallocate the semaphores.
	if (mySem == SEM_USER_1)
		DeleteSemaphoreSet(idSem);
}


/**
* Creates a new semaphore set.
*
* @param n
Number of semaphores in set.
* @param vals Default values to start off with.
* @return Id of semaphore set.
*/
int CreateSemaphoreSet(int n, short* vals)
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


/**
* Allocates a shared memory segment.
*
* @param n Size (in bytes) of chunk to allocate.
* @return Id of shared memory chunk.
*/
int AllocateSharedMemory(int n)
{
	assert(n > 0); /* Idiot-proof the call. */
	return shmget(IPC_PRIVATE, n, IPC_CREAT | SHM_R | SHM_W);
}
/**
* Maps a shared memory segment onto our address space.
*
* @param id Shared memory block to map.
* @return Address of mapped block.
*/
void* MapSharedMemory(int id)
{
	void* addr;
	assert(id != 0); /* Idiot-proof the call. */
	addr = shmat(id, NULL, 0); /* Attach the segment...
	*/
	shmctl(id, IPC_RMID, NULL); /* ...and mark it destroyed. */
	return addr;
}
