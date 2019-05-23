#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "dsa_core_defs.h"
#include "xml_source.h"


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

static void test_xml_source() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	xml_source_t* result = xml_source_from_resname(ar, "talents");
	
	assert(result != NULL);
	assert(result->data.resfile != NULL);

	DEBUG_LOG_ARGS("src size (%i == %i)\n", *result->src_size, result->data.resfile->file_size);
	assert(*result->src_size == result->data.resfile->file_size);

	DEBUG_LOG_ARGS("src data pointer (%p == %p)\n", result->src_data, result->data.resfile->data);
	assert(result->src_data == result->data.resfile->data);

	assert( strcmp(result->data.resfile->complete, "xml/talents.xml" ) == 0 );
	
	DEBUG_LOG_ARGS("found file (%s)\n", result->data.resfile->complete);

	xml_source_free(&result);

	assert(result == NULL);

	result = xml_source_from_resname(ar, "notfound");
	
	assert(result == NULL);

	xml_source_free(&result);

	DEBUG_LOG("## create by resource file object\n");

	resource_search_result_t* res_result = archive_resource_search(ar, ".*talents.xml");

	result = xml_source_from_resfile(res_result->files[0]);

	assert(result != NULL);
	assert(result->data.resfile != NULL);

	DEBUG_LOG_ARGS("src size (%i == %i)\n", *result->src_size, result->data.resfile->file_size);
	assert(*result->src_size == result->data.resfile->file_size);

	DEBUG_LOG_ARGS("src data pointer (%p == %p)\n", result->src_data, result->data.resfile->data);
	assert(result->src_data == result->data.resfile->data);

	assert( strcmp(result->data.resfile->complete, "xml/talents.xml" ) == 0 );
	
	DEBUG_LOG_ARGS("found file (%s)\n", result->data.resfile->complete);

	resource_search_result_free(&res_result);

	xml_source_free(&result);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}


int 
main() 
{

	DEBUG_LOG(">> Start xml source tests:\n");
	
	test_xml_source();
	
	DEBUG_LOG("<< end xml source tests:\n");
	return 0;
}

