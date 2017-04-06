#include <memory.h>
#import "diningphil.h"
#import "probabilityengine.h"

//!TO DO LIST
//!TEST ON LINUX
//!DO THE MAKEFILE THING

int NO_OF_PHILOSOPHERS;

typedef struct {
    int ID;
    unsigned int think_time;
    unsigned int eat_time;
    int eat_count;
    unsigned int total_hungry_time;
    int *hungry_times;
} philosopher;

void getSticks(void *args, int eating_no) {

    philosopher *actual_args = args; //simpler approach than casting void pointer into philosopher and dereferencing

    pthread_mutex_lock(&mutex); //acquire lock to stop all threads from accessing the chopsticks

    philStates[actual_args->ID] = HUNGRY;
    printf("philosopher %d hungry\n", actual_args->ID);
    clock_t start_time = clock();

    while(stickStates[LEFT(actual_args->ID)] == IN_USE || stickStates[RIGHT(actual_args->ID)] == IN_USE) {
        pthread_cond_wait(&cond[actual_args->ID], &mutex); //wait for other to release the chopstick lock
    }

    clock_t end_time = clock();

    int hungry_duration = end_time - start_time;

    actual_args->total_hungry_time += hungry_duration;
    actual_args->hungry_times[eating_no] = hungry_duration;
    philStates[actual_args->ID] = EATING;


    stickStates[LEFT(actual_args->ID)] = IN_USE;
    stickStates[RIGHT(actual_args->ID)] = IN_USE;

    pthread_mutex_unlock(&mutex); //other threads may interact with the chopsticks as well

    printf("philosopher %d eating\n", actual_args->ID);
    usleep(actual_args->eat_time);
}

void putSticks(void *args)
{
    philosopher *actual_args = args; //simpler approach than casting void pointer into philosopher and dereferencing

    pthread_mutex_lock(&mutex); //acquire lock to stop all threads from accessing the chopsticks

    stickStates[LEFT(actual_args->ID)] = FREE;
    stickStates[RIGHT(actual_args->ID)] = FREE;

    philStates[actual_args->ID] = THINKING;

    pthread_cond_signal(&cond[LEFTNEIGHBOR(actual_args->ID)]);
    pthread_cond_signal(&cond[RIGHTNEIGHBOR(actual_args->ID)]);

    pthread_mutex_unlock(&mutex); //other threads may interact with the chopsticks as well

    printf("philosopher %d thinking\n", actual_args->ID);
    usleep(actual_args->think_time);
}

void *lifeOfAPhilosopher(void *args) { //do not need a seperate struct to store the args. Only argument is the philID

    philosopher *actual_args = args;

    //printf("A wild philosopher with ID %d has appeared\n", actual_args->ID); //Are you old enough to get the pokemon reference here?

    for (int i = 0; i < actual_args->eat_count; i++) {
        getSticks(args, i);
        putSticks(args);
    }

    return 0;
}

int main(int argc, char **argv) {

    //time unit is milliseconds. 1ms <= actionTime <= 60ms
    NO_OF_PHILOSOPHERS = atoi(argv[1]);

    assert(NO_OF_PHILOSOPHERS <= MAX_NO_OF_PHILOSOPHERS);

    const int minthink = atoi(argv[2]) * 100000; //minimum thinking time //because usleep works with microseconds
    const int maxthink = atoi(argv[3]) * 100000; //maximum thinking time
    const int mineat = atoi(argv[4]) * 100000; //minimum eating time
    const int maxeat = atoi(argv[5]) * 100000; //maximum eating time
    const char *const dist = argv[6]; //can be "uniform" or "exponential"
    const int count = atoi(argv[7]); //each philosopher will ear count times

    printf("Number of Philosophers: %d\n", NO_OF_PHILOSOPHERS);
    printf("Minthink Time: %d ms\n", minthink / 100); //here, I want to show in miliseconds
    printf("Maxthink Time: %d ms\n", maxthink / 100);
    printf("Mineat Time: %d ms\n", mineat / 100);
    printf("Maxeat Time: %d ms\n", maxeat / 100);
    printf("Distribution: %s\n", dist);
    printf("Eat Count: %d\n\n", count);

    assert(NO_OF_PHILOSOPHERS <= 27); //number is philosophers is less than or equal to 27
    assert(NO_OF_PHILOSOPHERS % 2 == 1); //number of philosophers is odd

    pthread_t philThread[NO_OF_PHILOSOPHERS];
    philosopher *philosopherList[NO_OF_PHILOSOPHERS];
    int err;

    double mean = (mineat + maxeat) / 2;

    //initialize states for philosophers and sticks, create condition variables
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++)
    {
        philStates[i] = THINKING;
        stickStates[i] = FREE;
        err = pthread_cond_init(&cond[i], NULL); //will return 0 if no error occurs during the initialization of the conditional variable
        assert(!err); //terminate the program if any error occurs during the creation of any condition variable
    }

    //start philosopher threads
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {

        philosopherList[i] = malloc(sizeof *philosopherList[0]); //allocate space for the philosopherList
        //insert the properties of a philosopher
        philosopherList[i]->ID = i;
        philosopherList[i]->eat_count = count;

        if (strcmp("uniform", dist) == 0) {
            philosopherList[i]->eat_time = get_uniform_random_value(mineat, maxeat); //generate the eat_time
            philosopherList[i]->think_time = get_uniform_random_value(mineat, maxeat); //generate the think_time
        }
        else if (strcmp("exponential", dist) == 0) {
            philosopherList[i]->eat_time = get_exponential_random_value(mean, mineat, maxeat); //generate the eat_time
            philosopherList[i]->think_time = get_exponential_random_value(mean, mineat, maxeat); //generate the think_time
        }
        else {
            assert(0);
        }

        philosopherList[i]->total_hungry_time = 0;
        philosopherList[i]->hungry_times = malloc(sizeof(int) * count);

        err = pthread_create(&(philThread[i]), NULL, lifeOfAPhilosopher, philosopherList[i]); //a new philosopher is born...
        assert(!err); //terminate the program if any error occurs during thread creation
    }

    pthread_mutex_init(&mutex, NULL);

    //wait for the philosophers to be done with their lives
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        pthread_join(philThread[i], NULL);
    }

    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        printf("philosopher %d duration of hungry state = %u\n", philosopherList[i]->ID, philosopherList[i]->total_hungry_time);
    }

    //UNCOMMENT BELOW PART TO SEE STANDARD DEVIATIONS OF HUNGRY TIMES

    /*printf("\nNow find the standard deviation of each philosopher\'s hungry times\n\n");
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        for (int j = 0; j < count; j++) {
            printf("time phil %d spent for hungry time %d is %d\n", philosopherList[i]->ID, j, philosopherList[i]->hungry_times[j]);
        }
        printf("Standard deviation for philosopher %d is %.1f\n", philosopherList[i]->ID, calculateSD(philosopherList[i]->hungry_times, count));
    }*/

    return 0;
}