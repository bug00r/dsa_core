#ifndef XPATH_UTILS_H
#define XPATH_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "regex_utils.h"

void regexmatch_xpath_func(xmlXPathParserContextPtr ctxt, int nargs);
void max_xpath_func(xmlXPathParserContextPtr ctxt, int nargs);
void str_in_range_xpath_func(xmlXPathParserContextPtr ctxt, int nargs);

#endif