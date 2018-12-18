/********************************************************************************
 * Author: Blaine Oakley
 * Program: Updated code from Project 1 designed to implement the concept of 
            semaphores in order to protect the critical section of each process.
 * Date: September 23 2018
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
#include <sys/sem.h>


/* key number */
#define SEMKEY ((key_t) 400L)        // shared memory key
#define SHMKEY ((key_t) 1497)        // semaphore key
#define NSEMS 1                      // number of generated semaphores


typedef struct                        // structure of shared memory
{
  int value;
} shared_mem;

shared_mem *total;            // pointer to shared mem


typedef union                //semaphore union used to generate semaphore
{    
	int val;		
	struct semid_ds *buf;
	ushort *array;
} semunion;


int sem_id;                         // semaphore id
int status;
static struct sembuf OP = {0,-1,0};      //semaphore buffers
static struct sembuf OV = {0,1,0};      // semaphore buffers
struct sembuf *P = &OP;                // pointer to buffer
struct sembuf *V = &OV;                // pointer to buffer

/*function prototypes*/
void process1();
void process2();
void process3();
void process4();
int POP();
int VOP();

int main()
{
  
  int shmid;
  int pid1,pid2,pid3,pid4; // process ids
 
  
	char *shmadd;
	shmadd = (char *) 0;
 
  int semnum = 0;
  int value;
  int value1;
  
  semunion semctl_arg;
  semctl_arg.val =1;
  

  /* Create semaphores */
	sem_id = semget(SEMKEY, NSEMS, IPC_CREAT | 0666);
	if(sem_id < 0) printf("Error in creating the semaphore./n");

	
  /* Initialize semaphore */
	value1 = semctl(sem_id, semnum, SETVAL, semctl_arg);
  value = semctl(sem_id, semnum, GETVAL, semctl_arg);
	if (value < 1) printf("Error detected in SETVAL.\n");

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
 
 
  total -> value = 0;
  
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
   
  // detaching shared memory
  if(shmdt(total) == -1)
  {
    perror("shmdt");
    exit(-1);
  }
  
  // removing shared memory
  shmctl(shmid, IPC_RMID, NULL);
  
  /* De-allocate semaphore */
	semctl_arg.val = 0;
	status = semctl(sem_id, 0, IPC_RMID, semctl_arg);
	if( status < 0) printf("Error in removing the semaphore.\n");
 
  printf("End of Simulation\n");
 
return 0; 
}

/*process 1 which increases the variable total by 100,000*/
void process1()
{
	int i;
	for (i = 0; i < 100000; i++)
	{
    POP();		// critical section of process 1 protected by POP and VOP
		total -> value = total -> value + 1;
    VOP();
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
		 POP();		// critical section of process 1 protected by POP and VOP
		 total -> value = total -> value + 1;
     VOP();
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
    POP();		// critical section of process 1 protected by POP and VOP
		total -> value = total -> value + 1;
    VOP(); 
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
    POP();		    // critical section of process 1 protected by POP and VOP
		total -> value = total -> value + 1;
    VOP();
	}

	printf("For Process 4: counter = %d\n", total->value);
	exit(0);
}

// POP (wait()) function for semaphore to protect critical section
int POP()
{	
	int status;
	status = semop(sem_id, P,1);
	return status;
}

// VOP (signal()) function for semaphore to release protection
int VOP()
{	
	int status;
	status = semop(sem_id, V,1);
	return status;
}
