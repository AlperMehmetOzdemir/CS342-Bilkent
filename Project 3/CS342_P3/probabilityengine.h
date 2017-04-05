#include <math.h>

#ifndef CS342_P3_PROBABILITYENGINE_H
#define CS342_P3_PROBABILITYENGINE_H
#endif //CS342_P3_PROBABILITYENGINE_H

//Generate random number between 0 and 1 http://stackoverflow.com/questions/6218399/how-to-generate-a-random-number-between-0-and-1
#define RAND (double)rand() / (double)RAND_MAX

#define get_uniform_random_value(lower_bound, upper_bound) (unsigned int)((rand() % (upper_bound + 1 - lower_bound)) + lower_bound)

//exponential random value generation http://stackoverflow.com/questions/11491458/how-to-generate-random-numbers-with-exponential-distribution-with-mean
unsigned int get_exponential_random_value(double mean, int lower_bound, int upper_bound) {

    double val = -log(RAND)/(1/mean);

    while (val < lower_bound || val > upper_bound) {
        val = -log(RAND)/(1/mean);
    }

    return (unsigned int)val;
}