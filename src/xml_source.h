#ifndef XML_SOURCE_H
#define XML_SOURCE_H

#if 0
    Concept global, just for remembering target: 
        - Utils have to work with xml sources. So we need xml_source_t with different loading interfaces. At first from memory or memory Wrapper Pattern
          in this case from resource too.        
#endif

#include "resource.h"

typedef enum {
    RESOURCE_FILE
} xml_source_type_t;

typedef struct {
    const xml_source_type_t     const type;
    const size_t			  * const src_size; /* size of xml source in byte */
	const unsigned char 	  * const src_data; /* data of xml source as byte array */
    union {
        const resource_file_t * const resfile;
    } data;
} xml_source_t;

/*
	This function reads an xml source from fix archiv folder xml.
	
    Example:
        This will search at xml in archiv "xml/talents.xml"
        xml_source_t *result = xml_source_from_resname(archiv, "talents");

        This will search at xml in archiv "xml/breeds.xml"
        xml_source_t *result = xml_source_from_resname(archiv, "breeds");

        Invalid use:
        This will search at xml in archiv "xml/xml2/breeds.xml"
        xml_source_t *result = xml_source_from_resname(archiv, "xml2/breeds");

	Parameter			Decription
	---------			-----------------------------------------
	ar		            resource archive for search within.
	name				name of the resource, without path or suffix.
	
	returns: new xml source object or NULL if no resource was found by name
	
*/
xml_source_t* xml_source_from_resname(archive_resource_t* ar, const char *name);

/*
	The function "xml_source_free" frees the memory of xml source complete.
	
	Parameter			Decription
	---------			-----------------------------------------
	source				pointer to pointer of source, this pointer will be NULL
	
*/
void xml_source_free(xml_source_t **source);

#endif