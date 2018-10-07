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

typedef struct {
  pthread_t                   thread_id;
  int                         parameter;
  bool                        finished;
  int                         array_index;
  bool			                  slot_used; //is the slot used (running or waiting to be joined)
} THREAD_CONSTRUCT;

// create a bitmask where bit at position n is set
#define BITMASK(n)          (((uint128_t) 1) << (n))
// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))
// set bit n in v
#define BIT_SET(v,n)        ((v) =  (v) |  BITMASK(n))
// clear bit n in v
#define BIT_CLEAR(v,n)      ((v) =  (v) & ~BITMASK(n))
// declare a mutex, and it is initialized as well

uint128_t     v;



void startThreat(int base, int index);
void * flipping(void * arg);
void print_buffer();

int ints_in_buffer;

static pthread_mutex_t       mutex[(NROF_PIECES/128)+1];

THREAD_CONSTRUCT                thread_collection[NROF_THREADS];

int main(void) {
  ints_in_buffer = (NROF_PIECES/128) + 1;
  //initialise mutexes
  for (int mutex_index = 0; mutex_index < ints_in_buffer; mutex_index++) {
    pthread_mutex_init(&mutex[mutex_index], NULL);
    buffer[mutex_index] = ~0;
  }

  for (int i = 0; i < NROF_THREADS; i++) {
    thread_collection[i].slot_used = false;
    thread_collection[i].finished = false;
  }

  int k; //number of threads started
  k = 0;
  int current_base = 2;
  bool found_base;
  for(current_base; current_base < NROF_PIECES; current_base++) {
    found_base = false;
    while(found_base=false){
    for (int i = 0; i < NROF_THREADS; i++) {
      if (found_base){
        break;
      } //...find a thread that has finished and start a new one
      if (thread_collection[i].finished = true) {
        pthread_join(thread_collection[i].thread_id,NULL);
        thread_collection[i].slot_used = false;
        thread_collection[i].finished = true;
      }
      if(thread_collection[i].slot_used = false){
        thread_collection[i].parameter = current_base;
        thread_collection[i].array_index = i;
        thread_collection[i].slot_used = true;
        found_base = true;
        pthread_create (&thread_collection[i].thread_id, NULL,flipping, &thread_collection[i].array_index);
      }
    }
  }
}
for (int k = 0; k < NROF_THREADS; k++) {
  if (thread_collection[k].slot_used) {
    pthread_join (thread_collection[k].thread_id, NULL);
  }
}
  print_buffer();
  return (0);
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
    pthread_mutex_lock (&mutex[current_base/128]);
    if(BIT_IS_SET(buffer[(current_base / 128)], (current_base % 128))){
      BIT_CLEAR(buffer[(current_base / 128)], (current_base % 128));
    }
    else {
      BIT_SET(buffer[(current_base / 128)], (current_base % 128));
    }
    pthread_mutex_unlock (&mutex[current_base/128]);
    current_base = old_base * tracker;
    tracker++;
  }
  thread_collection[thread_index].finished = true;
  return 0;
}



void print_buffer() //for testing purposes, prints all the 64 bit ints in buffer
{for(int x=0;x<((NROF_PIECES/128)+1);x++){
  for(int z=0;z<128;z++){
    if(BIT_IS_SET(buffer[x],z)){
      int q = z+(x*128);
      printf("%d \n",q);
    }
  }

}
}
