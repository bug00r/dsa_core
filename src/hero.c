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

static char * __dsa_heros_get_next_id(dsa_heros_t *heros) {
    char *maxid = NULL;

    xmlXPathObjectPtr result = xml_ctx_xpath(heros->heros, "/heros/hero[@id = max(/heros/hero/@id)]");

    if (result && result->nodesetval && result->nodesetval->nodeNr == 1) {
       maxid = (char *)xmlGetProp(result->nodesetval->nodeTab[0] , (xmlChar*)"id");
       long long maxval = atoll(maxid);
       xmlFree(maxid);
       maxid = format_string_new("%llu", ++maxval);
    } 

    xmlXPathFreeObject(result);

    return maxid;
}

static void __dsa_delete_hero_by_id(xml_ctx_t *hctx, const int id) {
    if (hctx != NULL) {
        xmlXPathObjectPtr o_result = xml_ctx_xpath_format(hctx, "/heros/hero[@id='%i']", id); 

        xml_ctx_rem_nodes_xpres(o_result);

        xmlXPathFreeObject(o_result);
    }
}

static void __dsa_heros_inc_value_by(xmlNodePtr toeditnode, int add_value) {
    if (toeditnode != NULL) {
        xmlChar *value_str = xmlGetProp(toeditnode, (xmlChar*)"value");
        if(value_str != NULL) {
            int value = atoi((char *)value_str);
            xmlFree(value_str);

            char * new_val = format_string_new("%i", value + add_value);
            xmlSetProp(toeditnode, (xmlChar*)"value", (xmlChar*) new_val);
            free(new_val);
        }
    }
}

static void __dsa_heros_inc_dec_value_raw(xml_ctx_t *hero_xml, const char * hero_talent_xpath, const char * hero_edit_talent_xpath, int add_value) {

    xmlXPathObjectPtr foundhero = xml_ctx_xpath(hero_xml, hero_talent_xpath);

    if(xml_xpath_has_result(foundhero)) {

        xmlXPathObjectPtr foundedit = xml_ctx_xpath(hero_xml, hero_edit_talent_xpath);

        if(xml_xpath_has_result(foundedit)) {
            __dsa_heros_inc_value_by(foundedit->nodesetval->nodeTab[0], add_value);
        }

        xmlXPathFreeObject(foundedit);

    }

    xmlXPathFreeObject(foundhero);

}

static void __dsa_heros_set_col_raw(dsa_hero_t *hero, const unsigned char *color_type_name, const unsigned char *color_name) {
    xml_ctx_t * heroxml = hero->xml;
    
    xmlXPathObjectPtr hair_color = xml_ctx_xpath_format(heroxml, "/hero/breedcontainer/breed/colortypes/colors[@name = '%s']/color[@name = '%s']", color_type_name, color_name);

    if (xml_xpath_has_result(hair_color)) {

        xml_ctx_remove_format(heroxml, "/hero/edit/breed/colors[@name = '%s']/color", color_type_name);

        xml_ctx_nodes_add_node_xpath_format(hair_color->nodesetval->nodeTab[0], heroxml, "/hero/edit/breed/colors[@name = '%s']", color_type_name);

    }

    xmlXPathFreeObject(hair_color);
}   

static void __dsa_heros_set_col_dice_raw(dsa_hero_t *hero, const unsigned char *color_type_name) {
    xml_ctx_t * heroxml = hero->xml;

    xmlXPathObjectPtr dice_pattern = xml_ctx_xpath_format(heroxml, "/hero/breedcontainer/breed/colortypes/colors[@name = '%s']", color_type_name);

    if (xml_xpath_has_result(dice_pattern)) {

        xmlChar *dice_str = xmlGetProp(dice_pattern->nodesetval->nodeTab[0] , (xmlChar *)"dice");

        int dice_result = dsa_dice_result((const char *)dice_str);

        xmlFree(dice_str);

        xmlXPathObjectPtr hair_color = xml_ctx_xpath_format(heroxml, "/hero/breedcontainer/breed/colortypes/colors[@name = '%s']/color[in_range(@value,'%i')]", color_type_name, dice_result);

        if (xml_xpath_has_result(hair_color)) {

            xml_ctx_remove_format(heroxml, "/hero/edit/breed/colors[@name = '%s']/color", color_type_name);

            xml_ctx_nodes_add_node_xpath_format(hair_color->nodesetval->nodeTab[0], heroxml, "/hero/edit/breed/colors[@name = '%s']", color_type_name);

        }

        xmlXPathFreeObject(hair_color);

    }

    xmlXPathFreeObject(dice_pattern);

}

