#include "string_utils.h"

char * copy_string(const char * string) {
	size_t size = strlen(string) + 1;
	char * copy = malloc(size*sizeof(char));
	memcpy(copy, string, size);
	return copy;
}

char * format_string_new(const char * msg, ...) {
	va_list vl;
	va_start(vl, msg);
	int buffsize = vsnprintf(NULL, 0, msg, vl);
	va_end(vl);
	buffsize += 1;
	char * buffer = malloc(buffsize);
	va_start(vl, msg);
	vsnprintf(buffer, buffsize, msg, vl);
	va_end( vl);
	return buffer;
}

char * format_string_va_new(const char * msg, va_list argptr)  {
	int buffsize = vsnprintf(NULL, 0, msg, argptr);
	buffsize += 1;
	char * buffer = malloc(buffsize);
	vsnprintf(buffer, buffsize, msg, argptr);
	return buffer;
}

bool name_match(const unsigned char *search, const unsigned char *base) {
	return strcmp((char *)search, (char *)base) == 0;
}
