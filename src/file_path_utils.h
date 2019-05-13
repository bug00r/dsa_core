#ifndef FILE_PATH_UTILS_H
#define FILE_PATH_UTILS_H

#include <stdlib.h>
#include <string.h>
#include "string_utils.h"

char * path_from_full_filepath(const char * full_file_path);
char * file_from_full_filepath(const char * full_file_path);
char * type_from_filename(const char * file_name);
char * name_from_filename(const char * file_name);

#endif