static void __dsa_heros_set_hw_raw(dsa_hero_t *hero, float heightadd, xmlXPathObjectPtr _height_res) {
    xml_ctx_t *heroxml = hero->xml;

    xmlXPathObjectPtr height_res = _height_res;
    if ( _height_res == NULL ) {
        height_res = xml_ctx_xpath(heroxml, "/hero/breedcontainer/breed/body-height");
    }

    xmlXPathObjectPtr weight_res = xml_ctx_xpath(heroxml, "/hero/breedcontainer/breed/body-weight");

    if (xml_xpath_has_result(height_res) && xml_xpath_has_result(weight_res) ) {
        xmlChar *wvalue = xmlGetProp(weight_res->nodesetval->nodeTab[0],(xmlChar*)"value");
        xmlChar *hvalue = xmlGetProp(height_res->nodesetval->nodeTab[0],(xmlChar*)"value");

        float height = atof((const char*)hvalue);
        float weight = atof((const char*)wvalue);

        height += heightadd;
        weight += height;

        unsigned char *strweight = (unsigned char *)format_string_new("%i",(int)weight); 
        unsigned char *strheight = (unsigned char *)format_string_new("%.2f",(height/100.f)); 

        xml_ctx_set_attr_str_xpath(hero->xml, (const unsigned char *)strheight, "/hero/edit/breed/height/@value");
        xml_ctx_set_attr_str_xpath(hero->xml, (const unsigned char *)strweight, "/hero/edit/breed/weight/@value");

        free(strweight);
        free(strheight);
        xmlFree(wvalue);
        xmlFree(hvalue);
    }
    if ( _height_res == NULL ) {
        xmlXPathFreeObject(height_res);
    }
    xmlXPathFreeObject(weight_res);
}

#if 0
// ################################################################################################
// EOF private Section
// ################################################################################################
#endif


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
        free_xml_ctx_src(((xml_ctx_t **)&delete_heros->liturgies));
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
        __dsa_heros_ctx_from_src(archive, "liturgies"),
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

