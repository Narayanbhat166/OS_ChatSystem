#include<assert.h>
#include<semaphore.h>
#include<fcntl.h>
sem_t* rw_mutex = NULL;
sem_t* mutex = NULL;
int read_count = 0;

void createSemaphores()
{
	rw_mutex = sem_open("/read-write_mutex",O_CREAT,644,1);
	mutex = sem_open("/mutex",O_CREAT,644,1);
}

void writeMessage(int shm_id,char *msg)
{
	// insert producer code here
	// the process becomes the writer process
	do
	{
		sem_wait(rw_mutex);
		// writing code
		// add a code to exit this loop when user presses 'ENTER' key
		sem_post(rw_mutex);
	}while(1);
}

char *readMessage(int shm_id)
{
	// insert consumer code here
	// the process becomes the reader process
	do 
	{
		sem_wait(mutex);
		read_count++;
		if (read_count == 1)
			sem_wait(rw_mutex);
		sem_post(mutex);
			/* reading is performed */
		sem_wait(mutex);
		read_count--;
		if (read_count == 0)
			sem_post(rw_mutex);
		sem_post(mutex);
	}while(true);
}


