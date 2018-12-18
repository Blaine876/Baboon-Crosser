
#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
//#include <sys/ipc.h>
//#include <sys/shm.h>
//#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
//#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

sem_t rope;
sem_t right_mutex;
sem_t left_mutex;
sem_t deadlock_protection;
sem_t count;

int right = 0;
int left = 0;
int travel;    

void cross(int x);
void* right_side(void*);
void* left_side(void*);

//thread handling the right to left to travel
void* right_side(void*arg)            
{
    int baboon = *(int*)arg;
    int on_rope;
    sem_wait(&deadlock_protection);
    sem_wait(&right_mutex);
    right++;
    if (right == 1)
    {
        sem_wait(&rope);
        printf("Baboon %d: Waiting\n", baboon + 1);
    }
    sem_post(&right_mutex);
    sem_post(&deadlock_protection);
    sem_wait(&count);
    sem_getvalue(&count, &on_rope);
    printf("Baboon %d: Cross rope request granted (Baboon crossing: left to right, Number of baboons on rope: %d)\n", baboon + 1,3-on_rope);
    sleep(travel);
    sem_getvalue(&count, &on_rope);
    printf("Baboon %d: Exits rope\n", baboon + 1);
    sem_post(&count);
    sem_wait(&right_mutex);
    right--;
    if (right == 0)
        sem_post(&rope);
    sem_post(&right_mutex);
    return NULL;
}

//thread handling left to right travel
void* left_side(void*arg)    
{
    int baboon = *(int*)arg;
    int on_rope;
    sem_wait(&deadlock_protection);
    sem_wait(&left_mutex);
    left++;
    if (left == 1)
    {
        sem_wait(&rope);
        printf("Baboon %d: Waiting\n", baboon + 1);
    }
    sem_post(&left_mutex);
    sem_post(&deadlock_protection);
    sem_wait(&count);
    sem_getvalue(&count, &on_rope);
    printf("Baboon %d: Cross rope request granted (Baboon crossing: right to left, Number of baboons on rope: %d)\n", baboon + 1, 3-on_rope);
    sleep(travel);
    sem_getvalue(&count, &on_rope);
    printf("Baboon %d: Exits rope\n", baboon + 1);
    sem_post(&count);
    sem_wait(&left_mutex);
    left--;
    if (left == 0)
        sem_post(&rope);
    sem_post(&left_mutex);
    return NULL;

}
/*main function*/
int main(int argc, char *argv[])
{ 

    char c;
    int baboonCnt= 0;
    char temp[100];

    sem_init(&rope,0,1);                        //mutual exclusion on rope
    sem_init(&right_mutex,0,1);                  //right side
    sem_init(&left_mutex,0,1);                  //left side
    sem_init(&deadlock_protection,0,1);         //prevent deadlocks
    sem_init(&count,0,3);                     //only 3 baboons are allowed on the rope

    //all input arguements should be entered
    if ( argc == 3 )                    
    {
        travel = atoi(argv[2]);
        FILE *file;
        if((file = fopen(argv[1], "r"))){
        //printf("read\n");
            while(fscanf(file, "%c", &c) != EOF)
            {
                if(c == 'L'|| c == 'R')
                {
                    temp[baboonCnt] = c;
                    baboonCnt++;
                }
            }
          printf("The input is\n");
          int j = 0;
          for(j = 0;j< baboonCnt;++j)
          {
              printf("%c ",temp[j]);
          }
          printf("\n");
    
       } else   
        {
            printf("Unable to read data from the input file.");
            return 0;
        }
       }
       
        //int id[baboonCnt];
        pthread_t rightThread[baboonCnt],leftThread[baboonCnt];
        int rightcnt=0, leftcnt=0, rightid[baboonCnt], leftid[baboonCnt];
    
      int i = 0;
  		for(i = 0;i < baboonCnt;++i)
  		{
			sleep(1);
			if(temp[i]=='L')
			{
				rightid[rightcnt]=i;
				printf("Baboon %d wants to cross left to right\n",i + 1);
				pthread_create(&rightThread[rightcnt],NULL, (void *) &right_side,(void *) &rightid[rightcnt] );
				++rightcnt;
			}
			else if(temp[i]=='R')
			{
				leftid[leftcnt]=i;
				printf("Baboon %d wants to cross right to left\n",i + 1);
				pthread_create(&leftThread[leftcnt],NULL, (void *) &left_side,(void *) &leftid[leftcnt] );
				++leftcnt;
			}
      
		}
			int k = 0;
        for(k=0;k< leftcnt;++k)
        {
			if(temp[k]=='L') {
				pthread_join(rightThread[k],NULL);
				//printf("rightThread %d",k);
			} else if(temp[k] == 'R'){
				pthread_join(leftThread[k],NULL); 
				//printf("leftThread %d",k); 
			}
       }
        sem_destroy (&rope); 
        sem_destroy (&right_mutex);
        sem_destroy (&left_mutex);
        sem_destroy (&deadlock_protection);
        sem_destroy (&count);
        pthread_exit(NULL);
        exit(0);
    
}
