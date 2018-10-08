/*
* Operating Systems  [2INCO]  Practical Assignment
* Threaded Application
*
* STUDENT_NAME_1 (Dennis Rizvic)
* STUDENT_NAME_2 (Hans Chia)
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
  pthread_t                   thread_id; //the thread id
  int                         parameter; //the multiplier the thread has to work on
  bool                        finished; //if the thread is finished or still busy with its task
  int                         array_index; //the thread index
  bool			                  in_use; //is the slot used (running or waiting to be joined)
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


void *shortflip(int flip_multiplier);
void startThreat(int base, int index);
void * flipping(void * arg);
void print_buffer();

int noofbits_buffer;

static pthread_mutex_t       mutex[(NROF_PIECES/128)+1];

THREAD_STRUCT                threads[NROF_THREADS];

int main(void) {
  int flip_multiplier = 2;
  noofbits_buffer = (NROF_PIECES/128) + 1;
  if(NROF_THREADS>1){ //This if-else is placed to enable the test case with one thread to finish within time
  //initialise mutexes
  int mutex_index;
  //create mutex for every buffer
  for (mutex_index = 0; mutex_index < noofbits_buffer; mutex_index++) {
    pthread_mutex_init(&mutex[mutex_index], NULL);
    //flip bits in buffer (so it starts as black)
    buffer[mutex_index] = ~0;
  }
  for (int i = 0; i < NROF_THREADS; i++) {
    threads[i].in_use = false;
  }

  int k; //number of threads started
  k = 0;
   //integer that gives the muliplier for the pieces to be flipped

  //creates threads until max number of threads or pieces has been reached
  while (k < NROF_THREADS && flip_multiplier < NROF_PIECES) {
    startThreat(flip_multiplier,k);
    k++;
    flip_multiplier++;
  }

  while (flip_multiplier < NROF_PIECES) { //flips until flip_multiplier reaches NROF_PIECES
    for (int i = 0; i < NROF_THREADS; i++) { //checks if a thread is finished and gives new order if that is the case.
      if (threads[i].finished == true) {
        startThreat(flip_multiplier,i);
        threads[i].in_use = false;
        flip_multiplier++;
      }

    }
  }
  for (int i = 0; i < NROF_THREADS; i++) { //wait for all threads to finish
    if (threads[i].in_use == true) {
      pthread_join (threads[i].thread_id, NULL);
    }
  }
}
  else{ //in case there is only one thread
    for (int x = 0; x < noofbits_buffer; x++) {
      buffer[x] = ~0;
    }
    while (flip_multiplier < NROF_PIECES) {
    pthread_t th1;
    pthread_create(&th1,NULL,shortflip,flip_multiplier);
    pthread_join(th1, NULL);
    flip_multiplier++;
    }
  }
  print_buffer();
  return (0);
}

//starts a thread
void startThreat(int base, int index) {
  threads[index].parameter = base;
  threads[index].finished = false;
  threads[index].array_index = index;
  threads[index].in_use = true;
  pthread_create (&threads[index].thread_id, NULL,flipping, &threads[index].array_index);
}

//flips all the pieces corresponding with the multiple
void * flipping(void * arg){
  int *argi;
  argi = (int *) arg;
  int thread_index = *argi;
  int flip_multiplier = threads[thread_index].parameter; //gets the multiplier for the flipping
  int old_base = flip_multiplier;
  while (flip_multiplier < NROF_PIECES) { //flips all the bits that are on the flip_multiplier
    pthread_mutex_lock (&mutex[flip_multiplier/128]); //lock the buffer it accesses, so mulitple threads can't access it at the same time
    if(BIT_IS_SET(buffer[(flip_multiplier / 128)], (flip_multiplier % 128))){ //if bit is 1, flip to 0
      //printf("buffer %d, flipped bit number %d to white. \n",flip_multiplier / 128, flip_multiplier % 128);
      BIT_CLEAR(buffer[(flip_multiplier / 128)], (flip_multiplier % 128));
    }
    else { //if bit is 0, flip to 1
      //printf("buffer %d, flipped bit number %d to black. \n",flip_multiplier / 128, flip_multiplier % 128);
      BIT_SET(buffer[(flip_multiplier / 128)], (flip_multiplier % 128));
    }
    pthread_mutex_unlock (&mutex[flip_multiplier/128]); //unlock the buffer, so other threads can access this buffer again
    flip_multiplier += old_base; //makes it so only the multiples of the multiplier get flipped
  }
  threads[thread_index].finished = true;
  return 0;
}

//the flipping function for 1 thread, less information is needed to run it making it faster
void *shortflip(int flip_multiplier){
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
}

void print_buffer(){  //prints the flipped bits
int q;
for(int x=0;x<((NROF_PIECES/128)+1);x++){
  for(int z=0;z<128;z++){
    q= z+(x*128);
    if(BIT_IS_SET(buffer[x],z)&&(q>0) &&(q <NROF_PIECES)){
      printf("%d\n",q);
    }
    }
  }
}
