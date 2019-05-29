#ifndef XML_UTILS_H
#define XML_UTILS_H

#if 0
    Concept global, just for remembering target: 
        - Utils have to work with xml sources. So we need xml_source_t with different loading interfaces. At first from memory or memory Wrapper Pattern
          in this case from resource too.
        - the source will be used inside an xml_context_t. The context will load the libxml DocPtr from source.
        - an xml_context_t can be used inside of services. We want to have xml_services or interfaces at first for:
            - search based on xpath(including regex)
            - search based on Attribute Names and values(including regex and regular(string matching))
            - editing node(add delete change) and Attributes

        - As next we need some different context services/Interfaces
            - A Context can be:
                - Breed, Culture, Profession....etc.
            - a context service may use other context services as well. 
        
#endif

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "xpath_utils.h"

#include "xml_source.h"

typedef enum {
    XML_CTX_SUCCESS,    /* operation was successfully */
    XML_CTX_ERROR,      /* operation causes error */
    XML_CTX_NO_STATE    /* operation does not set state because without a reson */
} xml_ctx_state_no_t;

typedef enum {
    XML_CTX_NO_REASON,          /* reason for no reason oO */
    XML_CTX_READ_AND_PARSE,     /* reason for missing or invalid source: null pointer oder src size = 0 */
    XML_CTX_SRC_INVALID,        /* for src context is invalid, like missing src or doc pointer */
    XML_CTX_XPATH_INVALID,      /* for src xpath is invalid(NULL) */
    XML_CTX_ADD
} xml_ctx_state_reason_t;

typedef struct {
    xml_ctx_state_no_t      state_no;
    xml_ctx_state_reason_t  reason;
} xml_ctx_state_t;

typedef struct {
    const xml_source_t * const src; /* used xml source */
    xmlDocPtr  doc;                 /* parsed xml doc from given source */
    xml_ctx_state_t state;          /* state of the last operation */
} xml_ctx_t;

/*

    This Function creates a new xml context with given xml_source.
    If there are arose some xml loading issues this function will
    return NULL and set some error code to given pointer

    Parameter:

    name            description
    ------------------------------------------------------------
    ctx             pointer to xml context pointer

    returns new xml context in every case with given state

*/
xml_ctx_t* xml_ctx_new(const xml_source_t *xml_src);

/*

    This Function creates a new xml context without xml source.
    XML Source will be set NULL and a new Document will created.

    returns new xml context in every case with given state

*/
xml_ctx_t* xml_ctx_new_empty();

/*

    This Function creates a new xml context without xml source.
    XML Source will be set NULL and a new Document will created.

    Parameter:

    name            description
    ------------------------------------------------------------
    rootname        name of the root element

    returns new xml context in every case with given state

*/
xml_ctx_t* xml_ctx_new_empty_root_name(const char* rootname);

/*

    This Function creates a new xml context without xml source.
    XML Source will be set NULL and a new Document will created.
    Documents root node will be a copy root node.

    Parameter:

    name            description
    ------------------------------------------------------------
    rootnode        root node of new Document

    returns new xml context in every case with given state

*/
xml_ctx_t* xml_ctx_new_node(const xmlNodePtr rootnode);

/*

    This Function creates a new xml context without xml source.
    XML Source will be set NULL and a new Document will created.
    The Document will be read from file name in utf-8.

    Parameter:

    name            description
    ------------------------------------------------------------
    filename        name of xml File

    returns new xml context in every case with given state

*/
xml_ctx_t* xml_ctx_new_file(const char *filename);

/*

    This Function will save current context doc to file as utf-8.

    Parameter:

    name            description
    ------------------------------------------------------------
    ctx             give xml context to write
    filename        name of xml File target

    returns new xml context in every case with given state

*/
void xml_ctx_save_file(const xml_ctx_t *ctx, const char *filename);

/*

    This Function frees the memory from xml_ctx_t and all its given attributes, BUT
    not the used xml_xource. You have toe free it extra. This function is useful
    if you want to create a new xml context based on the same source.

    Hint: You have to save the pointer before freeing context.

    After this Operation the overgiven pointer will set with NULL.

    Parameter:

    name            description
    ------------------------------------------------------------
    ctx             pointer to xml context pointer

*/
void free_xml_ctx(xml_ctx_t **ctx);
void free_xml_ctx_ptr(xml_ctx_t *ctx);

/*

    This Function frees the memory from xml_ctx_t and all its given attributes AND
    the used xml_xource.

    After this Operation the overgiven pointer will set with NULL.

    Parameter:

    name            description
    ------------------------------------------------------------
    ctx             pointer to xml context pointer

*/
void free_xml_ctx_src(xml_ctx_t **ctx);

/*

    This Function executes an xpath against xml context document.

    Parameter:

    name            description
    ------------------------------------------------------------
    ctx             pointer to xml context pointer
    xpath           xpath for execution

    returns a xmlXPathObjectPtr with xpath result
*/
xmlXPathObjectPtr xml_ctx_xpath( const xml_ctx_t *ctx, const char *xpath);

/*

    This Function executes an xpath against xml context document.

    Parameter:

    name            description
    ------------------------------------------------------------
    ctx             pointer to xml context pointer
    xpath           xpath format string
    ...             xpath format parameter

    returns a xmlXPathObjectPtr with xpath result
*/
xmlXPathObjectPtr xml_ctx_xpath_format( const xml_ctx_t *ctx, const char *xpath_format, ...);
xmlXPathObjectPtr xml_ctx_xpath_format_va( const xml_ctx_t *ctx, const char *xpath_format, va_list argptr);

/*

    This Function is searching nodes from src by using src_xpath and add them
    to dst context to node which was found by dst_xpath.

    This function will set given states and reasons into src or dst context,
    this is based on the error side like:

    Source xpatch does not match any node this will saved in src context. The 
    same will be with the destination ctx.

    Parameter:

    name            description
    ------------------------------------------------------------
    src             source context
    src_xpath       source context used xpath expression
    dst             destination context
    dst_xpath       destination context used xpath expression

*/
void xml_ctx_nodes_add_xpath(xml_ctx_t *src, const char *src_xpath, xml_ctx_t *dst, const char *dst_xpath);

void xml_ctx_rem_nodes_xpres(xmlXPathObjectPtr xpres);
void xml_ctx_remove(xml_ctx_t *ctx, const char *xpath);
void xml_ctx_remove_format(xml_ctx_t *ctx, const char *xpath_format, ...);

bool xml_ctx_exist(xml_ctx_t *ctx, const char *xpath);
bool xml_ctx_exist_format(xml_ctx_t *ctx, const char *xpath_format, ...);

bool xml_xpath_has_result(xmlXPathObjectPtr xpathobj);

#endif