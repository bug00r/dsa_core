#include "xml_source.h"

static xml_source_t* xml_source_new(xml_source_type_t type, resource_file_t *res_file) {
    
    xml_source_t _tmp_newxml_source = { type, &res_file->file_size, res_file->data, res_file };

    xml_source_t* newxml_source = malloc(sizeof(xml_source_t));
    
    memcpy(newxml_source, &_tmp_newxml_source, sizeof(xml_source_t));
    
    return newxml_source;
}

xml_source_t* xml_source_from_resname(archive_resource_t* ar, const char *name) {
    
    xml_source_t *result = NULL;

    if (ar != NULL && name != NULL) {
        char *searchname = format_string_new("xml/%s.xml", name);
        resource_search_result_t* searchresult = archive_resource_search_by_name(ar, searchname);
        free(searchname);

        if ( searchresult->cnt == 1 ) {
            result = xml_source_new(RESOURCE_FILE, searchresult->files[0]);
        }

         resource_search_result_free(&searchresult);

    }

    return result;
}

void xml_source_free(xml_source_t **source) {

    if( source != NULL && *source != NULL ) {
    
        xml_source_t *_delete_source = *source;
    
        switch(_delete_source->type) {
            case RESOURCE_FILE: resource_file_free((resource_file_t**)&_delete_source->data.resfile);
                                break;
        }
    
        free(_delete_source);
    
        *source = NULL;
    }
}
