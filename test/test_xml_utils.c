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

static void test_xml_ctx_empty() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	xml_ctx_t *nCtx = xml_ctx_new_empty();

	assert(nCtx->src == NULL);
	assert(nCtx->doc != NULL);
	
	#if debug > 0
		int writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		assert(writtenbytes == 39);
	#endif

	free_xml_ctx_src(&nCtx);

	assert(nCtx == NULL);

	nCtx = xml_ctx_new_empty_root_name("heros");

	assert(nCtx->src == NULL);
	assert(nCtx->doc != NULL);
	
	#if debug > 0
		writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
		assert(writtenbytes == 48);
	#endif

	xmlNodePtr nroot = xmlDocGetRootElement(nCtx->doc);

	xml_ctx_t * nodectx = xml_ctx_new_node(nroot);

	xmlNodePtr noderoot = xmlDocGetRootElement(nodectx->doc);

	assert(nroot != noderoot);
	assert(strcmp((const char *)noderoot->name, (const char *)nroot->name) == 0);

	#if debug > 0
		writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> from node copy BYTES %i\n", writtenbytes);
		assert(writtenbytes == 48);
	#endif

	free_xml_ctx_src(&nCtx);

	free_xml_ctx_src(&nodectx);

	assert(nCtx == NULL);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_file() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	xml_ctx_t *nCtx = xml_ctx_new_file("data\\xml\\basehero.xml");

	assert(nCtx->src == NULL);
	assert(nCtx->doc != NULL);

	#if debug > 1
		int writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	const char *file = "build\\gcc\\dumpbasehero.xml";

	xml_ctx_save_file(nCtx, file);

	free_xml_ctx_src(&nCtx);

	assert(nCtx == NULL);


	nCtx = xml_ctx_new_file(file);

	assert(nCtx->src == NULL);
	assert(nCtx->doc != NULL);

	#if debug > 1
		DEBUG_LOG("WRITTEN FILE:\n");
		writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	free_xml_ctx_src(&nCtx);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_extra_src() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	xml_source_t* result = xml_source_from_resname(ar, "talents");

	assert(result != NULL);

	DEBUG_LOG_ARGS(">>> file type => %s\n", result->data.resfile->type);

	xml_ctx_t *nCtx = xml_ctx_new(result);
	
	assert(nCtx->doc != NULL);
	assert(nCtx->src != NULL && nCtx->src == result);
	assert(nCtx->state.state_no == XML_CTX_SUCCESS);
	assert(nCtx->state.reason == XML_CTX_READ_AND_PARSE);

	free_xml_ctx(&nCtx);

	assert(nCtx == NULL && result != NULL);

	xml_source_free(&result);

	assert(result == NULL);


	result = xml_source_from_resname(ar, "notfound");
	
	nCtx = xml_ctx_new(result);

	assert(nCtx != NULL);
	assert(nCtx->src == NULL && nCtx->src == result);
	assert(nCtx->doc == NULL);
	assert(nCtx->state.state_no == XML_CTX_ERROR);
	assert(nCtx->state.reason == XML_CTX_READ_AND_PARSE);

	free_xml_ctx(&nCtx);
	xml_source_free(&result);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_incl_src() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	xml_source_t* result = xml_source_from_resname(ar, "talents");

	xml_ctx_t *nCtx = xml_ctx_new(result);
	
	assert(nCtx->doc != NULL);
	assert(nCtx->src != NULL && nCtx->src == result);
	assert(nCtx->state.state_no == XML_CTX_SUCCESS);
	assert(nCtx->state.reason == XML_CTX_READ_AND_PARSE);

	free_xml_ctx_src(&nCtx);

	assert(nCtx == NULL); //Be careful with result at this point, i was freed and become a dangling pointer

	result = xml_source_from_resname(ar, "notfound");
	
	nCtx = xml_ctx_new(result);

	assert(nCtx != NULL);
	assert(nCtx->src == NULL && nCtx->src == result);
	assert(nCtx->doc == NULL);
	assert(nCtx->state.state_no == XML_CTX_ERROR);
	assert(nCtx->state.reason == XML_CTX_READ_AND_PARSE);

	free_xml_ctx_src(&nCtx);

	archive_resource_free(&ar);

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

	const char *xpath = "//group[@name = 'Kampf']/*[regexmatch(@name,'Dolche')]";

	xmlXPathObjectPtr xpathObj = xml_ctx_xpath(nCtx, xpath);

	__debug_xpath_obj_ptr(xpathObj);

	xmlXPathObjectPtr xpathObj2 = xml_ctx_xpath(nCtx, xpath);

	assert(xpathObj->nodesetval->nodeTab[0] == xpathObj2->nodesetval->nodeTab[0]);
	DEBUG_LOG_ARGS(">>> %p => %p\n", xpathObj->nodesetval->nodeTab[0] , xpathObj2->nodesetval->nodeTab[0]);

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeObject(xpathObj2);
	free_xml_ctx_src(&nCtx);
	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_xpath_format() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "talents");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	xmlXPathObjectPtr xpathObj = xml_ctx_xpath_format(nCtx, "//group[@name = '%s']/*[regexmatch(@name,'%s')]", "Kampf", "Dolche");

	assert(xml_xpath_has_result(xpathObj));

	__debug_xpath_obj_ptr(xpathObj);

	xmlXPathFreeObject(xpathObj);

	assert(xml_ctx_exist(nCtx, "//group[@name = 'Kampf']/*[regexmatch(@name,'Dolche')]"));
	assert(!xml_ctx_exist(nCtx, "//group[@name = 'Natur']/*[regexmatch(@name,'Dolche')]"));

	assert(xml_ctx_exist_format(nCtx, "//group[@name = '%s']/*[regexmatch(@name,'%s')]", "Kampf", "Dolche"));
	assert(!xml_ctx_exist_format(nCtx, "//group[@name = '%s']/*[regexmatch(@name,'%s')]", "Natur", "Dolche"));

	free_xml_ctx_src(&nCtx);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_add_node_xpath() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "basehero");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	xml_ctx_t *hCtx = xml_ctx_new_empty_root_name("heros");

	xml_ctx_nodes_add_xpath(nCtx, "/hero", hCtx, "/heros");

	#if debug > 1
		int writtenbytes = xmlSaveFileEnc("-", hCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	free_xml_ctx_src(&nCtx);

	result = xml_source_from_resname(ar, "breeds");
	nCtx = xml_ctx_new(result);

	xml_ctx_nodes_add_xpath(nCtx, "/breeds//breed[@name = 'Die Tulamiden']", hCtx, "/heros/hero/breedcontainer");

	#if debug > 1
		writtenbytes = xmlSaveFileEnc("-", hCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	xml_ctx_nodes_add_xpath(nCtx, "/breeds//breed[@name = 'Die Tulamiden']//culture", hCtx, "/heros/*//group");

	#if debug > 1
		writtenbytes = xmlSaveFileEnc("-", hCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	xml_ctx_set_attr_str_xpath(hCtx, (unsigned char *)"New Name Baradon", "//hero/@description");
	xml_ctx_set_attr_str_xpath(hCtx, (unsigned char *)"150", "//hero/config/base-gp/@value");

	xml_ctx_set_attr_str_xpath_format(hCtx, (unsigned char *)"10", "//hero/baseinformations//attribute[regexmatch(@name,'%s')]/@basevalue", "[KkSs]*.*");

	xml_ctx_set_content_xpath(hCtx, (unsigned char *)"Ups falscher Text", "//hero/story/text()");
	xml_ctx_set_content_xpath(hCtx, (unsigned char *)"Das ist die wahre Story von Baradon", "//hero/story/text()");
	xml_ctx_set_content_xpath(hCtx, (unsigned char *)"Ganz neuer Look wa!!!", "//hero/look/text()");

	xml_ctx_set_content_xpath_format(hCtx, (unsigned char *)"ganz neuer Titel", "//hero/%s/text()", "title");

	#if debug > 1
		xmlSaveFileEnc("-", hCtx->doc,"UTF-8");
	#endif

	free_xml_ctx_src(&nCtx);
	free_xml_ctx_src(&hCtx);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}


int 
main() 
{

	DEBUG_LOG(">> Start xml utils tests:\n");
	
	test_xml_ctx_empty();

	test_xml_ctx_file();

	test_xml_ctx_extra_src();

	test_xml_ctx_incl_src();

	test_xml_ctx_xpath();

	test_xml_ctx_xpath_format();

	test_xml_ctx_add_node_xpath();
	
	DEBUG_LOG("<< end xml utils tests:\n");

	return 0;
}

