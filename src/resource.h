#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <archive.h>
#include <archive_entry.h>

#include "regex_utils.h"
#include "string_utils.h"
#include "file_path_utils.h"


typedef struct {
	char 			*complete;	/* my/path/to/file.type */
	char			*path;		/* my/path/to/ */
	char 			*file;		/* file.type */
	char 			*name;		/* file */
	char 			*type;		/* type */
	size_t			file_size;	/* size of file in bytes */
	unsigned char 	*data;		/* data as byte array */
} resource_file_t;

typedef struct {
	unsigned int cnt;			/* Count of found result */ 
	resource_file_t **files;	/* pointer to search result */
} resource_search_result_t;

typedef struct {
	bool					delete_data_on_free;	
	void 					(*free_func)(void *ptr);
	const unsigned char 	*archive_data;
	size_t 					size;
} archive_resource_t;

/*
	The function "archive_resource_memory" initialize an archive resource from
	external memory. It only assign the memory pointer and set delete data on
	free to false, this means you have to delete external memory chunk by yourself.
	
	Note: freeing memory before archive resource usage ends will cause dangling pointer
		  or at least undefined behaviour. You can prevent this with function
		  "archive_resource_set_config_free".
	
	Parameter		Decription
	---------		-----------------------------------------
	data			pointer to archive memory chunk
	size_in_bytes	size of bytes to read.
	
	returns: pointer of archive_resource_t with given data.
*/
archive_resource_t* archive_resource_memory(const unsigned char *data, size_t size_in_bytes);

/*
	The function "archive_resource_set_config_free" sets or unsets an automatic
	memory chunk deletion option. the automatic memory deletion will happen
	by calling archive_resource_free function.
	
	Parameter			Decription
	---------			-----------------------------------------
	archive_resource	archive resource
	delete_data_on_free	true means calling free_func or free default 
	free_func			pointer to customized free function, if delete_data_on_free is true
						and this pointer is NULL then the default "free" method will be called
						(see archive_resource_set_config_free_default)
	
*/
void archive_resource_set_config_free(archive_resource_t *archive_resource, bool delete_data_on_free, void (*free_func)(void * ptr));

/*
	The function "archive_resource_set_config_free_default" does the same like
	function "archive_resource_set_config_free" but sets delete with standard free func
*/
void archive_resource_set_config_free_default(archive_resource_t *archive_resource, bool delete_data_on_free);

/*
	The function "archive_resource_search" search against path name with pcre2
	regex.
	
	Parameter			Decription
	---------			-----------------------------------------
	pattern				Search Pattern based on pcre2

	returns: pointer to array with pointer to resource_file_t item as data. Last Element is NULL.
			 You have to free each pointer with "resource_file_free", the point todo this is your
			 choice(based on reusage in another context), at least you have to free the resource_file_t*
			 pointer.
	
*/
resource_search_result_t *archive_resource_search(archive_resource_t *archive_resource, const unsigned char *pattern);
resource_search_result_t *archive_resource_search_by_name(archive_resource_t *archive_resource, const unsigned char *name);

/*
	The function "resource_file_new_empty" creates a new empty file.
	
	returns: new empty file
	
*/
resource_file_t * resource_file_new_empty();

/*
	The function "resource_file_new" creates a new file. It will create all needed entries of
	complete, path, file, type... if needed, all other values will be set NULL.
	
	Parameter			Decription
	---------			-----------------------------------------
	full_file_path		full file pahh like (my/path/to/file.type or file.type or file)
	data				data of file in bytes, will only set as pointer no as explicit copy
	data_size			size of data in byte count
	
	returns: new filled file
	
*/
resource_file_t * resource_file_new(const char *full_file_path, unsigned char 	*data, size_t	data_size);

/*
	The function "resource_file_copy_deep" makes a deep copy of given
	file.
	
	Parameter			Decription
	---------			-----------------------------------------
	file				pointer to the file which should be copied
	
	returns: a new copy of given file.
	
*/
resource_file_t * resource_file_copy_deep(resource_file_t *file);

/*
	The function "resource_file_free" frees the complete memory a resource file, including the
	needed memory for data inside.
	
	Parameter			Decription
	---------			-----------------------------------------
	file				pointer to pointer of file, this pointer will be NULL
	
*/
void resource_file_free(resource_file_t **file);

/*
	The function "resource_search_result_free" frees the complete memory for the container of resource files.
	This does no include the file entries at itself. Ich you want delete the whole search result with files too
	you have to use the function "resource_file_free".
	
	Parameter			Decription
	---------			-----------------------------------------
	result				pointer to pointer of result, this pointer will be NULL
	
*/
void resource_search_result_free(resource_search_result_t **result);

/*
	The function "resource_search_result_full_free" frees the complete memory for the container of resource files
	including the files itself. So you have to notice that any other usage of an entry will be a dangling pointer 
	and have to copy in deep.
	
	Parameter			Decription
	---------			-----------------------------------------
	result				pointer to pointer of result, this pointer will be NULL
	
*/
void resource_search_result_full_free(resource_search_result_t **result);

/*
	The function "archive_resource_free"  will clean up the archive resource object.
	If auto deletion was set this will called before.
	
	Parameter			Decription
	---------			-----------------------------------------
	archive_resource	archive resource to delete, will be set to NULL.
	
*/
void archive_resource_free(archive_resource_t **archive_resource);

#endif