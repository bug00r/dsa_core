#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

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

    DEBUG_LOG("<<<\n");
}


int main(int argc, char **argv) {

	DEBUG_LOG(">> Start dice tests:\n");

	test_dice_alloc_free();

	DEBUG_LOG("<< end dice tests:\n");

	return 0;
}