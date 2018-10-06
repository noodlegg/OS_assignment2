/*
* Operating Systems  [2INCO]  Practical Assignment
* Threaded Application
*
* STUDENT_NAME_1 (STUDENT_NR_1)
* STUDENT_NAME_2 (STUDENT_NR_2)
*
* Grading:
* Students who hand in clean code that fully satisfies the minimum requirements will get an 8.
* Extra steps can lead to higher marks because we want students to take the initiative.
* Extra steps can be, for example, in the form of measurements added to your code, a formal
* analysis of deadlock freeness etc.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>          // for perror()
#include <pthread.h>

#include "uint128.h"
#include "flip.h"
// create a bitmask where bit at position n is set
#define BITMASK(n)          (((uint128_t) 1) << (n))
// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))
// set bit n in v
#define BIT_SET(v,n)        ((v) =  (v) |  BITMASK(n))
// clear bit n in v
#define BIT_CLEAR(v,n)      ((v) =  (v) & ~BITMASK(n))
// declare a mutex, and it is initialized as well
static pthread_mutex_t       mutex[(NROF_PIECES/128)+1];
uint128_t     v = 0;

typedef struct {
    pthread_t                   thread_id;
    int                         parameter;
    bool                        finished;
    int                         array_index; //index of array when put in thread_collection
    bool			slot_used; //is the slot used (running or waiting to be joined)
} THREAD_CONSTRUCT;
int ints_in_buffer;
THREAD_CONSTRUCT                thread_collection[NROF_THREADS];
void * flipping(void * arg);
void print_buffer();




int main(void) {
  ints_in_buffer = (NROF_PIECES/128) + 1;

    //initialise mutexes
    int mutex_index;
    for (mutex_index = 0; mutex_index < ints_in_buffer; mutex_index++) {
        pthread_mutex_init(&mutex[mutex_index], NULL);
    }
    int i;
for (i = 0; i < ints_in_buffer; i++) {
    buffer[i] = ~0;
}
for (i = 0; i < NROF_THREADS; i++) {
        thread_collection[i].slot_used = false;
    }
    int k; //number of threads started
k = 0;
int current_base;
current_base = 2;
printf("1 \n");
while (k < NROF_THREADS && current_base <= NROF_PIECES) {
  startThreat(current_base,k);
        k++;
    current_base++;
}



printf("2 \n");
bool found_base;
printf("2.2 \n");
while (current_base <= NROF_PIECES) { //until finished starting up threads...
    for (i = 0; i < NROF_THREADS; i++) { //...find a thread that has finished and start a new one
        if (thread_collection[i].finished == true) { //found finished thread
            pthread_join (thread_collection[i].thread_id, NULL);
            thread_collection[i].slot_used = false;

            //search for next suitable (prime) base
            found_base = false;
            while (k < NROF_THREADS && current_base <= NROF_PIECES) {
              startThreat(current_base,k);
                    k++;
                current_base++;
            }
        }
    }
}
printf("3 \n");

for (i = 0; i < NROF_THREADS; i++) { //join (and wait for) all the threads that are used
    if (thread_collection[i].slot_used == true) {
        pthread_join (thread_collection[i].thread_id, NULL);
    }
}
printf("4 \n");

print_buffer();
return (0);
}

void startThreat(int base, int index) {
    thread_collection[index].parameter = base;
    thread_collection[index].finished = false;
    thread_collection[index].array_index = index;
    thread_collection[index].slot_used = true;
    pthread_create (&thread_collection[index].thread_id, NULL,flipping, &thread_collection[index].array_index);
}


void * flipping(void * arg){
  int *argi;
  int base;
  int multiple;
  argi = (int *) arg;
  int thread_index = *argi;
  int current_base = thread_collection[thread_index].parameter;
  int tracker = 2;
  int old_base = current_base;
  while (current_base <= NROF_PIECES) {
  if(BIT_IS_SET(buffer[(current_base / 128)], current_base % 128)){
    BIT_SET(buffer[(current_base / 128)], current_base % 128);
  }
  else {
    BIT_CLEAR(buffer[(current_base / 128)], current_base % 128);
  }
  current_base = old_base * tracker;
  tracker++;
}
thread_collection[thread_index].finished = true;
return 0;
}



void print_buffer() //for testing purposes, prints all the 64 bit ints in buffer
{
  for(int x=0;x<((NROF_PIECES/128)+1);x++){
    for(int z=0;z<128;z++){
    if(BIT_IS_SET(x,z)){
      int q = z+(x*128);
      printf("%d \n",q);
    }
  }
}
}
