#pragma once
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>

/* The ParallelTester runs a series of tests in parallel on a given TestSubject. 
   The tests are run using the Tester interface. The Tester reports how many tests
   to perform, and it has an implemention for running a test for a given a test number.

   To use this class, implement your own tester class that inherits the Tester interface
   and give it to the ParallelTester constructor. Then you should SetResources() for the
   ParallelTester to use so that you can RunTests() on it.

   For example, here's an example class made to see how well a network predicts random numbers.

    class MyTester : public ParallelTester<int> {
        public:
            // This constructor simply saves a reference to an NLib to use for creating NeuroInstances.
            MyTester(const NLib &nlib) : nlib(nlib) { }

            int GetNumTests() { return num_tests; }

            // The state holds a NeuroInstance that can be reused between tests.
            void* CreateState(NeuroNetwork* net, NeuroDevice* device) {
                MyState* s = new MyState;
                s->instance = nlib.CreateInstance(net, device);
                return (void*) s;
            }

            void DeleteState(void* state) {
                MyState* s = (MyState*) state;
                delete s->instance;
                delete s;    
            }

            // This RunTest() simulates the NeuroInstance to see how well it performs on some task.
            int RunTest(void* state, int test_index) {
                int score;
                MOA rng;
                rng.Seed(test_index);

                // This example is just tests how well the network can predict random numbers.
                score = 0;
                s->instance->StartJob();
                for (i = 0; i < 20; i++) {
                    s->instance->Simulate(100);
                    if (s->instance->GetIntegerOutput(0) == rng.Random_Integer() % 10) {
                        score++;
                    }
                }
                s->instance->StopJob('R');
                return score;
            }

            // This CombineResults() simply averages the results.
            double CombineResults(const vector<int> &results) {
                size_t i;
                double sum;
                sum = 0;
                for (i = 0; i < results.size(); i++) sum += results[i];
                return sum / (double) results.size();
            }
        protected:
            struct MyState {
                NeuroInstance* instance;
            }
            const NLib &nlib;
    }

    // Here's an example using the class. 
    double GetAverageScore(NeuroNetwork* network, const vector<NeuroDevice*> &devices) {
        NLib nlib;
        MyTester tester(nlib);
        ParallelTester<double> parallel_tester(tester);

        parallel_tester.SetResources(devices);
        return parallel_tester.RunTests(network);
    }
*/

template <typename TestOutput>
class ParallelTester {
    public:
        /* Note: These could be templated to work with other resources and test subjects. */
        typedef class NeuroDevice* Resource;
        typedef class NeuroNetwork* TestSubject;

        class Tester {
            public:
                virtual ~Tester() {};

                /* GetNumTests() returns the number of tests that should be performed. */

                virtual int GetNumTests() = 0;

                /* The void* state allows the Tester to store any information it can reuse for different tests.
                   A state is created for each thread, and then multiple tests are run using that state. */

                virtual void* CreateState(TestSubject subject, Resource resource) = 0;
                virtual void DeleteState(void* state) = 0;

                /* RunTest() returns the score for the given test number. It takes a
                   state that has been created with CreateState. It also takes a test
                   index, which is in the range [0, GetNumTests() - 1]. */

                virtual TestOutput RunTest(void* state, int test_index) = 0;

                /* CombineResults() combines results from all the tests into a single value. */

                virtual double CombineResults(const std::vector<TestOutput> &results);
        };

        ParallelTester(Tester &tester);
        void SetResources(const std::vector<Resource> &resources);

        /* RunTests() runs all the tests on the given subject and returns the average score. */

        double RunTests(TestSubject subject);

    protected:
        static void* TestingPThread(void *v);

        void TestingThread();
        Resource GetNextAvailableResource();

        Tester &tester;
        TestSubject subject;
        std::vector<TestOutput> results;

        pthread_mutex_t lock;
        std::vector<Resource> resource_pool;
        int next_available_resource;
        int nextTest;
};

template <typename TestOutput>
inline ParallelTester<TestOutput>::ParallelTester(Tester &tester) : tester(tester) {
    pthread_mutex_init(&lock, NULL);
}

template <typename TestOutput>
inline void ParallelTester<TestOutput>::SetResources(const std::vector<Resource> &resources) {
    next_available_resource = 0;
    resource_pool = resources;
}

template <typename TestOutput>
inline typename ParallelTester<TestOutput>::Resource ParallelTester<TestOutput>::GetNextAvailableResource() {
    Resource res;
    pthread_mutex_lock(&lock);
    res = resource_pool[next_available_resource];
    next_available_resource++;
    pthread_mutex_unlock(&lock);
    return res;
}

template <typename TestOutput>
inline void* ParallelTester<TestOutput>::TestingPThread(void *v) {
    ParallelTester* t = (ParallelTester*) v;
    t->TestingThread();
    return NULL;
}

template <typename TestOutput>
inline double ParallelTester<TestOutput>::RunTests(TestSubject sub) {
    size_t i;
    std::vector<pthread_t> tids;
    int err;

    subject = sub;
    next_available_resource = 0;
    results.clear();
    results.resize(tester.GetNumTests());
    nextTest = 0;

    /* Create a new thread for each device in the device pool.*/
    tids.resize(resource_pool.size());
    for (i = 0; i < tids.size(); i++) {
        // cout << "Launching thread " << i << endl;
        if (pthread_create(&tids[i], NULL, TestingPThread, this) != 0) {
            fprintf(stderr, "pthread_create error\n");
            exit(1);
        }
    }

    /* Wait for threads to complete. */
    for (i = 0; i < tids.size(); i++) {
        err = pthread_join(tids[i], NULL);
        if (err != 0) {
            std::cerr << "pthread_join error on thread " << i << " (" << tids[i] << "): " << err << std::endl;
            exit(1);
        }
    }

    return tester.CombineResults(results);
}

template <typename TestOutput>
inline void ParallelTester<TestOutput>::TestingThread() {
    int myTest;
    void* testerState;

    testerState = tester.CreateState(subject, GetNextAvailableResource());

    while (1) {
        pthread_mutex_lock(&lock);
        if (nextTest >= (int) results.size()) {
            pthread_mutex_unlock(&lock);
            break;
        }
        myTest = nextTest;
        nextTest++;
        pthread_mutex_unlock(&lock);

        results[myTest] = tester.RunTest(testerState, myTest);
    }

    tester.DeleteState(testerState);
}
