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

bool name_match(const char *search, const char *pathname) {
	return strcmp(search, pathname) == 0;
}