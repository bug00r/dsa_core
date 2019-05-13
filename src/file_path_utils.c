#include "file_path_utils.h"

char * path_from_full_filepath(const char * full_file_path) {
	const char *pLastSlash = strrchr(full_file_path, '/');
	char *path = NULL;
	if ( pLastSlash ) {
		size_t count = pLastSlash - full_file_path;
		count++;
		path = malloc((count+1)*sizeof(char));
		strncpy(path, full_file_path, count);
		path[count] = '\0';
	}
	
	return path;
}

char * file_from_full_filepath(const char * full_file_path) {
	const char *pLastSlash = strrchr(full_file_path, '/');
	if ( !pLastSlash ) { pLastSlash = full_file_path; }
	else { pLastSlash++; }
	
	char * file = NULL;
	if (strlen(pLastSlash) > 0) {
		file = copy_string(pLastSlash);
	}
	return file;
}

char * type_from_filename(const char * file_name) {

	const char *pLastPoint = strrchr(file_name, '.');
	if ( !pLastPoint ) { pLastPoint = file_name; }
	else { pLastPoint++; }
	char *type = NULL;
	if ( strlen(pLastPoint) > 0) {
		type = copy_string(pLastPoint);
	}
	
	return type;
}

char * name_from_filename(const char * file_name) {

	const char *pLastSlash = strrchr(file_name, '/');
	if ( !pLastSlash ) { pLastSlash = file_name; }
	else { pLastSlash++; }
	
	const char *pLastPoint = strrchr(pLastSlash, '.');
	size_t count = 0;
	if ( !pLastPoint ) { count = strlen(pLastSlash); }
	else { count = pLastPoint - pLastSlash; }
	char *name = malloc((count+1)*sizeof(char));
	strncpy(name, pLastSlash, count);
	name[count] = '\0';
	return name;
}