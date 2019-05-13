#include "resource.h"

#if 0
/*
	################### PRIVATE SECTION ###################
*/
#endif 

static void* __resource_reade_entry_data_byte(la_int64_t entry_size, struct archive *a, int *status) {
	int r;
	const void *buff;
	size_t size;
	la_int64_t offset;

	void * buffer = malloc(entry_size);
	
	for (;;) {
		*status = archive_read_data_block(a, &buff, &size, &offset);

		memcpy(buffer + offset, buff, size);
		if (*status == ARCHIVE_EOF) {
			break;
		}
		if (*status < ARCHIVE_OK) {
			break;
		}
	}	
	
	return buffer;
}

static unsigned char* __resource_read_entry_data_string(struct archive *a, struct archive_entry *entry, int *status, size_t *readed_bytes) {
	la_int64_t entry_size = archive_entry_size(entry);
	unsigned char *data = __resource_reade_entry_data_byte(entry_size + 1, a, status);
	data[entry_size] = '\0';
	*readed_bytes = entry_size;
	return data;
}

static resource_file_t* __resource_read_entry(struct archive *a, struct archive_entry *entry) {

	la_int64_t entry_size = archive_entry_size(entry);
	resource_file_t *result = NULL;
	if ( entry_size > 0) {
	
		const char *entry_pathname = archive_entry_pathname(entry);
		
		int data_read_status;
		size_t readed_bytes;
		unsigned char *data = __resource_read_entry_data_string(a, entry, &data_read_status, &readed_bytes);
		
		char * copy_path = copy_string(entry_pathname);

		result = resource_file_new(copy_path, data, readed_bytes);
	
	}
	
	return result;
}

typedef struct _sr_item {
	resource_file_t *file;
	struct _sr_item *next;
} sr_item_t;

static sr_item_t* __new_sr_item() {
	sr_item_t *new_item = malloc(sizeof(sr_item_t));
	new_item->file = NULL;
	new_item->next = NULL;
}

static void __delete_sr_item(sr_item_t **item) {
	if ( item ) {
		sr_item_t *cur_item = *item;
		while(cur_item != NULL) {
			sr_item_t *next_item = cur_item->next;
			free(cur_item);
			cur_item = next_item;
		}
		*item = NULL;
	}
}

static resource_search_result_t * __resource_load_resource(archive_resource_t * archive_resource, const char *pattern) {
	struct archive *a = archive_read_new();
	
	sr_item_t *first_file = __new_sr_item();
	sr_item_t *cur_temp_file = first_file;
	unsigned int cnt_files = 0;
	
	archive_read_support_compression_all(a);
	archive_read_support_format_7zip(a);
	
	int r = archive_read_open_memory(a, archive_resource->archive_data, (intptr_t)&archive_resource->size);
	if (r == ARCHIVE_OK) {

		#if debug > 0
			printf("archive ok :)\n");
		#endif 

		struct archive_entry *entry;
		while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		  const char *pathname = archive_entry_pathname(entry);
		  if ( regex_match(pattern, pathname) ) {

			resource_file_t *new_file = __resource_read_entry(a, entry);
			if ( new_file != NULL ) {
				cur_temp_file->file = new_file;
				cur_temp_file->next = __new_sr_item();
				cur_temp_file = cur_temp_file->next;
				++cnt_files;
			}

		  }
		}
	} 

	#if debug > 0 
	if (r != ARCHIVE_OK) {
			printf("archive not ok!!!! :(\n");
	}
	#endif
	
	resource_search_result_t *result = malloc(sizeof(resource_search_result_t));
	result->cnt = cnt_files;
	result->files = malloc(cnt_files * sizeof(resource_file_t *));
	
	cur_temp_file = first_file;
	unsigned int cur_file = 0;
	while(cur_temp_file->file != NULL) {
		result->files[cur_file++] = cur_temp_file->file;
		cur_temp_file = cur_temp_file->next;
	}
	
	__delete_sr_item(&first_file);
	
	r = archive_read_close(a);
	r = archive_read_free(a);

	#if debug > 0 
	if (r != ARCHIVE_OK) {
		printf("archive free failed :(\n");
	} else {
		printf("archive free success :)\n");
	}
	#endif

	return result;
}

