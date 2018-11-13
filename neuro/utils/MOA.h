/* This code was pulled from 
  http://web.eecs.utk.edu/~plank/plank/classes/cs494/494/notes/Bloom-Filter/MOAC.c
  on July 29, 2016 */

/* These are wrappers around the 
   "Mother of All" random number generator from http://www.agner.org/random/.

   If you seed this from time(0), you should XOR time(0) with some fixed number that
   differs from usage to usage.  That way, multiple modules can seed from time(0), and
   they will all be using different RNG seeds
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/universal.h"

namespace NeuroUtils {

inline double MOA::Random_Double() {
    uint32_t result;
    do {
      result = Random_32();
    } while (result == 0xffffffffU);
    return (double)result / (double)(0xffffffffU);
}

inline double MOA::Random_DoubleI() {
    uint32_t result;
    result = Random_32();
    return (double)result / (double)(0xffffffffU);
}

inline uint32_t MOA::Random_32() {
  uint64_t sum;
  sum = (uint64_t)2111111111UL * (uint64_t)X[3] +
     (uint64_t)1492 * (uint64_t)(X[2]) +
     (uint64_t)1776 * (uint64_t)(X[1]) +
     (uint64_t)5115 * (uint64_t)(X[0]) +
     (uint64_t)X[4];
  X[3] = X[2];  X[2] = X[1];  X[1] = X[0];
  X[4] = (uint32_t)(sum >> 32);
  X[0] = (uint32_t)sum;
  Counter++;
  return X[0];
}

inline int MOA::Random_Integer() {
  uint32_t r;

  r = Random_32();
  r &= 0x7fffffff;
  return (int) r;
}

inline uint64_t MOA::Random_64() {
  uint64_t sum;

  sum = Random_32();
  sum <<= 32;
  sum |= Random_32();
  return sum;
}

inline void MOA::Random_128(uint64_t *x) {
  x[0] = Random_64();
  x[1] = Random_64();
  return;
}

inline uint32_t MOA::Random_W(int w, int zero_ok)
{
  uint32_t b;

  do {
    b = Random_32();
    if (w == 31) b &= 0x7fffffff;
    if (w < 31)  b %= (1 << w);
  } while (!zero_ok && b == 0);
  return b;
}

inline void MOA::Seed(uint32_t seed) {
  int i;
  uint32_t s = seed;
  for (i = 0; i < 5; i++) {
    s = s * 29943829 - 1;
    X[i] = s;
  }
  Counter = 0;
  for (i=0; i<19; i++) Random_32();
}

inline void MOA::Fill_Random_Region (void *reg, int size)
{
  uint32_t *r32;
  uint8_t *r8;
  int i;

  r32 = (uint32_t *) reg;
  r8 = (uint8_t *) reg;
  for (i = 0; i < size/4; i++) r32[i] = Random_32();
  for (i *= 4; i < size; i++) r8[i] = (uint8_t) Random_W(8, 1);
}

inline void MOA::Get_State(void *buffer)
{
  memcpy(buffer, X, sizeof(uint32_t) * 5);
  Counter = 0;
}

inline uint64_t MOA::Get_Counter()
{
  return Counter;
}

inline void MOA::Set_State(void *buffer, uint64_t counter)
{
  memcpy(X, buffer, sizeof(uint32_t) * 5);
  Counter = 0;
  while (Counter < counter) (void) Random_32();
}

}
