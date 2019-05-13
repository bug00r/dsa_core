#ifndef REGEX_UTILS_H
#define REGEX_UTILS_H

#include <stdbool.h>
#if debug > 0
    #include <stdio.h>
#endif 

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

bool regex_match(const char *_pattern, const char *_text);

#endif