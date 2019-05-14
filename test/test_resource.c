#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "resource.h"
#include "dsa_core_defs.h"
#include <archive.h>
#include <archive_entry.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

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

static void test_resource_read_archive_items() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	
	resource_search_result_t* result = archive_resource_search(ar, "xml/.*.xml");
	if (result == NULL) return;
	
	DEBUG_LOG_ARGS("found results (%i)\n", result->cnt);
	
	for(unsigned int cnt_files = result->cnt; cnt_files--;) {
		resource_file_t *file = result->files[cnt_files];
		DEBUG_LOG_ARGS("full: %s(%i)\npath: %s(%i)\nfile: %s(%i)\nname: %s(%i)\ntype: %s(%i)\nfile_size: %i\n", file->complete, strlen(file->complete), file->path, strlen(file->path), file->file, strlen(file->file), file->name, strlen(file->name), file->type, strlen(file->type), file->file_size);
	}
	
	resource_search_result_full_free(&result);
	
	archive_resource_free(&ar);
	
	DEBUG_LOG("<<<\n");
}

static void test_resource_read_archive_items_xml() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	
	resource_search_result_t* result = archive_resource_search(ar, ".*talents.*.xml");
	
	if (result == NULL) return;
	DEBUG_LOG_ARGS("found results (%i)\n", result->cnt);
	
	for(unsigned int cnt_files = result->cnt; cnt_files--;) {
		resource_file_t *file = result->files[cnt_files];
		DEBUG_LOG_ARGS("full: %s(%i)\npath: %s(%i)\nfile: %s(%i)\nname: %s(%i)\ntype: %s(%i)\nfile_size: %i\n", file->complete, strlen(file->complete), file->path, strlen(file->path), file->file, strlen(file->file), file->name, strlen(file->name), file->type, strlen(file->type), file->file_size);
		xmlDocPtr doc; /* the resulting document tree */

		doc = xmlReadMemory(file->data, file->file_size, "noname.xml", NULL, 0);
		if (doc == NULL) {
			fprintf(stderr, "Failed to parse document\n");
			return;
		}
		
		#if defined(LIBXML_XPATH_ENABLED) && defined(LIBXML_SAX1_ENABLED)
			DEBUG_LOG("xpath support available :)\n");
			xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
			if ( xpathCtx != NULL ) {
				xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((const xmlChar*)"//talent", xpathCtx);
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
				xmlXPathFreeObject(xpathObj);
			}
			xmlXPathFreeContext(xpathCtx); 
		#else
			DEBUG_LOG("no xpath support :(\n");
		#endif
		    
		xmlFreeDoc(doc);
	}
	
	resource_search_result_full_free(&result);
	
	archive_resource_free(&ar);
	
	DEBUG_LOG("<<<\n");
}

static void test_resource_search_name_xml() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	
	resource_search_result_t* result = archive_resource_search_by_name(ar, "xml/talents.xml");
	
	assert(result->cnt == 1);
	DEBUG_LOG_ARGS("found results (%i)\n", result->cnt);

	resource_search_result_full_free(&result);

	result = archive_resource_search_by_name(ar, "xml/nothinfound.xml");
	
	assert(result->cnt == 0);
	DEBUG_LOG_ARGS("found results (%i)\n", result->cnt);

	resource_search_result_full_free(&result);

	result = archive_resource_search_by_name(ar, ".*talents.*.xml");
	
	assert(result->cnt == 0);
	DEBUG_LOG_ARGS("found results (%i)\n", result->cnt);

	resource_search_result_full_free(&result);

	result = archive_resource_search_by_name(ar, "xml/breeds.xml");
	
	assert(result->cnt == 1);
	DEBUG_LOG_ARGS("found results (%i)\n", result->cnt);

	resource_search_result_full_free(&result);

	DEBUG_LOG("<<<\n");
}

int 
main() 
{

	DEBUG_LOG(">> Start resource tests:\n");
	
	test_resource_read_archive_items();
	
	test_resource_read_archive_items_xml();

	test_resource_search_name_xml();
	
	DEBUG_LOG("<< end resource tests:\n");
	return 0;
}

