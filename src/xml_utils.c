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

static void __xml_ctx_attr_str_xpptr(xmlXPathObjectPtr node, const unsigned char *value) {

    if ( xml_xpath_has_result(node) ) {

        const int maxNodes = node->nodesetval->nodeNr;

        xmlNodePtr *nodes = node->nodesetval->nodeTab;
        
        for (int curattr = 0;curattr < maxNodes ; ++curattr) {
            
            xmlNodePtr node = nodes[curattr];
            
            if (node->type == XML_ATTRIBUTE_NODE) {

                xmlSetProp(node->parent, node->name, (xmlChar*)value);
            
            }
        
        }
    }
}

static void __xml_ctx_content_xpptr(xmlXPathObjectPtr node, const unsigned char *value) {

    if ( xml_xpath_has_result(node) ) {
        
        const int maxNodes = node->nodesetval->nodeNr;

        xmlNodePtr *nodes = node->nodesetval->nodeTab;
        
        for (int curattr = 0;curattr < maxNodes ; ++curattr) {
            
            xmlNodePtr node = nodes[curattr];

            xmlNodePtr parent = node->parent;

            if (node->type == XML_CDATA_SECTION_NODE ) { 
                
                xmlUnlinkNode(node);
                xmlFreeNode(node);

                xmlNodePtr content_node = xmlNewCDataBlock(parent->doc, BAD_CAST value, strlen((const char *)value));
                xmlAddChild(parent, content_node);
            }
        
        }
    }

}

#if 0
//
// EOF private section
//
#endif


xml_ctx_t* xml_ctx_new_empty() {

    xmlDocPtr doc = xmlNewDoc((xmlChar *)"1.0");

    xml_ctx_t *new_ctx = __xml_ctx_create(NULL, doc);
    __xml_ctx_set_state(new_ctx, XML_CTX_SUCCESS, XML_CTX_READ_AND_PARSE);

    return new_ctx;
}

xml_ctx_t* xml_ctx_new_empty_root_name(const char* rootname) {
    xml_ctx_t* new_ctx = xml_ctx_new_empty();

    if(rootname && strlen(rootname) > 0) {
        xmlNodePtr newroot = xmlNewNode(NULL, (xmlChar *) rootname);
        xmlDocSetRootElement(new_ctx->doc, newroot);
    }

    return new_ctx;
}


xml_ctx_t* xml_ctx_new(const xml_source_t *xml_src) {

    xmlDocPtr doc = NULL;

    xml_ctx_state_no_t state_no = XML_CTX_SUCCESS; 
    xml_ctx_state_reason_t reason = XML_CTX_READ_AND_PARSE;

    if ( xml_src != NULL && xml_src->src_data != NULL && *xml_src->src_size > 0 ) {

        doc = xmlReadMemory((const char *)xml_src->src_data, *xml_src->src_size, "noname.xml", NULL, 0);
        
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
        xmlXPathRegisterFunc(xpathCtx,(const xmlChar *) "in_range", str_in_range_xpath_func); 

        if ( xpathCtx != NULL ) {
            
            result = xmlXPathEvalExpression((const xmlChar*)xpath, xpathCtx);
                            
        }
        
        xmlXPathFreeContext(xpathCtx);
    }

    return result;
}


xmlXPathObjectPtr xml_ctx_xpath_format( const xml_ctx_t *ctx, const char *xpath_format, ...) {
    
    va_list args;
    va_start(args, xpath_format);
    char *gen_xpath = format_string_va_new(xpath_format, args);
    va_end(args);

    #if debug > 0
        printf("gen xpath: %s\n", gen_xpath);
    #endif

    xmlXPathObjectPtr result = xml_ctx_xpath(ctx, gen_xpath);

    free(gen_xpath);
    
    return result;
}

