#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "hero.h"

#include "dsa_core_defs.h"

EXTERN_BLOB(zip_resource, 7z);

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

static void test_heros_alloc_free() {
    DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	assert(heros != NULL );
	assert(heros->basehero != NULL );
	assert(heros->breeds != NULL );
	assert(heros->cultures != NULL );
	assert(heros->professions != NULL );
	assert(heros->pro_contra != NULL );
	assert(heros->specialabilities != NULL );
	assert(heros->spells != NULL );
	assert(heros->talents != NULL );
	assert(heros->heros != NULL);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

    DEBUG_LOG("<<<\n");
}

static void test_heros_add_hero() {
	    DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, "Baradon");

	#if debug > 0
		int writtenbytes = xmlSaveFileEnc("-", heros->heros->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	dsa_hero_free(&baradon);

	assert(baradon == NULL);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

    DEBUG_LOG("<<<\n");
}

int main(int argc, char **argv) {

	DEBUG_LOG(">> Start taw tests:\n");

	test_heros_alloc_free();

	test_heros_add_hero();

	DEBUG_LOG("<< end taw tests:\n");

	return 0;
}