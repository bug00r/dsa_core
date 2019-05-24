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

static bool __xml_ctx_valid( xml_ctx_t *ctx ) {
    bool isvalid = true;

    if ( ctx == NULL || ctx->doc == NULL) {
    
        __xml_ctx_set_state(ctx, XML_CTX_ERROR, XML_CTX_SRC_INVALID);
        isvalid = false;
    
    } else {
    
        __xml_ctx_set_state(ctx, XML_CTX_SUCCESS, XML_CTX_NO_REASON);
    
    }
    return isvalid;
}

static bool __xml_ctx_xpath_valid( xml_ctx_t *ctx, const char *xpath) {
    bool isvalid = true;

    if ( xpath == NULL || (strlen(xpath) == 0) ) {
        __xml_ctx_set_state(ctx, XML_CTX_ERROR, XML_CTX_XPATH_INVALID);
        isvalid = false;
    
    } else {
    
        __xml_ctx_set_state(ctx, XML_CTX_SUCCESS, XML_CTX_NO_REASON);
    
    }
    return isvalid;
}


xml_ctx_t* xml_ctx_new_empty() {

    xmlDocPtr doc = xmlNewDoc("1.0");

    xml_ctx_t *new_ctx = __xml_ctx_create(NULL, doc);
    __xml_ctx_set_state(new_ctx, XML_CTX_SUCCESS, XML_CTX_READ_AND_PARSE);

    return new_ctx;
}

xml_ctx_t* xml_ctx_new_empty_root_name(const char* rootname) {
    xml_ctx_t* new_ctx = xml_ctx_new_empty();

    if(rootname && strlen(rootname) > 0) {
        xmlNodePtr newroot = xmlNewNode(NULL, rootname);
        xmlDocSetRootElement(new_ctx->doc, newroot);
    }

    return new_ctx;
}


xml_ctx_t* xml_ctx_new(const xml_source_t *xml_src) {

    xmlDocPtr doc = NULL;

    xml_ctx_state_no_t state_no = XML_CTX_SUCCESS; 
    xml_ctx_state_reason_t reason = XML_CTX_READ_AND_PARSE;

    if ( xml_src != NULL && xml_src->src_data != NULL && xml_src->src_size > 0 ) {

        doc = xmlReadMemory(xml_src->src_data, *xml_src->src_size, "noname.xml", NULL, 0);
        
    } else {
        state_no = XML_CTX_ERROR; 
    }
    
    xml_ctx_t *new_ctx = __xml_ctx_create(xml_src, doc);
    __xml_ctx_set_state_ptr(new_ctx, &state_no, &reason);

    return new_ctx;
}

xml_ctx_t* xml_ctx_new_node(const xmlNodePtr rootnode) {
    xml_ctx_t *new_ctx = xml_ctx_new_empty();
    xmlNodePtr copyroot = xmlCopyNode(rootnode, 1);
    xmlDocSetRootElement(new_ctx->doc ,copyroot);
    return new_ctx;
}

xml_ctx_t* xml_ctx_new_file(const char *filename) {
    xml_ctx_t *new_ctx = xml_ctx_new_empty();
    
    new_ctx->doc = xmlReadFile(filename, "UTF-8", 0);

    xml_ctx_state_no_t state_no = XML_CTX_SUCCESS; 
    xml_ctx_state_reason_t reason = XML_CTX_READ_AND_PARSE;

    xmlErrorPtr error = xmlGetLastError(); 

    if (xmlGetLastError() != NULL) {
        state_no = XML_CTX_ERROR;
    }

    __xml_ctx_set_state_ptr(new_ctx, &state_no, &reason);

    return new_ctx;
}

void xml_ctx_save_file(const xml_ctx_t *ctx, const char *filename) {
    
    if (ctx != NULL && ctx->doc != NULL && filename != NULL && ( strlen(filename) > 0 )) {

        if ( xmlSaveFileEnc(filename, ctx->doc, "UTF-8") == -1 ) {
            
            xml_ctx_state_no_t state_no = XML_CTX_SUCCESS; 
            xml_ctx_state_reason_t reason = XML_CTX_READ_AND_PARSE; 

            if (xmlGetLastError() != NULL) {
                state_no = XML_CTX_ERROR;
            }

            __xml_ctx_set_state_ptr((xml_ctx_t *)ctx, &state_no, &reason);
        }

    }

}