xmlXPathObjectPtr xml_ctx_xpath_format_va( const xml_ctx_t *ctx, const char *xpath_format, va_list argptr) {
    
    char *gen_xpath = format_string_va_new(xpath_format, argptr);
    
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

    if ( xml_xpath_has_result(srcxpres) ) {

        const int numsrcs = srcxpres->nodesetval->nodeNr;
        xmlNodePtr * sources = srcxpres->nodesetval->nodeTab;

        xmlXPathObjectPtr dstxpres = xml_ctx_xpath(dst, dst_xpath);

        if ( xml_xpath_has_result(dstxpres) ) {

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

void xml_ctx_nodes_add_note_xpres(xmlNodePtr src_node, xmlXPathObjectPtr dst_result) {
    
    if ( xml_xpath_has_result(dst_result) ) {

        const int maxNodes = dst_result->nodesetval->nodeNr;
        xmlNodePtr *nodes = dst_result->nodesetval->nodeTab;

        for(int curNode = 0; curNode < maxNodes; ++curNode) {

            xmlNodePtr target_node = nodes[curNode];
            
            xmlNodePtr copy = xmlCopyNode(src_node, 1);
            
            xmlAddChild(target_node, copy);
        }

    }

}

void xml_ctx_nodes_add_node_xpath(xmlNodePtr src_node, xml_ctx_t *dst, const char *dst_xpath) {

    xmlXPathObjectPtr target_node_result = xml_ctx_xpath(dst, dst_xpath);

    xml_ctx_nodes_add_note_xpres(src_node, target_node_result);

    xmlXPathFreeObject(target_node_result);
}

void xml_ctx_nodes_add_node_xpath_format(xmlNodePtr src_node, xml_ctx_t *dst, const char *dst_xpath, ...) {

    va_list args;
    va_start(args, dst_xpath);
    xmlXPathObjectPtr target_node_result = xml_ctx_xpath_format_va(dst, dst_xpath, args);
    va_end(args);

    xml_ctx_nodes_add_note_xpres(src_node, target_node_result);

    xmlXPathFreeObject(target_node_result);
}

void xml_ctx_rem_nodes_xpres(xmlXPathObjectPtr xpres) {
    
    if (xml_xpath_has_result(xpres)) {
        const int cntNodes = xpres->nodesetval->nodeNr;
        for(int curnode = 0; curnode < cntNodes; ++curnode) {
            xmlNodePtr cn = xpres->nodesetval->nodeTab[curnode];
            xmlUnlinkNode(cn);
            xmlFreeNode(cn);
        }
    }

}


void xml_ctx_remove(xml_ctx_t *ctx, const char *xpath) {

    xmlXPathObjectPtr found = xml_ctx_xpath_format(ctx, xpath);

    xml_ctx_rem_nodes_xpres(found);

    xmlXPathFreeObject(found);
}

void xml_ctx_remove_format(xml_ctx_t *ctx, const char *xpath_format, ...) {

    va_list args;
    va_start(args, xpath_format);
    xmlXPathObjectPtr found = xml_ctx_xpath_format_va(ctx, xpath_format, args);
    va_end(args);

    xml_ctx_rem_nodes_xpres(found);

    xmlXPathFreeObject(found);
}

bool xml_ctx_exist(xml_ctx_t *ctx, const char *xpath) {
    xmlXPathObjectPtr found = xml_ctx_xpath(ctx, xpath);

    bool exist = xml_xpath_has_result(found);

    xmlXPathFreeObject(found);

    return exist;
}

bool xml_ctx_exist_format(xml_ctx_t *ctx, const char *xpath_format, ...) {

    va_list args;
    va_start(args, xpath_format);
    xmlXPathObjectPtr found = xml_ctx_xpath_format_va(ctx, xpath_format, args);
    va_end(args);

    bool exist = xml_xpath_has_result(found);

    xmlXPathFreeObject(found);

    return exist;
    
}

bool xml_xpath_has_result(xmlXPathObjectPtr xpathobj) {
    return ( xpathobj != NULL && xpathobj->nodesetval && (xpathobj->nodesetval->nodeNr > 0 ));
}

void xml_ctx_set_attr_str_xpath(xml_ctx_t *ctx, const unsigned char *value, const char *xpath) {
    
    xmlXPathObjectPtr found = xml_ctx_xpath(ctx, xpath);

    __xml_ctx_attr_str_xpptr(found, value);

    xmlXPathFreeObject(found);

}

void xml_ctx_set_attr_str_xpath_format(xml_ctx_t *ctx, const unsigned char *value, const char *xpath_format, ...) {
    
    va_list args;
    va_start(args, xpath_format);

    xmlXPathObjectPtr found = xml_ctx_xpath_format_va(ctx, xpath_format, args);

    __xml_ctx_attr_str_xpptr(found, value);

    va_end(args);

    xmlXPathFreeObject(found);
}

void xml_ctx_set_content_xpath(xml_ctx_t *ctx, const unsigned char *value, const char *xpath) {
    xmlXPathObjectPtr found = xml_ctx_xpath(ctx, xpath);

    __xml_ctx_content_xpptr(found, value);

    xmlXPathFreeObject(found);
}

void xml_ctx_set_content_xpath_format(xml_ctx_t *ctx, const unsigned char *value, const char *xpath_format, ...) {
    va_list args;
    va_start(args, xpath_format);

    xmlXPathObjectPtr found = xml_ctx_xpath_format_va(ctx, xpath_format, args);

    __xml_ctx_content_xpptr(found, value);

    va_end(args);

    xmlXPathFreeObject(found);
}

xmlChar * xml_ctx_get_attr(xml_ctx_t *ctx, const unsigned char *attr_name, const char *xpath) {
    
    xmlChar *value = NULL;
    
    if (ctx != NULL) {

        xmlXPathObjectPtr found = xml_ctx_xpath(ctx, xpath);

        if (xml_xpath_has_result(found)) {
            value = xmlGetProp(found->nodesetval->nodeTab[0], (const xmlChar*)attr_name);
        }

        xmlXPathFreeObject(found);

    }

    return value;

}

xmlChar * xml_ctx_get_attr_format(xml_ctx_t *ctx, const unsigned char *attr_name, const char *xpath_format, ...) {
    
    xmlChar *value = NULL;
    
    if (ctx != NULL) {

        va_list args;
        va_start(args, xpath_format);

        xmlXPathObjectPtr found = xml_ctx_xpath_format_va(ctx, xpath_format, args);

        va_end(args);

        if (xml_xpath_has_result(found)) {
            value = xmlGetProp(found->nodesetval->nodeTab[0], (const xmlChar*)attr_name);
        }

        xmlXPathFreeObject(found);
    
    }

    return value;
}