dsa_hero_t* dsa_hero_new(dsa_heros_t *heros, const unsigned char* hero_name) {

    dsa_hero_t *new_hero = NULL;

    if ( regex_not_blank(hero_name) ) {

        xml_ctx_t *nCtx = xml_ctx_new_empty();
    	xmlNodePtr newheroroot = xmlCopyNode(xmlDocGetRootElement(heros->basehero->doc), 1);
        
        xmlSetProp(newheroroot, (xmlChar *)"name", hero_name);
        
        char * id = __dsa_heros_get_next_id(heros);

        xmlSetProp(newheroroot, (xmlChar *)"id", (id == NULL ? (xmlChar *)"0" : (xmlChar *)id ));

        #if debug > 0
            printf("create new hero: name \"%s\" with id \"%s\"\n", hero_name, (id == NULL ? "0" : id ));
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

void dsa_hero_free(dsa_hero_t **hero) {
    if (hero != NULL && *hero != NULL) {
        dsa_hero_t *todelete_hero = *hero;
        free_xml_ctx_src(&todelete_hero->xml);
        free(todelete_hero);
        *hero = NULL;
    }
}

//returns a copy of found hero by id. Id is needed because names are redudant.
dsa_hero_t* dsa_hero_get(dsa_heros_t *heros, const int id) {
    
    xmlXPathObjectPtr result = xml_ctx_xpath_format(heros->heros, "/heros/hero[@id='%i']", id);

    dsa_hero_t *found_hero = NULL;

    if (xml_xpath_has_result(result)) {
        xmlNodePtr found = result->nodesetval->nodeTab[0];

        found_hero = malloc(sizeof(dsa_hero_t));
        found_hero->xml = xml_ctx_new_node(found);
    }

    xmlXPathFreeObject(result);

    return found_hero;
}

//returns the pointer of first hero or NULL if no hero was found
//the last pointer element is set to NULL for easier iteration.
dsa_hero_entry_t* dsa_hero_get_all(dsa_heros_t *heros) {
    
    xmlXPathObjectPtr result = xml_ctx_xpath(heros->heros, "/heros/hero");

    dsa_hero_entry_t *herolist = NULL;

    if (xml_xpath_has_result(result)) {
        

        xmlNodeSetPtr nodes = result->nodesetval;
        herolist = malloc((result->nodesetval->nodeNr+1) * sizeof(dsa_hero_entry_t));

        #if debug > 0
            printf("found entries: %i\n", result->nodesetval->nodeNr);
        #endif

        for(int curnode = 0; curnode < nodes->nodeNr; ++curnode) {

            xmlNodePtr found = nodes->nodeTab[curnode];

            dsa_hero_entry_t *hero_entry = &herolist[curnode];
            hero_entry->name = xmlGetProp(found, (xmlChar *)"name");
            xmlChar *id = xmlGetProp(found, (xmlChar *)"id");
            hero_entry->id = atoll((char *)id);
            free(id);

        }

        herolist[nodes->nodeNr].name = NULL;
        herolist[nodes->nodeNr].id = -1;

    }

    xmlXPathFreeObject(result);

    return herolist;
}

void dsa_hero_list_free(dsa_hero_entry_t **hero_list) {

    dsa_hero_entry_t *todelete_hero_list = *hero_list;
    dsa_hero_entry_t *cur;

	int cnt = 0;
	cur = todelete_hero_list;
	while( cur->name != NULL && cur->id != -1 ) {
        xmlFree(cur->name);
		cnt++;
		cur++;
	}
    free(todelete_hero_list);
    *hero_list = NULL;
}

//saves as replacement the hero inside of heros(not to file) by searching the ID
void dsa_heros_save_hero(dsa_heros_t *heros, const dsa_hero_t *hero) {
    if (heros != NULL && hero != NULL) {

        xmlNodePtr root = xmlDocGetRootElement(hero->xml->doc);
        xmlChar * id = xmlGetProp(root, (xmlChar *)"id");

        xml_ctx_t *heros_ctx = heros->heros;
        
        __dsa_delete_hero_by_id(heros_ctx, atoi((char *)id));
        
        xmlAddChild(xmlDocGetRootElement(heros_ctx->doc), xmlCopyNode(root ,1));
        
        xmlFree(id);
    }
}

void dsa_heros_delete_hero(dsa_heros_t *heros, const int id) {
    
    if (heros != NULL) {

        xml_ctx_t *heros_ctx = heros->heros;

        __dsa_delete_hero_by_id(heros_ctx, id);
    }

}

void dsa_heros_delete_hero_obj(dsa_heros_t *heros, dsa_hero_t *hero) {

    if ( heros != NULL && hero != NULL ) {
        
        xmlChar * id_str = dsa_heros_get_id(hero);
        
        int id = atoi((char *)id_str);
        
        dsa_heros_delete_hero(heros, id);
        
        xmlFree(id_str);
    }

}

void dsa_heros_add_breed(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *name) {
    if (heros != NULL && hero != NULL) {
        char *src_xpath = format_string_new("/breeds//breed[@name = '%s']", name);
        xml_ctx_remove(hero->xml, "/hero/breedcontainer/breed");
        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->breeds, src_xpath, hero->xml, "/hero/breedcontainer");
        free(src_xpath);
    }
}

void dsa_heros_add_culture(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *name) {
    if (heros != NULL && hero != NULL) {
        char *src_xpath = format_string_new("/cultures//culture[@name = '%s']", name);
        xml_ctx_remove(hero->xml, "/hero/culturecontainer/culture");
        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->cultures, src_xpath,  hero->xml, "/hero/culturecontainer");
        free(src_xpath);
    }
}

