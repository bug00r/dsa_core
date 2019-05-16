#include "xml_utils.h"

static xml_ctx_t* __xml_ctx_create(const xml_source_t *xml_src, xmlDocPtr doc) {
    xml_ctx_t temp = {xml_src, doc};
    xml_ctx_t * new_ctx = malloc(sizeof(xml_ctx_t));
    memcpy(new_ctx, &temp, sizeof(xml_ctx_t));
    return new_ctx;
}

static void __xml_ctx_set_state(xml_ctx_t * ctx,  xml_ctx_state_no_t state_no, xml_ctx_state_reason_t  reason ) {
    ctx->state.state_no = state_no;
    ctx->state.reason   = reason;
}

static void __xml_ctx_set_state_ptr(xml_ctx_t * ctx,  xml_ctx_state_no_t *state_no, xml_ctx_state_reason_t *reason ) {
    __xml_ctx_set_state(ctx, *state_no, *reason);
}



xml_ctx_t* xml_ctx_new(const xml_source_t *xml_src) {

    xmlDocPtr doc = NULL;

    xml_ctx_state_no_t state_no = XML_CTX_SUCCESS; 
    xml_ctx_state_reason_t reason = XML_CTX_READ_AND_PARSE;

    if ( xml_src != NULL && xml_src->src_data != NULL && xml_src->src_size > 0 ) {

        doc = xmlReadMemory(xml_src->src_data, *xml_src->src_size, "noname.xml", NULL, 0);
        
    } else {
        state_no = XML_CTX_ERROR; 
        reason = XML_CTX_READ_AND_PARSE;
    }
    
    xml_ctx_t *new_ctx = __xml_ctx_create(xml_src, doc);
    __xml_ctx_set_state_ptr(new_ctx, &state_no, &reason);

    return new_ctx;
}


void free_xml_ctx(xml_ctx_t **ctx) {
    
    if ( ctx != NULL && *ctx != NULL ) {
        xml_ctx_t *todelete_ctx = *ctx;
        
        if (todelete_ctx->doc) {
            xmlFreeDoc(todelete_ctx->doc);
        }

        free(todelete_ctx);
        *ctx = NULL;
    }
}


void free_xml_ctx_src(xml_ctx_t **ctx) {
    if ( ctx != NULL && *ctx != NULL ) {
        xml_ctx_t *todelete_ctx = *ctx;

        xml_source_t * _src = (xml_source_t*)todelete_ctx->src;

        free_xml_ctx(ctx);

        if(_src) {
            xml_source_free(&_src);
        }
    }
}

/*
xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
			xmlXPathRegisterAllFunctions(xpathCtx);
			xmlXPathRegisterFunc(xpathCtx,(const xmlChar *) "regexmatch", regexmatch_xpath_func);
			
			#if 0
			//xmlXPathRegisterNs(xpathCtx, (xmlChar *)"re", (xmlChar *)"http://exslt.org/regular-expressions");
			#endif

			if ( xpathCtx != NULL ) {
				
				
				char *gen_xpath = NULL;
				if ( IupGetInt(groups, "VALUE") == 1 ) {

					#if 0
					//regex is not working in current version, have to add own regex function
					//gen_xpath = format_string_new("//group/*[contains(@name,'%s')]", search_string);
					#endif

					gen_xpath = format_string_new("//group/*[regexmatch(@name,'%s')]", search_string);
					
				} else {

					#if 0
					//regex is not working in current version, have to add own regex function
					//gen_xpath = format_string_new("//group[@name = '%s']/*[contains(@name,'%s')]", selected_group, search_string);
					#endif

					gen_xpath = format_string_new("//group[@name = '%s']/*[regexmatch(@name,'%s')]", selected_group, search_string);
					
				}
				
				#if debug > 0
					printf("used search xpath: %s\n", gen_xpath);
				#endif
				
				xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((const xmlChar*)gen_xpath, xpathCtx);
				free(gen_xpath);
				
				lsrs->xpath_result[cur_file] = xpathObj;
					
			}
			
			xmlXPathFreeContext(xpathCtx);
*/
xmlXPathObjectPtr xml_ctx_xpath( const xml_ctx_t *ctx, const char *xpath) {

    xmlXPathObjectPtr result = NULL;

    if(ctx->doc && xpath) {
        
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext(ctx->doc);
        xmlXPathRegisterAllFunctions(xpathCtx);
        xmlXPathRegisterFunc(xpathCtx,(const xmlChar *) "regexmatch", regexmatch_xpath_func);

        if ( xpathCtx != NULL ) {
            
            result = xmlXPathEvalExpression((const xmlChar*)xpath, xpathCtx);
                            
        }
        
        xmlXPathFreeContext(xpathCtx);
    }

    return result;
}


xmlXPathObjectPtr xml_ctx_xpath_format( const xml_ctx_t *ctx, const char *xpath_format, ...) {
    
    va_list args;
    va_start(args,xpath_format);
    char *gen_xpath = format_string_va_new(xpath_format, args);
    va_end(args);

    #if debug > 0
        printf("gen xpath: %s\n", gen_xpath);
    #endif

    xmlXPathObjectPtr result = xml_ctx_xpath(ctx, gen_xpath);

    free(gen_xpath);
    
    return result;
}