#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 10
#define ARRAY_SIZE 5

char shared_array[ARRAY_SIZE];
int write_counter = 0;

int active_readers = 0;
int writer_active = 0;
int writer_waiting = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;