void dsa_heros_add_profession(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *name) {
    if (heros != NULL && hero != NULL) {
        char *src_xpath = format_string_new("/professions//profession[@name = '%s']", name);
        xml_ctx_remove(hero->xml, "/hero/professioncontainer/profession");
        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->professions, src_xpath,  hero->xml, "/hero/professioncontainer");
        free(src_xpath);
    }
}

void dsa_heros_add_pro(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name) {
    
    if (!xml_ctx_exist_format(hero->xml, "/hero/procontainer/pro[@name = '%s']", name)) {

        char *src_xpath = format_string_new("/procontra/group[@name = '%s']/pro[@name = '%s']", group, name);
        char *dst_xpath = format_string_new("/hero/procontainer/group[@name = '%s']", group);
        
        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->pro_contra, src_xpath,  hero->xml, dst_xpath);
        
        free(src_xpath);
        free(dst_xpath);
    }

}

void dsa_heros_add_pro_calc_group(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *name) {

    xmlChar *foundGroup = xml_ctx_get_attr_format((xml_ctx_t*)heros->pro_contra, (const unsigned char*)"name", (const char*)"/procontra//pro[@name = '%s']/..", name);
    
    dsa_heros_add_pro(heros, hero, (const unsigned char*)foundGroup, name);
    
    xmlFree(foundGroup);
}

void dsa_heros_remove_pro(dsa_hero_t *hero, const unsigned char *name) {
    xml_ctx_remove_format(hero->xml, "/hero/procontainer//pro[@name = '%s']", name);
}

void dsa_heros_add_contra(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name) {
    if (!xml_ctx_exist_format(hero->xml, "/hero/contracontainer/contra[@name = '%s']", name)) {

        char *src_xpath = format_string_new("/procontra/group[@name = '%s']/contra[@name = '%s']", group, name);
        char *dst_xpath = format_string_new("/hero/contracontainer/group[@name = '%s']", group);
        
        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->pro_contra, src_xpath,  hero->xml, dst_xpath);
        
        free(src_xpath); 
        free(dst_xpath);
    }    
}

void dsa_heros_remove_contra(dsa_hero_t *hero, const unsigned char *name) {
    xml_ctx_remove_format(hero->xml, "/hero/contracontainer//contra[@name = '%s']", name);
}

void dsa_heros_add_specialability(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name) {
    
    if (!xml_ctx_exist_format(hero->xml, "/hero/specialcontainer/group[@name = '%s']/specialability[@name = '%s']", group, name)) {

        char *src_xpath = format_string_new("/specialabilities/group[@name = '%s']/specialability[@name = '%s']", group, name);
        char *dst_xpath = format_string_new("/hero/specialcontainer/group[@name = '%s']", group);
        
        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->specialabilities, src_xpath,  hero->xml, dst_xpath);
        
        free(src_xpath);
        free(dst_xpath);
    }  
}

void dsa_heros_remove_specialability(dsa_hero_t *hero, const unsigned char *name) {
    xml_ctx_remove_format(hero->xml, "/hero/specialcontainer//specialability[@name = '%s']", name);
}

void dsa_heros_add_talent(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name) {
    
    if (!xml_ctx_exist_format(hero->xml, "/hero/talents/group[@name = '%s']/talent[@name = '%s' and @type != 'base']", group, name)) {
        
        char *src_xpath = format_string_new("/talents/group[@name = '%s']/talent[@name = '%s' and @type != 'base']", group, name);
        char *dst_xpath = format_string_new("/hero/talents/group[@name = '%s']", group);

        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->talents, src_xpath,  hero->xml, dst_xpath);
        
        free(dst_xpath);
        
        dst_xpath = format_string_new("/hero/edit/talents/group[@name = '%s']", group);

        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->talents, src_xpath,  hero->xml, dst_xpath);

        free(dst_xpath);
        free(src_xpath);

    }

}

