#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>
#include <pthread.h>

int user_main(int argc, char **argv);

typedef struct
{
  int start1;
  int start2;
  std::function<void(int)> lambda1;
  std::function<void(int, int)> lambda2;
  int end1;
  int end2;
} thread_args;

void *thread_func_1(void *ptr)
{
  thread_args *t = ((thread_args *)ptr);
  for (int i = t->start1; i < t->end1; i++)
  {
    t->lambda1(i);
  }
  return NULL;
}

void *thread_func_2(void *ptr)
{
  thread_args *t = ((thread_args *)ptr);
  for (int i = t->start1; i < t->end1; i++)
  {
    for (int j = t->start2; j < t->end2; j++)
    {
      t->lambda2(i, j);
    }
  }
  return NULL;
}

void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads)
{
  pthread_t tid[numThreads];
  thread_args args[numThreads];
  int chunk = (high - low) / numThreads;
  for (int i = 0; i < numThreads; i++)
  {
    args[i].start1 = i * chunk;
    args[i].lambda1 = lambda;
    args[i].end1 = (i + 1) * chunk;
    pthread_create(&tid[i],
                   NULL,
                   thread_func_1,
                   (void *)&args[i]);
  }
  for (int i = 0; i < numThreads; i++)
  {
    pthread_join(tid[i], NULL);
  }
}

void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads)
{
  std::cout << "Checking for matrix.cpp" << std::endl;
  pthread_t tid[numThreads];
  thread_args args[numThreads];
  int chunk1 = (high1 - low1) / numThreads;
  int chunk2 = (high2 - low2) / numThreads;
  for (int i = 0; i < numThreads; i++)
  {
    args[i].start1 = i * chunk1;
    args[i].end1 = (i + 1) * chunk1;
    args[i].start2 = low2 + i * chunk2;
    args[i].end2 = low2 + (i + 1) * chunk2;
    args[i].lambda2 = lambda;
    pthread_create(&tid[i],
                   NULL,
                   thread_func_2,
                   (void *)&args[i]);
  }
  for (int i = 0; i < numThreads; i++)
  {
    pthread_join(tid[i], NULL);
  }
}

/* Demonstration on how to pass lambda as parameter.
 * "&&" means r-value reference. You may read about it online.
 */
void demonstration(std::function<void()> &&lambda)
{
  lambda();
}

int main(int argc, char **argv)
{
  /*
   * Declaration of a sample C++ lambda function
   * that captures variable 'x' by value and 'y'
   * by reference. Global variables are by default
   * captured by reference and are not to be supplied
   * in the capture list. Only local variables must be
   * explicity captured if they are used inside lambda.
   */
  int x = 5, y = 1;
  // Declaring a lambda expression that accepts void type parameter
  auto /*name*/ lambda1 = /*capture list*/ [/*by value*/ x, /*by reference*/ &y](void)
  {
    /* Any changes to 'x' will throw compilation error as x is captured by value */
    y = 5;
    std::cout << "====== Welcome to Assignment-" << y << " of the CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  // Executing the lambda function
  demonstration(lambda1); // the value of x is still 5, but the value of y is now 5

  int rc = user_main(argc, argv);

  auto /*name*/ lambda2 = [/*nothing captured*/]()
  {
    std::cout << "====== Hope you enjoyed CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  demonstration(lambda2);
  return rc;
}

#define main user_main