#pragma once
#include <vector>
#include <pthread.h>
#include <iostream>
#include <cstdlib>
/*
  This is a type-generic implementation of a thread-safe pool. Values are added
  to the pool using CheckIn() and removed from the pool using CheckOut().

  This is useful for distributing non-trivial objects for a function used
  repeatedly by third-party library pthreads, in which case creating and
  destroying the objects for each function call can be very inefficient.

  Example usage:

    ThreadSafePool<ApplicationEngine*> pool;
    int main() {
        for (int i = 0; i < min(MAX_APPLICATION_ENGINES, MAX_THREADS); i++)
            pool.CheckIn(new ApplicationEngine);
        ThirdPartyStartCalculations();
    }
    double Fitness(int i) {
        double fitness;
        ApplicationEngine* ae = pool.CheckOut();

        // Use application engine for something

        pool.CheckIn(ae);
        return fitness;
    }

    ThirdPartyStartCalculations() {
        pthread_t tids[MAX_THREADS];
        for (int i = 0; i < MAX_THREADS; i++)
          pthread_create(&tids[i], NULL, RunCalculations, (void*) i);
        for (int i = 0; i < MAX_THREADS; i++)
          pthread_join(tids[i], NULL);
    }
    void* RunCalculations(void* arg) {
        int start = (int) arg;
        for (int i = start; i < start+100; i++)
          ThirdPartyRecordFitness(Fitness(i));
    }
*/

namespace NeuroUtils {

template<class T>
class ThreadSafePool {
  public:
    ThreadSafePool() {
      pthread_mutex_init(&lock, NULL);
      pthread_cond_init(&poolNonEmpty, NULL);
    }

    /* Adds the given value to the pool. Returns true on success. */
    bool CheckIn(T value) {
      pthread_mutex_lock(&lock);
      pool.push_back(value);
      pthread_cond_signal(&poolNonEmpty);
      pthread_mutex_unlock(&lock);
      return true;
    }

    /* Removes a value from the pool. If true is passed, this will error if 
       the pool is empty. */
    T CheckOut(bool errorIfEmpty = false) {
      pthread_mutex_lock(&lock);
      if (errorIfEmpty && pool.size() == 0) {
        std::cerr << "ThreadSafePool::CheckOut error: pool is empty" << std::endl;
        exit(1);
      }
      // Wait till something is available
      while (pool.size() == 0) pthread_cond_wait(&poolNonEmpty, &lock);
      T rval = pool.back();
      pool.pop_back();
      pthread_mutex_unlock(&lock);
      return rval;
    }

    bool Empty() {
      bool empty;
      pthread_mutex_lock(&lock);
      empty = pool.empty();
      pthread_mutex_unlock(&lock);
      return empty;
    }

    size_t Size() {
      size_t size;
      pthread_mutex_lock(&lock);
      size = pool.size();
      pthread_mutex_unlock(&lock);
      return size;
    }

  protected:
    pthread_mutex_t lock;
    pthread_cond_t poolNonEmpty;
    std::vector<T> pool;
};

}