void dsa_heros_remove_talent(dsa_hero_t *hero, const unsigned char *name) {
    xml_ctx_remove_format(hero->xml, "/hero/talents//talent[@name = '%s' and @type != 'base' ]", name);
    xml_ctx_remove_format(hero->xml, "/hero/edit/talents//talent[@name = '%s' and @type != 'base' ]", name);
}

void dsa_heros_add_spell(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name) {

    if (!xml_ctx_exist_format(hero->xml, "/hero/spells/group[@name = '%s']/spell[@name = '%s']", group, name)) {
        
        char *src_xpath = format_string_new("/spells/group[@name = '%s']/spell[@name = '%s']", group, name);
        char *dst_xpath = format_string_new("/hero/spells/group[@name = '%s']", group);

        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->spells, src_xpath,  hero->xml, dst_xpath);
        
        free(src_xpath);
        free(dst_xpath);

    }    

}

void dsa_heros_remove_spell(dsa_hero_t *hero, const unsigned char *name) {
    xml_ctx_remove_format(hero->xml, "/hero/spells//spell[@name = '%s']", name);
}

void dsa_heros_add_liturgie(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name) {
    
    if (!xml_ctx_exist_format(hero->xml, "/hero/liturgies/group[@name = '%s']/liturgy[@name = '%s']", group, name)) {
        
        char *src_xpath = format_string_new("/liturgies/group[@name = '%s']/liturgy[@name = '%s']", group, name);
        char *dst_xpath = format_string_new("/hero/liturgies/group[@name = '%s']", group);

        xml_ctx_nodes_add_xpath((xml_ctx_t*)heros->liturgies, src_xpath,  hero->xml, dst_xpath);
        
        free(src_xpath);
        free(dst_xpath);

    }      
}

void dsa_heros_remove_liturgie(dsa_hero_t *hero, const unsigned char *name) {
    xml_ctx_remove_format(hero->xml, "/hero/liturgies//liturgy[@name = '%s']", name);
}

void dsa_heros_talent_inc(dsa_hero_t *hero, const unsigned char *name) {
    char * hero_talent_xpath = format_string_new("/hero/talents//talent[@name = '%s']", name);
    char * hero_edit_talent_xpath = format_string_new("/hero/edit/talents//talent[@name = '%s']", name);

    __dsa_heros_inc_dec_value_raw(hero->xml, hero_talent_xpath, hero_edit_talent_xpath, 1);

    free(hero_talent_xpath);
    free(hero_edit_talent_xpath);

}

void dsa_heros_talent_dec(dsa_hero_t *hero, const unsigned char *name) {
    char * hero_talent_xpath = format_string_new("/hero/talents//talent[@name = '%s']", name);
    char * hero_edit_talent_xpath = format_string_new("/hero/edit/talents//talent[@name = '%s']", name);

    __dsa_heros_inc_dec_value_raw(hero->xml, hero_talent_xpath, hero_edit_talent_xpath, -1);

    free(hero_talent_xpath);
    free(hero_edit_talent_xpath);
}

void dsa_heros_spell_inc(dsa_hero_t *hero, const unsigned char *name) {
    
}

void dsa_heros_spell_dec(dsa_hero_t *hero, const unsigned char *name) {
    
}

void dsa_heros_liturgie_inc(dsa_hero_t *hero, const unsigned char *name) {
    
}

void dsa_heros_liturgie_dec(dsa_hero_t *hero, const unsigned char *name) {
    
}

void dsa_heros_specialability_inc(dsa_hero_t *hero, const unsigned char *name) {
    
}

void dsa_heros_specialability_dec(dsa_hero_t *hero, const unsigned char *name) {

}

void dsa_heros_attr_inc(dsa_hero_t *hero, const unsigned char *short_name) {
    char * hero_attr_xpath = format_string_new("/hero/attributes/attribute[@shortname = '%s']", short_name);
    char * hero_edit_attr_xpath = format_string_new("/hero/edit/attributes/attribute[@shortname = '%s']", short_name);

    __dsa_heros_inc_dec_value_raw(hero->xml, hero_attr_xpath, hero_edit_attr_xpath, 1);

    free(hero_attr_xpath);
    free(hero_edit_attr_xpath);
}

