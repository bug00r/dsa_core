#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

char * copy_string(const char * string);
char * format_string_new(const char * msg, ...);
char * format_string_va_new(const char * msg, va_list argptr);
bool name_match(const unsigned char *search, const unsigned char *base);


#endif