static void __resource_search_result_free_raw(resource_search_result_t **result, bool with_file_entries) {
	if ( result != NULL && *result != NULL ) {
		resource_search_result_t *to_delete_result = *result;
		
		if ( with_file_entries ) {
			resource_file_t ** files = to_delete_result->files;
			for(unsigned int cnt_files = to_delete_result->cnt; cnt_files--;) {
				resource_file_free(&files[cnt_files]); // TODO SEGFAULT!!! :(
			}
		}
		
		free(to_delete_result->files);
		free(to_delete_result);
		*result = NULL;
	}
}

#if 0
/*
	################### EOF PRIVATE SECTION ###################
*/
#endif 

archive_resource_t * archive_resource_memory(const unsigned char *data, size_t size_in_bytes) {
	archive_resource_t * new_ar = malloc(sizeof(archive_resource_t));
	
	new_ar->archive_data 		= data;
	new_ar->size 				= size_in_bytes;
	new_ar->delete_data_on_free = false;
	new_ar->free_func 			= NULL;
	
	return new_ar;
}

void archive_resource_set_config_free(archive_resource_t *archive_resource, bool delete_data_on_free, void (*free_func)(void * ptr)) {
	
	if ( archive_resource != NULL ) {
	
		archive_resource->delete_data_on_free 	= delete_data_on_free;
		archive_resource->free_func 			= free_func;
	
	}	
}

void archive_resource_set_config_free_default(archive_resource_t *archive_resource, bool delete_data_on_free) {

	if ( archive_resource != NULL ) {
	
		archive_resource->delete_data_on_free 	= delete_data_on_free;
		archive_resource->free_func 			= free;
	
	}

}

resource_search_result_t * archive_resource_search(archive_resource_t *archive_resource, const char *pattern) {
	return __resource_load_resource(archive_resource, pattern);
}

resource_file_t * resource_file_new_empty() {
	resource_file_t *new_file = malloc(sizeof(resource_file_t));
	new_file->complete = NULL;
	new_file->path = NULL;
	new_file->file = NULL;
	new_file->name = NULL;
	new_file->type = NULL;
	new_file->data = NULL;
	new_file->file_size = 0;
	return new_file;
}

resource_file_t * resource_file_new(const char *full_file_path, unsigned char 	*data, size_t	data_size) {

	resource_file_t *new_file = resource_file_new_empty();
	new_file->complete 	= copy_string(full_file_path);
	new_file->data 		= data;
	new_file->file_size = data_size;
	
	new_file->path = path_from_full_filepath(full_file_path);
	new_file->file = file_from_full_filepath(full_file_path);
	new_file->name = name_from_filename(new_file->file);
	new_file->type = type_from_filename(new_file->file);
	
	return new_file;
}

resource_file_t * resource_file_copy_deep(resource_file_t *file) {

	resource_file_t *copy_file = resource_file_new_empty();
	copy_file->complete 	= copy_string(file->complete);
	copy_file->path = copy_string(file->path);
	copy_file->file = copy_string(file->file);
	copy_file->name = copy_string(file->name);
	copy_file->type = copy_string(file->type);
	
	copy_file->file_size = file->file_size;
	copy_file->data = malloc(file->file_size*sizeof(unsigned char));
	memcpy(copy_file->data, file->data, file->file_size);
	
	return copy_file;
}

void resource_file_free(resource_file_t **file) {

	if ( file != NULL && *file != NULL ) {
	
		resource_file_t *to_deletefile = *file;
	
		free(to_deletefile->complete);
		free(to_deletefile->path);
		free(to_deletefile->file);
		free(to_deletefile->name);
		free(to_deletefile->type);
		free(to_deletefile->data);
		free(to_deletefile);
	
		to_deletefile = NULL;
	
	}
}

void resource_search_result_free(resource_search_result_t **result) {
	__resource_search_result_free_raw(result, false);
}

void resource_search_result_full_free(resource_search_result_t **result) {
	__resource_search_result_free_raw(result, true);
}

void archive_resource_free(archive_resource_t **archive_resource) {
	if ( archive_resource != NULL && *archive_resource != NULL ) {
		
		archive_resource_t * to_delete = *archive_resource;
		
		if ( to_delete->delete_data_on_free ) {
			
			if ( to_delete->free_func == NULL ) to_delete->free_func = free;
			
			to_delete->free_func((void*)to_delete->archive_data);
		}
		
		free(to_delete);
		
		*archive_resource = NULL;
	}
}