void dsa_heros_attr_dec(dsa_hero_t *hero, const unsigned char *short_name) {
    char * hero_attr_xpath = format_string_new("/hero/attributes/attribute[@shortname = '%s']", short_name);
    char * hero_edit_attr_xpath = format_string_new("/hero/edit/attributes/attribute[@shortname = '%s']", short_name);

    __dsa_heros_inc_dec_value_raw(hero->xml, hero_attr_xpath, hero_edit_attr_xpath, -1);

    free(hero_attr_xpath);
    free(hero_edit_attr_xpath);
}

xmlChar * dsa_heros_get_name(const dsa_hero_t *hero) {
    xmlChar * name = NULL;
    if ( hero != NULL && hero->xml != NULL ) {
        name = xmlGetProp(xmlDocGetRootElement(hero->xml->doc), (const xmlChar*)"name");
    }
    return name;
}

xmlChar * dsa_heros_get_gp(const dsa_hero_t *hero) {

    return (hero != NULL ? xml_ctx_get_attr(hero->xml, (const unsigned char*)"value", (const char*)"/hero/config/base-gp") : NULL); 

}

xmlChar * dsa_heros_get_id(const dsa_hero_t *hero) {
    xmlChar * id = NULL;
    if ( hero != NULL && hero->xml != NULL ) {
        id = xmlGetProp(xmlDocGetRootElement(hero->xml->doc), (const xmlChar*)"id");
    }
    return id;
}

xmlChar * dsa_heros_get_height(const dsa_hero_t *hero) {

    return (hero != NULL ? xml_ctx_get_attr(hero->xml, (const unsigned char*)"value", (const char*)"/hero/edit/breed/height") : NULL); 

}

int dsa_heros_get_height_min(const dsa_hero_t *hero) {
    xmlChar *dice = xml_ctx_get_attr(hero->xml, (const unsigned char*)"dice", (const char*)"/hero/breedcontainer/breed/body-height");
    int value = dsa_dice_min_result((const char*)dice);
    xmlFree(dice);
    return value;
}

int dsa_heros_get_height_max(const dsa_hero_t *hero) {
    xmlChar *dice = xml_ctx_get_attr(hero->xml, (const unsigned char*)"dice", (const char*)"/hero/breedcontainer/breed/body-height");
    int value = dsa_dice_max_result((const char*)dice);
    xmlFree(dice);
    return value;
}

xmlChar * dsa_heros_get_base_height(const dsa_hero_t *hero) {

    return (hero != NULL ? xml_ctx_get_attr(hero->xml, (const unsigned char*)"value", (const char*)"/hero/breedcontainer/breed/body-height") : NULL);
}

xmlChar * dsa_heros_get_weight(const dsa_hero_t *hero) {
    
    return (hero != NULL ? xml_ctx_get_attr(hero->xml, (const unsigned char*)"value", (const char*)"/hero/edit/breed/weight") : NULL); 

}

xmlChar * dsa_heros_get_hair_col(const dsa_hero_t *hero) {

    return (hero != NULL ? xml_ctx_get_attr(hero->xml, (const unsigned char*)"name", (const char*)"/hero/edit/breed/colors[@name = 'Haarfarbe']/color") : NULL);
 
}

xmlChar * dsa_heros_get_eye_col(const dsa_hero_t *hero) {

    return (hero != NULL ? xml_ctx_get_attr(hero->xml, (const unsigned char*)"name", (const char*)"/hero/edit/breed/colors[@name = 'Augenfarbe']/color") : NULL);

}

xmlChar * dsa_heros_get_attr(const dsa_hero_t *hero, const unsigned char *short_name) {

    return (hero != NULL ? xml_ctx_get_attr_format(hero->xml, (const unsigned char*)"value", (const char*)"/hero/edit/attributes/attribute[@shortname = '%s']", short_name) : NULL);

}

void dsa_heros_set_name(dsa_hero_t *hero, const unsigned char *name) {
    xml_ctx_set_attr_str_xpath(hero->xml, name, "/hero/@name");
}

