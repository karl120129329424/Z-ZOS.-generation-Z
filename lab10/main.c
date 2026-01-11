#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 10
#define ARRAY_SIZE 5

int shared_array[ARRAY_SIZE];
int write_counter = 0;

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;