#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "dice.h"

#ifndef DEBUG_LOG_ARGS
	#if debug != 0
		#define DEBUG_LOG_ARGS(fmt, ...) printf((fmt), __VA_ARGS__)
	#else
		#define DEBUG_LOG_ARGS(fmt, ...)
	#endif
#endif

#ifndef DEBUG_LOG
	#if debug != 0
		#define DEBUG_LOG(msg) printf((msg))
	#else
		#define DEBUG_LOG(msg)
	#endif
#endif

static void test_dice_alloc_free() {
    DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	const char * invalid_dice = "blabla";
	dice_t* dice = dsa_dice_new(invalid_dice);

	assert(strcmp(dice->raw,invalid_dice) == 0);
	assert(dice->first == NULL);

	dsa_dice_free(&dice);

	assert(dice == NULL);

	const char * valid_dice = "3";

	dice = dsa_dice_new(valid_dice);

	assert(strcmp(dice->raw,valid_dice) == 0);
	assert(dice->first != NULL);
	assert(dice->first->data.factor == 1);
	assert(dice->first->data.max == 3);
	assert(dsa_dice_roll(dice) == 3);

	dsa_dice_free(&dice);

	dice = dsa_dice_new("-3");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == -1);
	assert(dice->first->data.max == 3);
	assert(dsa_dice_roll(dice) == -3);

	dsa_dice_free(&dice);

	dice = dsa_dice_new("2w6");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == 1);
	assert(dice->first->data.cnt == 2);
	assert(dice->first->data.max == 6);
	assert(dice->first->next == NULL);
	int result = dsa_dice_roll(dice);

	#if debug > 0
		for (int i = 0; i < 10 ; ++i) {
			printf("2w6 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert((result >= 2) && (result <= 12));

	dsa_dice_free(&dice);

	dice = dsa_dice_new("-2w6");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == -1);
	assert(dice->first->data.cnt == 2);
	assert(dice->first->data.max == 6);
	assert(dice->first->next == NULL);
	result = dsa_dice_roll(dice);

	#if debug > 0
		for (int i = 0; i < 10 ; ++i) {
			printf("-2w6 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert((result >= -12) && (result <= -2));

	dsa_dice_free(&dice);

	dice = dsa_dice_new("6+6");

	
	assert(dice->first != NULL);
	assert(dice->first->data.factor == 1);
	assert(dice->first->data.cnt == 0);
	assert(dice->first->data.max == 6);
	assert(dice->first->next != NULL);
	assert(dice->first->next->data.factor == 1);
	assert(dice->first->next->data.cnt == 0);
	assert(dice->first->next->data.max == 6);
	assert(dice->first->next->next == NULL);
	result = dsa_dice_roll(dice);

	#if debug > 0
		for (int i = 0; i < 10 ; ++i) {
			printf("6+6 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert(result == 12);

	dsa_dice_free(&dice);
	
	dice = dsa_dice_new("-6-6");

	
	assert(dice->first != NULL);
	assert(dice->first->data.factor == -1);
	assert(dice->first->data.cnt == 0);
	assert(dice->first->data.max == 6);
	assert(dice->first->next != NULL);
	assert(dice->first->next->data.factor == -1);
	assert(dice->first->next->data.cnt == 0);
	assert(dice->first->next->data.max == 6);
	assert(dice->first->next->next == NULL);
	result = dsa_dice_roll(dice);

	#if debug > 0
		for (int i = 0; i < 10 ; ++i) {
			printf("-6-6 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert(result == -12);

	dsa_dice_free(&dice);

	dice = dsa_dice_new("2w6+2w6");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == 1);
	assert(dice->first->data.cnt == 2);
	assert(dice->first->data.max == 6);
	assert(dice->first->next != NULL);
	assert(dice->first->next->data.factor == 1);
	assert(dice->first->next->data.cnt == 2);
	assert(dice->first->next->data.max == 6);
	assert(dice->first->next->next == NULL);
	result = dsa_dice_roll(dice);

	#if debug > 0
		for (int i = 0; i < 10 ; ++i) {
			printf("2w6+2w6 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert((result >= 4) && (result <= 24));

	dsa_dice_free(&dice);

	dice = dsa_dice_new("-2w6-2w6");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == -1);
	assert(dice->first->data.cnt == 2);
	assert(dice->first->data.max == 6);
	assert(dice->first->next != NULL);
	assert(dice->first->next->data.factor == -1);
	assert(dice->first->next->data.cnt == 2);
	assert(dice->first->next->data.max == 6);
	assert(dice->first->next->next == NULL);
	result = dsa_dice_roll(dice);

	#if debug > 0
		for (int i = 0; i < 10 ; ++i) {
			printf("-2w6-2w6 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert((result >= -24) && (result <= -4));

	dsa_dice_free(&dice);

	dice = dsa_dice_new("5w6+5");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == 1);
	assert(dice->first->data.cnt == 5);
	assert(dice->first->data.max == 6);
	assert(dice->first->next != NULL);
	assert(dice->first->next->data.factor == 1);
	assert(dice->first->next->data.cnt == 0);
	assert(dice->first->next->data.max == 5);
	assert(dice->first->next->next == NULL);
	result = dsa_dice_roll(dice);

	#if debug > 0
		for (int i = 0; i < 10 ; ++i) {
			printf("5w6+5 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert((result >= 10) && (result <= 35));

	dsa_dice_free(&dice);

	dice = dsa_dice_new("5w6+3w10+4");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == 1);
	assert(dice->first->data.cnt == 5);
	assert(dice->first->data.max == 6);
	assert(dice->first->next != NULL);
	assert(dice->first->next->data.factor == 1);
	assert(dice->first->next->data.cnt == 3);
	assert(dice->first->next->data.max == 10);
	assert(dice->first->next->next != NULL);
	assert(dice->first->next->next->data.factor == 1);
	assert(dice->first->next->next->data.cnt == 0);
	assert(dice->first->next->next->data.max == 4);
	assert(dice->first->next->next->next == NULL);
	result = dsa_dice_roll(dice);

	#if debug > 0
		printf("new part:\n");
		for (int i = 0; i < 10 ; ++i) {
			printf("5w6+3w10+4 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert((result >= 12) && (result <= 64));

	dsa_dice_free(&dice);

	dice = dsa_dice_new("-2w6+3w20+4-3w6+5");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == -1);
	assert(dice->first->data.cnt == 2);
	assert(dice->first->data.max == 6);
	assert(dice->first->next != NULL);
	assert(dice->first->next->data.factor == 1);
	assert(dice->first->next->data.cnt == 3);
	assert(dice->first->next->data.max == 20);
	assert(dice->first->next->next != NULL);
	assert(dice->first->next->next->data.factor == 1);
	assert(dice->first->next->next->data.cnt == 0);
	assert(dice->first->next->next->data.max == 4);

	assert(dice->first->next->next->next != NULL);
	assert(dice->first->next->next->next->data.factor == -1);
	assert(dice->first->next->next->next->data.cnt == 3);
	assert(dice->first->next->next->next->data.max == 6);
	assert(dice->first->next->next->next->next != NULL);

	assert(dice->first->next->next->next->next->data.factor == 1);
	assert(dice->first->next->next->next->next->data.cnt == 0);
	assert(dice->first->next->next->next->next->data.max == 5);
	assert(dice->first->next->next->next->next->next == NULL);

	result = dsa_dice_roll(dice);

	#if debug > 0
		printf("new part:\n");
		for (int i = 0; i < 10 ; ++i) {
			printf("-2w6+3w20+4-3w6+5 roll = %i\n",dsa_dice_roll(dice));
		}		
	#endif

	assert((result >= -18) && (result <= 64));

	dsa_dice_free(&dice);

	dice = dsa_dice_new("W20");

	assert(dice->first != NULL);
	assert(dice->first->data.factor == 1);
	assert(dice->first->data.cnt == 1);
	assert(dice->first->data.max == 20);
	assert(dice->first->next == NULL);
	result = dsa_dice_roll(dice);

	#if debug > 0
		printf("W20 roll = %i\n",result);	
	#endif

	assert((result >= 1) && (result <= 20));

	dsa_dice_free(&dice);

    DEBUG_LOG("<<<\n");
}


int main(int argc, char **argv) {

	DEBUG_LOG(">> Start dice tests:\n");

	srand(time(NULL));

	test_dice_alloc_free();

	DEBUG_LOG("<< end dice tests:\n");

	return 0;
}