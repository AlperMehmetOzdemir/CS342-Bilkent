#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#ifndef CS342_P3_DININGPHIL_H
#define CS342_P3_DININGPHIL_H
#endif //CS342_P3_DININGPHIL_H

#define MAX_NO_OF_PHILOSOPHERS 27
#define NO_OF_PHILOSOPHERS 3

//define booleans to make the code more understandable
typedef enum {
    TRUE = 1,
    FALSE = 0
} Bool;

//define philosopher states
typedef enum {
    THINKING = 0,
    HUNGRY = 1,
    EATING = 2
} State;

//define a state for sticks
typedef enum {
    FREE = 0,
    IN_USE = 1
} Sticks;

#define RIGHT(philID) ((philID+1)%NO_OF_PHILOSOPHERS)
#define LEFT(philID) philID
#define RIGHTNEIGHBOR(philID) ((philID == NO_OF_PHILOSOPHERS-1)? 0 : philID+1)
#define LEFTNEIGHBOR(philID) ((philID == 0)? NO_OF_PHILOSOPHERS-1 : philID-1)

State philStates[NO_OF_PHILOSOPHERS];
Sticks stickStates[NO_OF_PHILOSOPHERS];

//mutex lock for the critical section
pthread_mutex_t mutex;

//condition variable for each philosopher
pthread_cond_t cond[NO_OF_PHILOSOPHERS];