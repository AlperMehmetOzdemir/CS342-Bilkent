#import "diningphil.h"
#import "probabilityengine.h"

//!TO DO LIST
//!TEST FOR EXPONENTIAL RANDOM VARIABLE RANGE (1ms, 60s)
//!NO OF PHILS MUST BE TAKEN FROM THE USER NOT FROM DININGPHIL.H
//!ADD SUPPORT FOR UNIFORM DISTRIBUTION. COMPARE STRINGS
//!TEST ON LINUX
//!DO THE MAKEFILE THING

typedef struct {
    int ID;
    unsigned int think_time; //because sleep() accepts unsigned ints
    unsigned int eat_time;
    int eat_count;
    unsigned int hungry_time;
} philosopher;

void getSticks(void *args) {

    philosopher *actual_args = args; //simpler approach than casting void pointer into philosopher and dereferencing

    pthread_mutex_lock(&mutex);
    philStates[actual_args->ID] = HUNGRY;
    clock_t start_time = clock();
    printf("philosopher %d hungry\n", actual_args->ID);

    while(stickStates[LEFT(actual_args->ID)] == IN_USE || stickStates[RIGHT(actual_args->ID)] == IN_USE) {
        pthread_cond_wait(&cond[actual_args->ID], &mutex);
    }

    clock_t end_time = clock();
    actual_args->hungry_time += end_time - start_time;
    philStates[actual_args->ID] = EATING;
    printf("philosopher %d eating\n", actual_args->ID);

    sleep(actual_args->eat_time);

    stickStates[LEFT(actual_args->ID)] = IN_USE;
    stickStates[RIGHT(actual_args->ID)] = IN_USE;

    pthread_mutex_unlock(&mutex);
}

void putSticks(void *args)
{
    philosopher *actual_args = args; //simpler approach than casting void pointer into philosopher and dereferencing

    pthread_mutex_lock(&mutex);

    stickStates[LEFT(actual_args->ID)] = FREE;
    stickStates[RIGHT(actual_args->ID)] = FREE;

    philStates[actual_args->ID] = THINKING;
    printf("philosopher %d thinking\n", actual_args->ID);

    sleep(actual_args->think_time);
    pthread_cond_signal(&cond[LEFTNEIGHBOR(actual_args->ID)]);
    pthread_cond_signal(&cond[RIGHTNEIGHBOR(actual_args->ID)]);

    pthread_mutex_unlock(&mutex);
}

void *lifeOfAPhilosopher(void *args) { //do not need a seperate struct to store the args. Only argument is the philID

    philosopher *actual_args = args;

    printf("A wild philosopher with ID %d has appeared\n", actual_args->ID);

    for (int i = 0; i < actual_args->eat_count; i++) {
        getSticks(args);
        putSticks(args);
    }

    return 0;
}

int main(int argc, char **argv) {

    //time unit is milliseconds. 1ms <= actionTime <= 60ms
    //const int NO_OF_PHILOSOPHERS = atoi(argv[1]);
    const int minthink = atoi(argv[2]); //minimum thinking time
    const int maxthink = atoi(argv[3]); //maximum thinking time
    const int mineat = atoi(argv[4]); //minimum eating time
    const int maxeat = atoi(argv[5]); //maximum eating time
    const char *const dist = argv[6]; //can be "uniform" or "exponential"
    const int count = atoi(argv[7]); //each philosopher will ear count times

    printf("Number of Philosophers: %d\n", NO_OF_PHILOSOPHERS);
    printf("Minthink Time: %d\n", minthink);
    printf("Maxthink Time: %d\n", maxthink);
    printf("Mineat Time: %d\n", mineat);
    printf("Maxeat Time: %d\n", maxeat);
    printf("Distribution: %s\n", dist);
    printf("Eat Count: %d\n", count);

    assert(NO_OF_PHILOSOPHERS <= 27); //number is philosophers is less than or equal to 27
    assert(NO_OF_PHILOSOPHERS % 2 == 1); //number of philosophers is odd

    double mean = (mineat + maxeat) / 2;

    pthread_t philThread[NO_OF_PHILOSOPHERS];
    philosopher *philosopherList[NO_OF_PHILOSOPHERS];
    int err;

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
        //philosopherList[i]->eat_time = get_exponential_random_value(mean, mineat, maxeat); //generate the eat_time
        //philosopherList[i]->think_time = get_exponential_random_value(mean, mineat, maxeat); //generate the think_time

        philosopherList[i]->eat_time = get_uniform_random_value(mineat, maxeat); //generate the eat_time
        philosopherList[i]->think_time = get_uniform_random_value(mineat, maxeat); //generate the think_time

        philosopherList[i]->hungry_time = 0;

        err = pthread_create(&(philThread[i]), NULL, lifeOfAPhilosopher, philosopherList[i]); //a new philosopher is born...
        assert(!err); //terminate the program if any error occurs during thread creation
    }

    pthread_mutex_init(&mutex, NULL);

    //wait for the philosophers to be done with their lives
    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        pthread_join(philThread[i], NULL);
    }

    for (int i = 0; i < NO_OF_PHILOSOPHERS; i++) {
        printf("philosopher %d duration of hungry state = %u\n", philosopherList[i]->ID, philosopherList[i]->hungry_time);
    }

    return 0;
}