void free_xml_ctx(xml_ctx_t **ctx) {
    
    if ( ctx != NULL && *ctx != NULL ) {
        xml_ctx_t *todelete_ctx = *ctx;
        
        if (todelete_ctx->doc) {
            xmlDocGetRootElement(todelete_ctx->doc);
            xmlFreeDoc(todelete_ctx->doc);
        }

        free(todelete_ctx);
        *ctx = NULL;
    }
}
void free_xml_ctx_ptr(xml_ctx_t *ctx) {
    if ( ctx != NULL ) {
        xml_ctx_t *todelete_ctx = ctx;
        
        if (todelete_ctx->doc) {
            xmlDocGetRootElement(todelete_ctx->doc);
            xmlFreeDoc(todelete_ctx->doc);
        }

        free(todelete_ctx);
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

xmlXPathObjectPtr xml_ctx_xpath( const xml_ctx_t *ctx, const char *xpath) {

    xmlXPathObjectPtr result = NULL;

    if(ctx->doc && xpath) {
        
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext(ctx->doc);
        xmlXPathRegisterAllFunctions(xpathCtx);
        xmlXPathRegisterFunc(xpathCtx,(const xmlChar *) "regexmatch", regexmatch_xpath_func);
        xmlXPathRegisterFunc(xpathCtx,(const xmlChar *) "max", max_xpath_func); 

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

void xml_ctx_nodes_add_xpath(xml_ctx_t *src, const char *src_xpath, xml_ctx_t *dst, const char *dst_xpath) {
    
    if ( !__xml_ctx_valid(src) || !__xml_ctx_valid(dst) ) return;
    
    if ( !__xml_ctx_xpath_valid(src, src_xpath) || !__xml_ctx_xpath_valid(dst, dst_xpath) ) return;

    xmlXPathObjectPtr srcxpres = xml_ctx_xpath(src, src_xpath);

    if ( srcxpres != NULL && srcxpres->nodesetval != NULL && srcxpres->nodesetval->nodeNr > 0 ) {

        const int numsrcs = srcxpres->nodesetval->nodeNr;
        xmlNodePtr * sources = srcxpres->nodesetval->nodeTab;

        xmlXPathObjectPtr dstxpres = xml_ctx_xpath(dst, dst_xpath);

        if ( dstxpres != NULL && dstxpres->nodesetval != NULL && dstxpres->nodesetval->nodeNr > 0 ) {

            for(int cursrcnum = 0; cursrcnum < numsrcs; ++cursrcnum) {
                
                #if debug > 1
                    printf("source: %i of %i\n",cursrcnum , numsrcs);
                #endif

                xmlNodePtr cursrc = sources[cursrcnum];

                const int numtargets = dstxpres->nodesetval->nodeNr;
                xmlNodePtr * targets = dstxpres->nodesetval->nodeTab;

                for(int curtargetnum = 0; curtargetnum < numtargets; ++curtargetnum) {
                    
                    xmlNodePtr curtarget = targets[curtargetnum];

                    xmlNodePtr result = NULL;
                    xmlNodePtr copy = NULL;

                    #if debug > 1
                            printf("target: %i of %i\n",curtargetnum , numtargets);
                    #endif

                    if ( numtargets == 1 ) {
                        
                        #if debug > 1
                            printf("target is node!!! \n");
                        #endif

                        copy = xmlCopyNode(cursrc, 1);
                        result = xmlAddChild(curtarget, copy);
                    } else {

                        #if debug > 1
                            printf("target is list!!! \n");
                        #endif

                        copy = xmlCopyNodeList(cursrc);
                        result = xmlAddChildList(curtarget, copy);
                    }
                    
                    if(result == NULL) {
                        __xml_ctx_set_state(dst, XML_CTX_ERROR, XML_CTX_ADD);
                    } else {
                        __xml_ctx_set_state(dst, XML_CTX_SUCCESS, XML_CTX_ADD);
                    }

                }
            }
        }

        xmlXPathFreeObject(dstxpres);
    }
    xmlXPathFreeObject(srcxpres);
}
