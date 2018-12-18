/********************************************************************************
 * Author: Blaine Oakley
 * Program: Designed to implement the concept of shared memory and the problems
 *          that occur when shared memory is not protected adequately
 * Date: September 7 2018
 * COP 4600
 * *****************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

/* key number */
#define SHMKEY ((key_t) 1497)

typedef struct
{
  int value;
} shared_mem;
shared_mem *total;

/*function prototypes*/
void process1();
void process2();
void process3();
void process4();

int main()
{
	int shmid,pid1,pid2,pid3,pid4;
	char *shmadd;
	shmadd = (char *) 0;

	/* Create and connect to a shared memory segment*/
 	if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
   	{
     		perror ("shmget");
      		exit (1);     
 	}
	
	if ((total = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1) 
	{
      		perror ("shmat");
      		exit (0);
	}

	/* if statements to take into account the 4 processes to be ran */
	if ((pid1 = fork()) == 0)
    	process1();

	else if ((pid2 = fork()) == 0)
	process2();

	else if ((pid3 = fork()) == 0)
	process3();

	else if ((pid4 = fork()) == 0)
	process4();

	/*wait function from sys/wait.h header, which waits for child process to join with parent */	
	waitpid(pid1, NULL, 0); 
	printf("Child with ID: %d has just exited.\n", pid1);

	waitpid(pid2, NULL, 0); 
	printf("Child with ID:%d has just exited.\n", pid2);

	waitpid(pid3, NULL, 0); 
	printf("Child with ID:%d has just exited.\n", pid3);
	
	waitpid(pid4, NULL, 0); 
	printf("Child with ID:%d has just exited.\n", pid4);

	printf("End of Simulation\n");
	return 0;
}

/*process 1 which increases the variable total by 100,000*/
void process1()
{
	int i;
	for (i = 0; i < 100000; i++)
	{
		total -> value = total -> value + 1;
	}
	printf("For Process 1: counter = %d\n", total->value);
	exit(0);
}
/*process 2 which increases the variable by 200,000*/
void process2()
{
	int i;
	for (i = 0; i < 200000; i++)
	{
		total -> value = total -> value + 1;
	}
	printf("For Process 2: counter = %d\n", total->value);
	exit(0);
}
/*process 3 which increases the variable total by 300,000*/
void process3()
{
	int i;
	for (i = 0; i < 300000; i++)
	{
		total -> value = total -> value + 1; 
	}
	
	printf("For Process 3: counter = %d\n", total->value);
	exit(0);
}
/*process 4 which increases the variable total by 500,000*/
void process4()
{
	int i;
	for (i = 0; i < 500000; i++)
	{
		total -> value = total -> value + 1; 
	}

	printf("For Process 4: counter = %d\n", total->value);
	exit(0);
}
