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

    assert(regex_match((const unsigned char *)"[\\d\\w]+", (const unsigned char *) "test bla bla bla"));

	assert(regex_not_blank((const unsigned char *)" 23 bla bla blubb sd"));
	assert(!regex_not_blank((const unsigned char *)"    "));
	assert(!regex_not_blank((const unsigned char *)""));
	assert(!regex_not_blank((const unsigned char *)"\t"));

	/*
	negative lookahead
	q(?!u) 		=> 
	(?!(regex)) =< NOT HERE :(

	positive lookahead
	q(?=u)		=> matches q followed by u
	(?=(regex)) => ist possible too

	pos look behind
	(?<=a)b		=> matches b preceded by a

	neg look behind
	(?<!a)b		=> matches b not preceded by an a

	*/
	const unsigned char * regex = (const unsigned char *)"^(\\d+)$|^(\\d+-\\d+)$";

	assert(regex_match(regex, (const unsigned char *)"12"));
	assert(regex_match(regex, (const unsigned char *)"1"));
	assert(regex_match(regex, (const unsigned char *)"1-2"));
	assert(regex_match(regex, (const unsigned char *)"12-12"));
	assert(regex_match(regex, (const unsigned char *)"1-12"));
	assert(regex_match(regex, (const unsigned char *)"12-1"));
	assert(!regex_match(regex,(const unsigned char *)"-12"));
	assert(!regex_match(regex,(const unsigned char *)"1-"));

	assert(regex_range_match((const unsigned char *)"12", (const unsigned char *)"12"));
	assert(regex_range_match((const unsigned char *)"1", (const unsigned char *)"1"));
	assert(regex_range_match((const unsigned char *)"1-2", (const unsigned char *)"1"));
	assert(regex_range_match((const unsigned char *)"1-2", (const unsigned char *)"2"));
	assert(regex_range_match((const unsigned char *)"1-12", (const unsigned char *)"4"));
	assert(regex_range_match((const unsigned char *)"1-12", (const unsigned char *)"3"));
	assert(!regex_range_match((const unsigned char *)"1-12", (const unsigned char *)"13"));
	assert(!regex_range_match((const unsigned char *)"12-1", (const unsigned char *)"1"));
	assert(!regex_range_match((const unsigned char *)"12", (const unsigned char *)"1"));
	assert(!regex_range_match((const unsigned char *)"-1", (const unsigned char *)"-1"));
	assert(!regex_range_match((const unsigned char *)"1-", (const unsigned char *)"1"));

	DEBUG_LOG("<< end taw tests:\n");

	return 0;
}
