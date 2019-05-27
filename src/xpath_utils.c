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
	
	bool match = regex_match(regex, text);
	
	xmlFree(regex);
	xmlFree(text);

	xmlXPathReturnBoolean(ctxt, match);
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
			
			long long value = atoll((const char *)sattr);

			max_val = (value > max_val ? value : max_val);

			if (sattr != NULL) {
				xmlFree(sattr);
			}
		}
		
	}

	xmlXPathFreeNodeSet(nodes);

	xmlXPathReturnNumber(ctxt, max_val);
}