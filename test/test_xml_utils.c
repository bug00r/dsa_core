#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "dsa_core_defs.h"
#include "xml_source.h"

#include "xml_utils.h"

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

static void test_xml_ctx_extra_src() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	xml_source_t* result = xml_source_from_resname(ar, "talents");

	assert(result != NULL);
	
	DEBUG_LOG_ARGS(">>> file type => %s\n", result->data.resfile->type);

	xml_ctx_t *nCtx = xml_ctx_new(result);
	
	assert(nCtx->doc != NULL);
	assert(nCtx->src != NULL && nCtx->src == result);

	free_xml_ctx(&nCtx);

	assert(nCtx == NULL && result != NULL);

	xml_source_free(&result);

	assert(result == NULL);


	result = xml_source_from_resname(ar, "notfound");
	
	nCtx = xml_ctx_new(result);

	assert(nCtx->src == NULL && nCtx->src == result);
	assert(nCtx->doc == NULL);

	free_xml_ctx(&nCtx);
	xml_source_free(&result);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_incl_src() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	xml_source_t* result = xml_source_from_resname(ar, "talents");

	xml_ctx_t *nCtx = xml_ctx_new(result);
	
	assert(nCtx->doc != NULL);
	assert(nCtx->src != NULL && nCtx->src == result);

	free_xml_ctx_src(&nCtx);

	assert(nCtx == NULL); //Be careful with result at this point, i was freed and become a dangling pointer

	result = xml_source_from_resname(ar, "notfound");
	
	nCtx = xml_ctx_new(result);

	assert(nCtx->src == NULL && nCtx->src == result);
	assert(nCtx->doc == NULL);

	free_xml_ctx_src(&nCtx);

	DEBUG_LOG("<<<\n");
}

static void __debug_xpath_obj_ptr(xmlXPathObjectPtr xpathObj) {
	if ( xpathObj != NULL ) {
		xmlNodeSetPtr nodes = xpathObj->nodesetval;
		int size = (nodes) ? nodes->nodeNr : 0;
		xmlNodePtr cur;
		for(int i = 0; i < size; ++i) {
			cur = nodes->nodeTab[i];
			
			//single property example
			//xmlChar *attr = xmlGetProp(cur, "name");
			//DEBUG_LOG_ARGS("= node \"%s\": type %d : content: %s\n", cur->name, cur->type, attr);
			//xmlFree(attr);
			if(cur == NULL) continue;
			
			xmlAttr * attr = cur->properties;
			
			DEBUG_LOG_ARGS("= node \"%s\": type %d - ", cur->name, cur->type);
			
			while(attr != NULL) {
				xmlChar *sattr = xmlGetProp(cur, attr->name);
				if (sattr != NULL) {
					DEBUG_LOG_ARGS("%s / %s ", attr->name, sattr);
					xmlFree(sattr);
				}
				attr = attr->next;
			}
			DEBUG_LOG("\n");
		}
	}
}

static void test_xml_ctx_xpath() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "talents");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	xmlXPathObjectPtr xpathObj = xml_ctx_xpath(nCtx, "//group[@name = 'Kampf']/*[regexmatch(@name,'Dolche')]");

	__debug_xpath_obj_ptr(xpathObj);

	xmlXPathFreeObject(xpathObj);
	free_xml_ctx_src(&nCtx);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_xpath_format() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "talents");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	xmlXPathObjectPtr xpathObj = xml_ctx_xpath_format(nCtx, "//group[@name = '%s']/*[regexmatch(@name,'%s')]", "Kampf", "Dolche");

	__debug_xpath_obj_ptr(xpathObj);

	xmlXPathFreeObject(xpathObj);
	free_xml_ctx_src(&nCtx);

	DEBUG_LOG("<<<\n");
}

int 
main() 
{

	DEBUG_LOG(">> Start xml source tests:\n");
	
	test_xml_ctx_extra_src();

	test_xml_ctx_incl_src();

	test_xml_ctx_xpath();

	test_xml_ctx_xpath_format();
	
	DEBUG_LOG("<< end xml source tests:\n");
	return 0;
}