void dsa_heros_set_gp(dsa_hero_t *hero, const unsigned char *gp) {
    xml_ctx_set_attr_str_xpath(hero->xml, gp, "/hero/config/base-gp/@value");
}

void dsa_heros_set_title(dsa_hero_t *hero, const unsigned char *title) {
    xml_ctx_set_content_xpath(hero->xml, title, "//hero/title/text()");
}

void dsa_heros_set_status(dsa_hero_t *hero, const unsigned char *status) {
    xml_ctx_set_content_xpath(hero->xml, status, "//hero/status/text()");
}

void dsa_heros_set_look(dsa_hero_t *hero, const unsigned char *look) {
    xml_ctx_set_content_xpath(hero->xml, look, "//hero/look/text()");
}

void dsa_heros_set_story(dsa_hero_t *hero, const unsigned char *story) {
    xml_ctx_set_content_xpath(hero->xml, story, "//hero/story/text()");
}

void dsa_heros_set_male(dsa_hero_t *hero) {
    xml_ctx_set_attr_str_xpath(hero->xml, (unsigned char *)"male", "/hero/@gender");
}

void dsa_heros_set_female(dsa_hero_t *hero) {
    xml_ctx_set_attr_str_xpath(hero->xml, (unsigned char *)"female", "/hero/@gender");
}

void dsa_heros_set_col_hair_by_name(dsa_hero_t *hero, const unsigned char *color_name) {
    __dsa_heros_set_col_raw(hero, (const unsigned char *)"Haarfarbe", color_name);
}

void dsa_heros_set_col_hair_by_dice(dsa_hero_t *hero) {
    __dsa_heros_set_col_dice_raw(hero, (const unsigned char *)"Haarfarbe");
}

void dsa_heros_set_col_eye_by_name(dsa_hero_t *hero, const unsigned char *color_name) {
    __dsa_heros_set_col_raw(hero, (const unsigned char *)"Augenfarbe", color_name);
}

void dsa_heros_set_col_eye_by_dice(dsa_hero_t *hero) {
    __dsa_heros_set_col_dice_raw(hero, (const unsigned char *)"Augenfarbe");
}

void dsa_heros_set_height_weight_by_value(dsa_hero_t *hero, const unsigned char *value) {
    __dsa_heros_set_hw_raw(hero, atof((const char *)value), NULL);
}

int dsa_heros_set_height_weight_by_dice(dsa_hero_t *hero) {
    xml_ctx_t *heroxml = hero->xml;

    xmlXPathObjectPtr height_res = xml_ctx_xpath(heroxml, "/hero/breedcontainer/breed/body-height");

    int dice_result = 0;

    if (xml_xpath_has_result(height_res)) {
        
        xmlChar *hdpattern = xmlGetProp(height_res->nodesetval->nodeTab[0],(xmlChar*)"dice");
        
        dice_result = dsa_dice_result((const char *)hdpattern);
        
        xmlFree(hdpattern);
 
        __dsa_heros_set_hw_raw(hero, (float)dice_result, height_res);
    }

    xmlXPathFreeObject(height_res);

    return dice_result;
}

xmlXPathObjectPtr dsa_heros_get_breeds(dsa_heros_t *heros) {
    return xml_ctx_xpath(heros->breeds, "/breeds//breed");
}

xmlXPathObjectPtr dsa_heros_get_cultures(dsa_heros_t *heros) {
    return xml_ctx_xpath(heros->cultures, "/cultures//culture");
}

xmlXPathObjectPtr dsa_heros_get_professions(dsa_heros_t *heros) {
    return xml_ctx_xpath(heros->professions, "/professions//profession");
}

xmlXPathObjectPtr dsa_heros_get_hair_colors(dsa_hero_t *hero) {
    return xml_ctx_xpath(hero->xml, "/hero/breedcontainer/breed/colortypes/colors[@name = 'Haarfarbe']/color");
}

xmlXPathObjectPtr dsa_heros_get_eye_colors(dsa_hero_t *hero) {
    return xml_ctx_xpath(hero->xml, "/hero/breedcontainer/breed/colortypes/colors[@name = 'Augenfarbe']/color");
}

