#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

char * copy_string(const char * string);
char * format_string_new(const char * msg, ...);
bool name_match(const char *search, const char *base);

#endif