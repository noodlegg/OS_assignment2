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
uint128_t v;

int bit_test (void){
  // set all bits to 1
  v = ~0;
  printf ("v (all 1's) : %lx%016lx\n", HI(v), LO(v));
  return v;
}

int flip (int x)
{
  if (BIT_IS_SET (v,x))
  {
    BIT_CLEAR (v,x);
  }
  else
  {
    BIT_SET (v,x);
  }
  //printf("flipped bit: %d\n", x);
  printf ("v in flip) : %lx%016lx\n", HI(v), LO(v));

  return v;
}

void printBlacks (uint128_t v)
{
  for (int i = 0; i < 10; i++)
  {
    if (BIT_IS_SET (v, i))
    {
      printf("%dth bit is black (1)\n", i+1);
    }
  }
}

int main (void)
{
  printf ("v (before bit_test) : %lx%016lx\n", HI(v), LO(v));
  bit_test ();
  printf ("v (before loop) : %lx%016lx\n", HI(v), LO(v));

  //for every bit starting at the second bit
  for (int bit = 1; bit < 10; bit++)
  {
    //loop through the other bits for multiples
    for (int x = 1; x < 10; x++)
    {
      //if its a multiple of the bit then flip
      if ((x+1)%(bit+1) == 0)
      {
        //printf("%d is multiple of %d so flip\n", x+1, bit+1);
        flip (x);
      }
    }
  }

  //printBlacks (v);
  printf ("v (after loop) : %lx%016lx\n", HI(v), LO(v));

  // TODO: start threads to flip the pieces and output the results
  // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
  //  see bit_test() how to manipulate bits in a large integer)


  return (0);
}
