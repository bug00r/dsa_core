#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "taw.h"

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

static void test_taw_alloc_free() {
    DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

    taw_col_t column = A_;

    taw_result_t *taw_result = taw_calc(&column, 0, 1);

    assert(taw_result->cnt_details == 1);

	taw_result_free(&taw_result);

    DEBUG_LOG("<<<\n");
}

static void test_taw_col_A_() {
    DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

    taw_col_t column = A_;

    taw_result_t *taw_result = taw_calc(&column, 0, 10);

    assert(taw_result->cnt_details == 10);
    assert(taw_result->complete->ap == 48);

	taw_result_free(&taw_result);

    taw_result = taw_calc(&column, -3, 10);

    assert(taw_result->cnt_details == 13);
    assert(taw_result->complete->ap == 63);

	taw_result_free(&taw_result);

    DEBUG_LOG("<<<\n");
}

int main(int argc, char **argv) {

	DEBUG_LOG(">> Start taw tests:\n");

	test_taw_alloc_free();

    test_taw_col_A_();

	DEBUG_LOG("<< end taw tests:\n");

	return 0;
}