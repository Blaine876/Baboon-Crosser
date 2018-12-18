/* Author: Blaine Oakley
/* Program: Designed to use semaphores to protect a limited size resource
/*          which was implemented on a circular buffer of size 15.
/* Date: October 11 2018
*/


#define _REENTRANT
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#define SHMKEY ((key_t) 400L)       /* shared memory key */
#define BUFFSIZE 15                 /* size of buffer */

void * producer_thread(void * arg);     /* function prototypes */
void * consumer_thread(void * arg);


typedef struct                      /* structure of shared memory */
    {
    int value;
} shared_mem;

shared_mem * total;                 /* pointer to shared memory */


char circle_buff[BUFFSIZE];       /* circular buffer */

char data;              
FILE* fp;                         /* pointer to FILE */

sem_t mutex;                      /* intialization of sempahores mutex, full and empty */
sem_t empty;                 
sem_t full;


int main()
{
   
    int shmid; //shared memory ID */
    pthread_t tid1[1]; //process id for thread 1
    pthread_t tid2[1]; /* process id for thread 2 */
    pthread_attr_t attr[1]; /* attribute pointer array */

    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
    
    char* shmadd;
    shmadd = (char*)0;

    if ((shmid = shmget(SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0) 
    {
        perror("shmget");
        exit(1);
    }

    if ((total = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1) 
    {
            perror ("shmat");
            exit (0);
    }

    /* Flush output buffer */
    fflush(stdout);

    /* Required to schedule thread independently.*/
    pthread_attr_init(&attr[0]);
    pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
    /* end to schedule thread independently*/

    /* Create the threads */
    pthread_create(&tid1[0], &attr[0], producer_thread, NULL);
    pthread_create(&tid2[0], &attr[0], consumer_thread, NULL);

    /* Wait for the threads to finish */
    pthread_join(tid1[0], NULL);
    pthread_join(tid2[0], NULL);

    /* freeing semaphore memory*/
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    if ((shmctl(shmid, IPC_RMID, (struct shmid_ds*)0)) == -1) {
        perror("shmctl");
        return -1;
    }

    pthread_exit(NULL);
    return 0;
}

// Producer function
void * producer_thread(void * arg)
{
    fp = fopen("mytest.dat", "r");
    while (fscanf(fp, "%c", &data) != EOF)
    {
        sem_wait(&empty);

        if (total->value == BUFFSIZE) 
        {
            total->value = 0;
        }

        sem_wait(&mutex);
        circle_buff[total->value % BUFFSIZE] = data;  /*critical section protected*/
        total->value++;
        sem_post(&mutex);
        printf("\n producer output: %c |", data);
        fflush(stdout);
        sem_post(&full);
    }
    
    fclose(fp);

    circle_buff[total->value] = '*';
    total->value++;
    
}

// Consumer function
void * consumer_thread(void * arg)
{
    sem_post(&empty);
    char item;
    while (item != '*')   /* indicates EOF*/
    {
        sleep(1);    /*sleep fucntion included so producer produces faster than consumer consumes*/
        sem_wait(&full);
        if (total->value > 0)     
        {
            sem_wait(&mutex);
            item = circle_buff[(total->value - 1)];    /*critical section protected*/
            total-> value--;
            sem_post(&mutex);
            printf(" consumer output: %c ", item);
            fflush(stdout);
        }
        sem_post(&empty);
    }
}