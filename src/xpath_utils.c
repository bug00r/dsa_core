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

void max_xpath_func(xmlXPathParserContextPtr ctxt, int nargs) {

	xmlNodeSetPtr nodes = xmlXPathPopNodeSet(ctxt);
	
	if (xmlXPathCheckError(ctxt)) {
		xmlXPathFreeNodeSet(nodes);
        return;
    }
	
	long long max_val = 0;

	for (int curnode = 0; curnode < nodes->nodeNr; ++curnode) { 
		xmlNodePtr curNode = nodes->nodeTab[curnode];
		if (curNode->type == XML_ATTRIBUTE_NODE) {

			xmlChar *sattr = xmlGetProp(curNode->parent, curNode->name);
			
			long long value = atoll(sattr);

			max_val = (value > max_val ? value : max_val);

			if (sattr != NULL) {
				xmlFree(sattr);
			}
		}
		
	}

	xmlXPathFreeNodeSet(nodes);

	xmlXPathReturnNumber(ctxt, max_val);
}