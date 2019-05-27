#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "regex_utils.h"

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


int main(int argc, char **argv) {

	DEBUG_LOG(">> Start taw tests:\n");

    assert(regex_match((const unsigned char *)"[\\d\\D]+", (const unsigned char *) "test bla bla bla"));

	DEBUG_LOG("<< end taw tests:\n");

	return 0;
}