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
static pthread_mutex_t      mutex          = PTHREAD_MUTEX_INITIALIZER;
uint128_t     v = 0;



void flipper(int x){
  printf ("time to set bit\n");
  int q = x;
  for(int z=1; q<10;z++){
    q = (x*z);
    if(BIT_IS_SET(v,q)){
      BIT_CLEAR(v,q);
      printf ("set the bit\n");
    }
    else{
      BIT_SET(v,q);
      printf ("unset the bit\n");
    }
  }
}

int main (void)
{
  for(int x=1;x<10;x++){
    flipper(x);
  }
  funclast(v);
  // TODO: start threads to flip the pieces and output the results
  // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
  //  see bit_test() how to manipulate bits in a large integer)
  return (0);
}

void funclast(uint128_t v){
  for(int x=1;x<10;x++){
    if(BIT_IS_SET(v,x)){
      printf("%d \n",x);
    }
  }
}
