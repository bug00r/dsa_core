#ifndef DSA_HERO_H
#define DSA_HERO_H

#include <time.h>
#include <stdlib.h>

#include "xml_utils.h"
#include "number_utils.h"

typedef struct {
    const xml_ctx_t * const basehero;           /* resource of new hero template */
    const xml_ctx_t * const breeds;             /* resource of breeds */
    const xml_ctx_t * const cultures;           /* resource of culturess */
    const xml_ctx_t * const professions;        /* resource of professions */
    const xml_ctx_t * const pro_contra;         /* resource of pro and contra */
    const xml_ctx_t * const specialabilities;   /* resource of special abilities */
    const xml_ctx_t * const spells;             /* resource of spells */
    const xml_ctx_t * const talents;            /* resource of talents */
    xml_ctx_t * heros;
} dsa_heros_t;

typedef struct {
    xml_ctx_t * xml;
} dsa_hero_t;

typedef struct {
    char *name;
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
dsa_hero_t* dsa_hero_new(dsa_heros_t *heros, const char* hero_name);
dsa_hero_t* dsa_hero_free(dsa_hero_t **hero);

//returns a copy of found hero by id. Id is needed because names are redudant.
dsa_hero_t* dsa_hero_get(dsa_heros_t *heros, const int id);

//returns the pointer of first hero or NULL if no hero was found
//the last pointer element is set to NULL for easier iteration.
dsa_hero_entry_t* dsa_hero_get_all(dsa_heros_t *heros);
void dsa_hero_list_free(dsa_hero_entry_t **hero_list);

//saves as replacement the hero inside of heros(not to file) by searching the ID
void dsa_heros_save_hero(dsa_heros_t *heros, const dsa_hero_t *hero);

#endif