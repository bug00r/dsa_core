#include "xpath_utils.h"

void regexmatch_xpath_func(xmlXPathParserContextPtr ctxt, int nargs) {
	if ( nargs != 2 ) return;
	
	xmlChar *regex = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (regex == NULL)) {
        return;
    }
	
	xmlChar *text = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (text == NULL)) {
		xmlFree(regex);
        return;
    }
		
	xmlXPathReturnBoolean(ctxt, regex_match(regex, text));
	
	xmlFree(regex);
	xmlFree(text);
}