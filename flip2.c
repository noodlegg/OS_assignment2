/*
* Operating Systems  [2INCO]  Practical Assignment
* Threaded Application
*
* Thomas Gian (0995114)
* Minjin Song (1194206)
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
static pthread_mutex_t      mutex[NROF_PIECES/128 + 1]; //array of mutexes
int buffer_amount = (NROF_PIECES / 128) + 1;

//structure to keep track of all threads
typedef struct {
  pthread_t thread_id;
  bool finished;
  bool in_use;
  int base;
  int index;
} THREAD_CONSTRUCT;

THREAD_CONSTRUCT threads[NROF_THREADS];

void printBlacks (void) {
  int black;
  int amount =0;
  for (int i = 0; i < buffer_amount; i++) {
    for (int j = 0; j < 128; j++) {
      black = (i * 128) + j;
      if (BIT_IS_SET (buffer[i], j) && black > 0 && black < NROF_PIECES) {
        amount++;
        printf("%d\n", black);
      }
    }
  }
  printf("%d\n", amount);
}

void * flip (void * arg) {
  int *argi;
  int base;
  int multiple;
  argi = (int *) arg;
  int thread_index = *argi;
  base = threads[thread_index].base;
  multiple = base;
  //loop through the other bits for multiples
  while (multiple <= NROF_PIECES) {
    pthread_mutex_lock (&mutex[multiple / 128]);
    if (BIT_IS_SET (buffer[(multiple / 128)], multiple % 128)) {
      BIT_CLEAR (buffer[(multiple / 128)], multiple % 128);
    } else {
      BIT_SET (buffer[(multiple / 128)], multiple % 128);
    }
    pthread_mutex_unlock (&mutex[multiple / 128]);
    multiple += base;
  }
  //set thread to finished and unused
  threads[thread_index].finished = true;
  return 0;
}

int main (void) {
  //initialise all mutexes and set all bits to 1
  for (int m = 0; m < buffer_amount; m++) {
    pthread_mutex_init (&mutex[m], NULL);
    buffer[m] = ~0;
  }

  //set all threads as unused
  for (int i = 0; i < NROF_THREADS; i++) {
    threads[i].in_use = false;
    threads[i].finished = false;
  }

  bool bit_thread_exists;

  //for every bit starting at the second bit
  for (int bit = 2; bit < NROF_PIECES; bit++) {
    bit_thread_exists = false;
    //iterate through all threads
    while (!bit_thread_exists) {
      for (int t = 0; t < NROF_THREADS; t++) {
        //skip all iterations if thread has been created
        if (bit_thread_exists) {
          break;
        }
        //check whether thread is finished, then join the thread
        if (threads[t].finished) {
          pthread_join (threads[t].thread_id, NULL);
          threads[t].in_use = false;
          threads[t].finished = false;
        }
        //if thread is free, create the thread and check whether it succeeded
        if (!threads[t].in_use) {
          threads[t].base = bit;
          threads[t].index = t;
          threads[t].in_use = true;
          bit_thread_exists = true;
          int creation = pthread_create (&threads[t].thread_id, NULL, flip, &threads[t].index);
          if (creation) {
            fprintf (stderr, "Error: pthread_create() return code: %d\n", creation);
            exit (EXIT_FAILURE);
          }
        }
      }
    }
  }

  //wait for all threads to complete
  for (int k = 0; k < NROF_THREADS; k++) {
    if (threads[k].in_use) {
      pthread_join (threads[k].thread_id, NULL);
    }
  }

  printBlacks ();
  return (0);
}
