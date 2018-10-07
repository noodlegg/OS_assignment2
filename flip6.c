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
  bool			      in_use; //is the slot used (running or waiting to be joined)
} THREAD_STRUCT;

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

int noofbits_buffer;

static pthread_mutex_t       mutex[(NROF_PIECES/128)+1];

THREAD_STRUCT                threads[NROF_THREADS];

int main(void) {
  noofbits_buffer = (NROF_PIECES/128) + 1;

  //initialise mutexes
  int mutex_index;
  for (mutex_index = 0; mutex_index < noofbits_buffer; mutex_index++) {
    pthread_mutex_init(&mutex[mutex_index], NULL);
  }

  int i;
  //make all pieces black by flipping them
  for (i = 0; i < noofbits_buffer; i++) {
    buffer[i] = ~0;
  }

  //set all threads to not in use
  for (i = 0; i < NROF_THREADS; i++) {
    threads[i].in_use = false;
  }

  int k; //number of threads started
  k = 0;
  int flip_multiplier = 2; //integer that gives the muliplier for the pieces to be flipped

  //creates threads until max number of threads or pieces has been reached
  while (k < NROF_THREADS && flip_multiplier < NROF_PIECES) {
    startThreat(flip_multiplier,k);
    k++;
    flip_multiplier++;
  }

  while (flip_multiplier < NROF_PIECES) { //flips until flip_multiplier reaches NROF_PIECES
    for (i = 0; i < NROF_THREADS; i++) { //checks if a thread is finished and gives new order if that is the case.
      if (threads[i].finished == true) {
        startThreat(flip_multiplier,i);
        //printf("2.5 \n");
        threads[i].in_use = false;
        flip_multiplier++;
      }

    }
  }
  for (i = 0; i < NROF_THREADS; i++) { //join (and wait for) all the threads that are used
    if (threads[i].in_use == true) {
      pthread_join (threads[i].thread_id, NULL);
    }
  }
  print_buffer();
  return (0);
}

void startThreat(int base, int index) {
  threads[index].parameter = base;
  threads[index].finished = false;
  threads[index].array_index = index;
  threads[index].in_use = true;
  pthread_create (&threads[index].thread_id, NULL,flipping, &threads[index].array_index);
}


void * flipping(void * arg){
  int *argi;
  argi = (int *) arg;
  int thread_index = *argi;
  int flip_multiplier = threads[thread_index].parameter;
  int old_base = flip_multiplier;
  while (flip_multiplier < NROF_PIECES) {
    pthread_mutex_lock (&mutex[flip_multiplier/128]);
    if(BIT_IS_SET(buffer[(flip_multiplier / 128)], (flip_multiplier % 128))){
      //printf("buffer %d, flipped bit number %d to white. \n",flip_multiplier / 128, flip_multiplier % 128);
      BIT_CLEAR(buffer[(flip_multiplier / 128)], (flip_multiplier % 128));
    }
    else {
      //printf("buffer %d, flipped bit number %d to black. \n",flip_multiplier / 128, flip_multiplier % 128);
      BIT_SET(buffer[(flip_multiplier / 128)], (flip_multiplier % 128));
    }
    pthread_mutex_unlock (&mutex[flip_multiplier/128]);
    flip_multiplier += old_base;
  }
  threads[thread_index].finished = true;
  return 0;
}



void print_buffer(){ //for testing purposes, prints all the 64 bit ints in buffer
for(int x=0;x<((NROF_PIECES/128)+1);x++){
  for(int z=0;z<128;z++){
    if(BIT_IS_SET(buffer[x],z)){
      int q = z+(x*128);
      if(q < NROF_PIECES && q != 0){
      printf("%d \n",q);
    }
    }
  }

}
}
