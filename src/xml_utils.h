#ifndef XML_UTILS_H
#define XML_UTILS_H

extern int blubb;

#if 0
    Concept global, just for remembering target: 
        - Utils have to work with xml sources. So we need xml_source_t with different loading interfaces. At first from memory or memory Wrapper Pattern
          in this case from resource too.
        - the source will be used inside an xml_context_t. The context will load the libxml DocPtr from source.
        - an xml_context_t can be used inside of services. We want to have xml_services or interfaces at first for:
            - search based on xpath(including regex)
            - search based on Attribute Names and values(including regex and regular(string matching))
            - editing node(add delete change) and Attributes

        - As next we need some different context services/Interfaces
            - A Context can be:
                - Breed, Culture, Profession....etc.
            - a context service may use other context services as well. 
        
#endif

#endif