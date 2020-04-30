#include <stdio.h>
#include <assert.h>

//Semaphores
#include <semaphore.h>
#include <fcntl.h>
#include<errno.h>

#define RWMUTEX1 "/read-write_mutex1"
#define RWMUTEX2 "/read-write_mutex2"

sem_t *lock1,*lock2;

void releaseLock(int uid)	// releases lock based on the user
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

void waitForLock(int uid)	// calls sem_wait() based on the user
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
	
void printLockValues(char c)		// prints the lock values; if 'w' is passed as parameter prints lock values before wait, otherwise after release lock values are printed
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
