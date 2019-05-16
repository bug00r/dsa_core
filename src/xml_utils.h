#ifndef XML_UTILS_H
#define XML_UTILS_H

extern int blubb;

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
    XML_CTX_NO_REASON,      /* reason for no reason oO */
    XML_CTX_READ_AND_PARSE  /* reason for missing or invalid source: null pointer oder src size = 0 */
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

xmlXPathObjectPtr xml_ctx_xpath( const xml_ctx_t *ctx, const char *xpath);
xmlXPathObjectPtr xml_ctx_xpath_format( const xml_ctx_t *ctx, const char *xpath_format, ...);

#endif