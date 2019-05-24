#include "number_utils.h"

float nu_random_min_max(const float lnum, const float hnum){
	return lnum + ((float)rand()/((float)(RAND_MAX)/(hnum-lnum)));
}

float nu_random_zero_max(const float seed){
	return (((float)rand()/(float)(RAND_MAX)) * 2.f * seed) - seed;
}