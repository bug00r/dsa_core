#include "hero.h"

static xml_ctx_t* __dsa_heros_ctx_from_src( archive_resource_t *archive, const char *resname) {
    
    xml_ctx_t* result = NULL;

    if ( archive != NULL && resname != NULL && ( strlen(resname) > 0 )) {
        const xml_source_t* src = xml_source_from_resname(archive, resname);

        if (src != NULL) {
            result = xml_ctx_new(src);
        }
	    
    }

    return result;
} 

void dsa_heros_free(dsa_heros_t **heros) {

    if(heros != NULL && *heros != NULL) {
        dsa_heros_t *delete_heros = *heros;
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->basehero));
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->breeds));
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->cultures));
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->professions));
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->pro_contra));
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->specialabilities));
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->spells));
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->talents));
        free_xml_ctx_src(&delete_heros->heros);

        free(delete_heros);
        *heros = NULL;
    }
}

dsa_heros_t* dsa_heros_new_archiv(archive_resource_t *archive) {

    dsa_heros_t _temp_heros = {
        __dsa_heros_ctx_from_src(archive, "basehero"),
        __dsa_heros_ctx_from_src(archive, "breeds"),
        __dsa_heros_ctx_from_src(archive, "cultures"),
        __dsa_heros_ctx_from_src(archive, "professions"),
        __dsa_heros_ctx_from_src(archive, "procontra"),
        __dsa_heros_ctx_from_src(archive, "specialabilities"),
        __dsa_heros_ctx_from_src(archive, "spells"),
        __dsa_heros_ctx_from_src(archive, "talents"),
        xml_ctx_new_empty_root_name("heros")
    };

    dsa_heros_t *newheros = malloc(sizeof(dsa_heros_t));

    memcpy(newheros, &_temp_heros, sizeof(dsa_heros_t));

    return newheros;
}

bool dsa_heros_load(dsa_heros_t *heros, const char *filename) {
    xml_ctx_t *nCtx = xml_ctx_new_file(filename);
    free_xml_ctx(&heros->heros);
    heros->heros = nCtx;
    return ( heros->heros->state.state_no == XML_CTX_SUCCESS);
}

bool dsa_heros_save(dsa_heros_t *heros, const char *filename) {
    xml_ctx_save_file(heros->heros, filename);
    return ( heros->heros->state.state_no == XML_CTX_SUCCESS);
}

dsa_hero_t* dsa_hero_new(dsa_heros_t *heros, const char* hero_name) {

    dsa_hero_t *new_hero = NULL;

    if ( regex_match("[\\d\\D]+",hero_name) ) {

        xml_ctx_t *nCtx = xml_ctx_new_empty();
    	xmlNodePtr newheroroot = xmlCopyNode(xmlDocGetRootElement(heros->basehero->doc), 1);
        
        xmlSetProp(newheroroot, "name", hero_name);
        
        struct timeval tval_before;
        gettimeofday(&tval_before, NULL);

        char * id = format_string_new("%ld%ld", (long int)tval_before.tv_sec, (long int)tval_before.tv_usec);
        xmlSetProp(newheroroot, "id", id);

        #if debug > 0
            printf("create new hero: name \"%s\" with id \"%s\"\n", hero_name, id);
        #endif

        free(id);

        xmlDocSetRootElement(nCtx->doc, newheroroot);
        
        new_hero = malloc(sizeof(dsa_hero_t));
        new_hero->xml = nCtx;

        xmlNodePtr herosroot = xmlDocGetRootElement(heros->heros->doc);

        xmlAddChild(herosroot, xmlCopyNode(newheroroot, 1));
    }

    return new_hero;
}

dsa_hero_t* dsa_hero_free(dsa_hero_t **hero) {
    if (hero != NULL && *hero != NULL) {
        dsa_hero_t *todelete_hero = *hero;
        free_xml_ctx_src(&todelete_hero->xml);
        free(todelete_hero);
        *hero = NULL;
    }
}