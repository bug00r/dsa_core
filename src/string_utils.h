#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

char * copy_string(const char * string);
char * format_string_new(const char * msg, ...);

#endif