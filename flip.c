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
static pthread_mutex_t      mutex[NROF_PIECES/128 + 1]; //array of mutexes
uint128_t v;

//structure to keep track of all threads
typedef struct {
  pthread_t thread_id;
  bool finished;
  bool in_use;
  int base;
  int index;
} THREAD_CONSTRUCT;

THREAD_CONSTRUCT threads[NROF_THREADS];

void printBlacks (uint128_t v) {
  for (int i = 0; i < 10; i++) {
    if (BIT_IS_SET (v, i)) {
      printf("%dth bit is black (1)\n", i+1);
    }
  }
}

void * flip (void * arg) {
  int *argi;
  int multiple;
  argi = (int *) arg;
  int thread_index = *argi;
  multiple = threads[thread_index].base;
  //loop through the other bits for multiples
  while (multiple <= NROF_PIECES) {
    pthread_mutex_lock (&mutex[multiple / 128]);
    //printf("%d is multiple of %d so flip\n", x+1, bit+1);
    if (BIT_IS_SET (buffer[(multiple / 128)], multiple % 128)) {
      BIT_CLEAR (buffer[(multiple / 128)], multiple % 128);
    } else {
      BIT_SET (buffer[(multiple / 128)], multiple % 128);
    }
    pthread_mutex_unlock (&mutex[multiple / 128]);
    multiple += multiple;
    //printf("flipped bit: %d\n", x);
  }
  //set thread to finished and unused
  threads[thread_index].finished = true;
}

int main (void) {
  int buffer_amount = (NROF_PIECES / 128) + 1;

  //initialise all mutexes and set all bits to 1
  for (int m = 0; m < buffer_amount; m++) {
    pthread_mutex_init (&mutex[m], NULL);
    buffer[m] = ~0;
  }


  //set all threads as unused
  for (int i = 0; i < NROF_THREADS; i++) {
    threads[i].thread_id = i;
    threads[i].in_use = false;
    threads[i].finished = false;
  }

  bool bit_thread_exists = false;

  //for every bit starting at the second bit
  for (int bit = 1; bit < NROF_PIECES; bit++) {

    //iterate through all threads
    for (int t = 0; t < NROF_THREADS; t++) {
      //if thread for this bit already exists then move to next bit
      if (bit_thread_exists) {
        break;
      }

      //if thread is free, create the thread and check whether it succeeded
      if (!threads[t].in_use) {
        threads[t].base = bit;
        threads[t].index = t;
        int creation = pthread_create (&threads[t].thread_id, NULL, flip, &threads[t].index);
        if (creation) {
          fprintf (stderr, "Error: pthread_create() return code: %d\n", creation);
          exit (EXIT_FAILURE);
        }
        printf ("Successfully created new thread at index %d\n", t);
        threads[t].in_use = true;
        bit_thread_exists = true;
        //else check whether thread is finished, then join the thread
      } else {
        if (threads[t].finished) {
          pthread_join (threads[t].thread_id, NULL);
          printf ("%lx: thread joined\n", pthread_self());
          threads[t].in_use = false;
          threads[t].finished = false;
        }
      }
    }
    bit_thread_exists = false;

  }

  //printBlacks (v);
  for (int m = 0; m < buffer_amount; m++) {
    printf ("buffer%d (after loop) : %lx%016lx\n", m, HI(buffer[m]), LO(buffer[m]));

  }

  // TODO: start threads to flip the pieces and output the results
  // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
  //  see bit_test() how to manipulate bits in a large integer)


  return (0);
}
