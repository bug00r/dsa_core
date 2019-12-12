#ifndef DSA_HERO_H
#define DSA_HERO_H

#include <time.h>
#include <stdlib.h>

#include "xml_utils.h"
#include "number_utils.h"
#include "dice.h"

typedef struct {
    const xml_ctx_t * const basehero;           /* resource of new hero template */
    const xml_ctx_t * const breeds;             /* resource of breeds */
    const xml_ctx_t * const cultures;           /* resource of culturess */
    const xml_ctx_t * const professions;        /* resource of professions */
    const xml_ctx_t * const pro_contra;         /* resource of pro and contra */
    const xml_ctx_t * const specialabilities;   /* resource of special abilities */
    const xml_ctx_t * const spells;             /* resource of spells */
    const xml_ctx_t * const talents;            /* resource of talents */
    const xml_ctx_t * const liturgies;          /* resource of talents */
    xml_ctx_t * heros;
} dsa_heros_t;

typedef struct {
    xml_ctx_t * xml;
} dsa_hero_t;

typedef struct {
    unsigned char *name;
    long long id;
} dsa_hero_entry_t;

void dsa_heros_free(dsa_heros_t **heros);

dsa_heros_t* dsa_heros_new_archiv(archive_resource_t *archive);

//load heros from file
bool dsa_heros_load(dsa_heros_t *heros, const char *filename);

//saves all heros to file
bool dsa_heros_save(dsa_heros_t *heros, const char *filename);

//creates new hero as a xml_ctx with copy from heros. 
//If need otherwise we load basehero and copy root node to heros and hero.
//if hero_name is invalid (check against regex :)
dsa_hero_t* dsa_hero_new(dsa_heros_t *heros, const unsigned char* hero_name);
void dsa_hero_free(dsa_hero_t **hero);

//returns a copy of found hero by id. Id is needed because names are redudant.
dsa_hero_t* dsa_hero_get(dsa_heros_t *heros, const int id);

//returns the pointer of first hero or NULL if no hero was found
//the last pointer element is set to NULL for easier iteration.
dsa_hero_entry_t* dsa_hero_get_all(dsa_heros_t *heros);
void dsa_hero_list_free(dsa_hero_entry_t **hero_list);

//saves as replacement the hero inside of heros(not to file) by searching the ID
void dsa_heros_save_hero(dsa_heros_t *heros, const dsa_hero_t *hero);

//deletes hero by id.
void dsa_heros_delete_hero(dsa_heros_t *heros, const int id);
void dsa_heros_delete_hero_obj(dsa_heros_t *heros, dsa_hero_t *hero);

void dsa_heros_add_breed(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *name);
void dsa_heros_add_culture(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *name);
void dsa_heros_add_profession(dsa_heros_t *heros,  dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_add_pro(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name);
void dsa_heros_remove_pro(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_add_contra(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name);
void dsa_heros_remove_contra(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_add_specialability(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name);
void dsa_heros_remove_specialability(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_add_talent(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name);
void dsa_heros_remove_talent(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_add_spell(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name);
void dsa_heros_remove_spell(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_add_liturgie(dsa_heros_t *heros, dsa_hero_t *hero, const unsigned char *group, const unsigned char *name);
void dsa_heros_remove_liturgie(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_talent_inc(dsa_hero_t *hero, const unsigned char *name);
void dsa_heros_talent_dec(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_spell_inc(dsa_hero_t *hero, const unsigned char *name);
void dsa_heros_spell_dec(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_liturgie_inc(dsa_hero_t *hero, const unsigned char *name);
void dsa_heros_liturgie_dec(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_specialability_inc(dsa_hero_t *hero, const unsigned char *name);
void dsa_heros_specialability_dec(dsa_hero_t *hero, const unsigned char *name);

void dsa_heros_attr_inc(dsa_hero_t *hero, const unsigned char *short_name);
void dsa_heros_attr_dec(dsa_hero_t *hero, const unsigned char *short_name);

xmlChar * dsa_heros_get_name(const dsa_hero_t *hero);
xmlChar * dsa_heros_get_gp(const dsa_hero_t *hero);
xmlChar * dsa_heros_get_id(const dsa_hero_t *hero);
xmlChar * dsa_heros_get_height(const dsa_hero_t *hero);
int dsa_heros_get_height_min(const dsa_hero_t *hero);
int dsa_heros_get_height_max(const dsa_hero_t *hero);
xmlChar * dsa_heros_get_base_height(const dsa_hero_t *hero);
xmlChar * dsa_heros_get_weight(const dsa_hero_t *hero);
xmlChar * dsa_heros_get_hair_col(const dsa_hero_t *hero);
xmlChar * dsa_heros_get_eye_col(const dsa_hero_t *hero);
xmlChar * dsa_heros_get_attr(const dsa_hero_t *hero, const unsigned char *short_name);

void dsa_heros_set_name(dsa_hero_t *hero, const unsigned char *name);
void dsa_heros_set_gp(dsa_hero_t *hero, const unsigned char *gp);
void dsa_heros_set_title(dsa_hero_t *hero, const unsigned char *title);
void dsa_heros_set_status(dsa_hero_t *hero, const unsigned char *status);
void dsa_heros_set_look(dsa_hero_t *hero, const unsigned char *look);
void dsa_heros_set_story(dsa_hero_t *hero, const unsigned char *story);
void dsa_heros_set_male(dsa_hero_t *hero);
void dsa_heros_set_female(dsa_hero_t *hero);
void dsa_heros_set_col_hair_by_name(dsa_hero_t *hero, const unsigned char *color_name);
void dsa_heros_set_col_hair_by_dice(dsa_hero_t *hero);

void dsa_heros_set_col_eye_by_name(dsa_hero_t *hero, const unsigned char *color_name);
void dsa_heros_set_col_eye_by_dice(dsa_hero_t *hero);

void dsa_heros_set_height_weight_by_value(dsa_hero_t *hero, const unsigned char *value);
void dsa_heros_set_height_weight_by_dice(dsa_hero_t *hero);

xmlXPathObjectPtr dsa_heros_get_breeds(dsa_heros_t *heros);
xmlXPathObjectPtr dsa_heros_get_cultures(dsa_heros_t *heros);
xmlXPathObjectPtr dsa_heros_get_professions(dsa_heros_t *heros);
xmlXPathObjectPtr dsa_heros_get_hair_colors(dsa_hero_t *hero);
xmlXPathObjectPtr dsa_heros_get_eye_colors(dsa_hero_t *hero);
#endif