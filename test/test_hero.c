#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "hero.h"

#include "defs.h"

EXTERN_BLOB(zip_resource, 7z);

#ifndef DEBUG_LOG_ARGS
	#if debug != 0
		#define DEBUG_LOG_ARGS(fmt, ...) printf((fmt), __VA_ARGS__)
	#else
		#define DEBUG_LOG_ARGS(fmt, ...)
	#endif
#endif

#ifndef DEBUG_LOG
	#if debug != 0
		#define DEBUG_LOG(msg) printf((msg))
	#else
		#define DEBUG_LOG(msg)
	#endif
#endif

static void test_heros_alloc_free() {
    DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	assert(heros != NULL );
	assert(heros->basehero != NULL );
	assert(heros->breeds != NULL );
	assert(heros->cultures != NULL );
	assert(heros->professions != NULL );
	assert(heros->pro_contra != NULL );
	assert(heros->specialabilities != NULL );
	assert(heros->spells != NULL );
	assert(heros->talents != NULL );
	assert(heros->heros != NULL);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

    DEBUG_LOG("<<<\n");
}

static void test_heros_add_hero() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	dsa_hero_free(&baradon);

	baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon der Zweite");
	dsa_hero_free(&baradon);

	baradon = dsa_hero_new(heros, (const unsigned char*)"Ellidon");
	dsa_hero_free(&baradon);

	baradon = dsa_hero_new(heros, (const unsigned char*)"Faleon");
	dsa_hero_free(&baradon);

	#if debug > 1
		int writtenbytes = xmlSaveFileEnc("-", heros->heros->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	assert(baradon == NULL);

	dsa_hero_entry_t *list = dsa_hero_get_all(heros);
	dsa_hero_entry_t *cur;

	int cnt = 0;
	cur = list;

	while( cur && cur->name != NULL && cur->id != -1 ) {
		#if debug > 0
			DEBUG_LOG_ARGS("all includes %s with id %ld\n", cur->name, cur->id);
		#endif
		cnt++;
		cur++;
	}
	assert(cnt == 4);

	dsa_hero_t **herosl = malloc(cnt*sizeof(dsa_hero_t *));

	cur = list;
	for(int i = 0; i < cnt; ++i, ++cur) {
		herosl[i] = dsa_hero_get(heros, cur->id);
		assert(herosl[i] != NULL);
		dsa_hero_free(&herosl[i]);
	}

	free(herosl);

	dsa_hero_list_free(&list);

	assert(list == NULL);

	dsa_hero_t* found = dsa_hero_get(heros, 0);

	assert(found != NULL);

	dsa_hero_free(&found);

	found = dsa_hero_get(heros, 2);

	xmlSetProp(xmlDocGetRootElement(found->xml->doc), (xmlChar *)"name", (xmlChar *)"Also Baradon :D");

	dsa_heros_save_hero(heros, found);

	#if debug > 0
		xmlSaveFileEnc("-", heros->heros->doc,"UTF-8");
	#endif

	assert(found != NULL);

	dsa_hero_free(&found);

	found = dsa_hero_get(heros, 12);

	assert(found == NULL);

	dsa_hero_free(&found);

	dsa_heros_delete_hero(heros, 0);
	dsa_heros_delete_hero(heros, 3);

	found = dsa_hero_get(heros, 2);
	dsa_heros_delete_hero_obj(heros, found);

	found = dsa_hero_get(heros, 2);

	assert(found == NULL);

	#if debug > 0
		printf("TEST OUT DELETED HEROS");
		xmlSaveFileEnc("-", heros->heros->doc,"UTF-8");
	#endif

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

    DEBUG_LOG("<<<\n");
}

static void	test_heros_add_breed() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_breed(heros, baradon, (const unsigned char*)"Die Tulamiden");
	dsa_heros_add_breed(heros, baradon, (const unsigned char*)"Die Mittelländer");

	dsa_heros_set_col_hair_by_name(baradon, (const unsigned char*)"dunkelblond");
	dsa_heros_set_col_hair_by_name(baradon, (const unsigned char*)"rot");

	dsa_heros_set_col_eye_by_name(baradon, (const unsigned char*)"grün");
	dsa_heros_set_col_eye_by_name(baradon, (const unsigned char*)"grau");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif
	
	dsa_heros_set_col_eye_by_dice(baradon);
	dsa_heros_set_col_hair_by_dice(baradon);

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_heros_set_height_weight_by_dice(baradon);

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_heros_set_height_weight_by_value(baradon, (const unsigned char *)"60");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void	test_heros_add_culture(){
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_culture(heros, baradon, (const unsigned char*)"Mittelländische Städte");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif
	
	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void	test_heros_add_pro() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_pro(heros, baradon, (const unsigned char*)"Normal", (const unsigned char*)"Amtsadel");
	dsa_heros_add_pro(heros, baradon, (const unsigned char*)"Magisch", (const unsigned char*)"Unknown");
	dsa_heros_add_pro(heros, baradon, (const unsigned char*)"Normal", (const unsigned char*)"Adliges Erbe");
	dsa_heros_add_pro(heros, baradon, (const unsigned char*)"Magisch", (const unsigned char*)"Astrale Regeneration III");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_heros_remove_pro(baradon, (const unsigned char*)"Astrale Regeneration III");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void	test_heros_add_contra() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_contra(heros, baradon, (const unsigned char*)"Normal", (const unsigned char*)"Aberglaube");
	dsa_heros_add_contra(heros, baradon, (const unsigned char*)"Magisch", (const unsigned char*)"Unknown");
	dsa_heros_add_contra(heros, baradon, (const unsigned char*)"Normal", (const unsigned char*)"Albino");
	dsa_heros_add_contra(heros, baradon, (const unsigned char*)"Magisch", (const unsigned char*)"Animalische Magie");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_heros_remove_contra(baradon, (const unsigned char*)"Animalische Magie");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void	test_heros_add_specialability() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_specialability(heros, baradon, (const unsigned char*)"Allgemeine", (const unsigned char*)"Akklimatisierung");
	dsa_heros_add_specialability(heros, baradon, (const unsigned char*)"Allgemeine", (const unsigned char*)"Unknown");
	dsa_heros_add_specialability(heros, baradon, (const unsigned char*)"Allgemeine", (const unsigned char*)"Fälscher");
	dsa_heros_add_specialability(heros, baradon, (const unsigned char*)"Kampf", (const unsigned char*)"Ausfall");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_heros_remove_specialability(baradon, (const unsigned char*)"Ausfall");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void	test_heros_add_talent() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_talent(heros, baradon, (const unsigned char*)"Kampf", (const unsigned char*)"Anderthalbhänder");
	dsa_heros_add_talent(heros, baradon, (const unsigned char*)"Natur", (const unsigned char*)"Unknown");
	dsa_heros_add_talent(heros, baradon, (const unsigned char*)"Natur", (const unsigned char*)"Fallenstellen");
	dsa_heros_add_talent(heros, baradon, (const unsigned char*)"Natur", (const unsigned char*)"Fährtensuchen");
	dsa_heros_add_talent(heros, baradon, (const unsigned char*)"Wissen", (const unsigned char*)"Gesteinskunde");
	dsa_heros_add_talent(heros, baradon, (const unsigned char*)"Handwerk", (const unsigned char*)"Alchimie");

	dsa_heros_talent_inc(baradon, (const unsigned char*)"Anderthalbhänder");
	dsa_heros_talent_inc(baradon, (const unsigned char*)"Anderthalbhänder");
	dsa_heros_talent_inc(baradon, (const unsigned char*)"Anderthalbhänder");
	dsa_heros_talent_inc(baradon, (const unsigned char*)"Anderthalbhänder");

	dsa_heros_talent_inc(baradon, (const unsigned char*)"Fallenstellen");
	dsa_heros_talent_inc(baradon, (const unsigned char*)"Fallenstellen");

	dsa_heros_talent_inc(baradon, (const unsigned char*)"Fallenstellen");
	dsa_heros_talent_inc(baradon, (const unsigned char*)"Gesteinskunde");

	dsa_heros_talent_inc(baradon, (const unsigned char*)"Heilkunde Wunden");
	dsa_heros_talent_inc(baradon, (const unsigned char*)"Heilkunde Wunden");
	dsa_heros_talent_inc(baradon, (const unsigned char*)"Heilkunde Wunden");
	dsa_heros_talent_inc(baradon, (const unsigned char*)"Heilkunde Wunden");

	dsa_heros_talent_dec(baradon, (const unsigned char*)"Anderthalbhänder");
	dsa_heros_talent_dec(baradon, (const unsigned char*)"Anderthalbhänder");

	dsa_heros_talent_dec(baradon, (const unsigned char*)"Heilkunde Wunden");
	dsa_heros_talent_dec(baradon, (const unsigned char*)"Heilkunde Wunden");
	dsa_heros_talent_dec(baradon, (const unsigned char*)"Heilkunde Wunden");
	
	dsa_heros_set_name(baradon, (const unsigned char*)"Elfariona");
	dsa_heros_set_gp(baradon, (const unsigned char*)"130");
	dsa_heros_set_title(baradon, (const unsigned char*)"Ich bin ein hübscher Elf :P");
	dsa_heros_set_status(baradon, (const unsigned char*)"Ich bin mit der NAtur verheiratet :P :D");
	dsa_heros_set_look(baradon, (const unsigned char*)"Wie mein Titel schon sagt, ich bin hübscher als du!!");
	dsa_heros_set_story(baradon, (const unsigned char*)"Meine Geschichte ist heroisch uind lang, Barden werden Äonen von ihr singen.");

	dsa_heros_set_female(baradon);

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_heros_remove_talent(baradon, (const unsigned char*)"Alchimie");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void	test_heros_add_spell() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_spell(heros, baradon, (const unsigned char*)"Dämonologie", (const unsigned char*)"Dämon bannen");
	dsa_heros_add_spell(heros, baradon, (const unsigned char*)"Natur", (const unsigned char*)"Unknown");
	dsa_heros_add_spell(heros, baradon, (const unsigned char*)"Dämonologie", (const unsigned char*)"Dämon beschwören");
	dsa_heros_add_spell(heros, baradon, (const unsigned char*)"Dämonologie", (const unsigned char*)"Dämon beschwören");
	dsa_heros_add_spell(heros, baradon, (const unsigned char*)"Veränderung", (const unsigned char*)"Dublicatus Doppelpein");
	dsa_heros_add_spell(heros, baradon, (const unsigned char*)"Illusion", (const unsigned char*)"Horriphobus");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_heros_remove_spell(baradon, (const unsigned char*)"Horriphobus");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void	test_heros_add_liturgy() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_liturgie(heros, baradon, (const unsigned char*)"Praios", (const unsigned char*)"Sonnenglanz");
	dsa_heros_add_liturgie(heros, baradon, (const unsigned char*)"Natur", (const unsigned char*)"Unknown");
	dsa_heros_add_liturgie(heros, baradon, (const unsigned char*)"Phex", (const unsigned char*)"Reichmacher");
	dsa_heros_add_liturgie(heros, baradon, (const unsigned char*)"Phex", (const unsigned char*)"Reichmacher");
	dsa_heros_add_liturgie(heros, baradon, (const unsigned char*)"Boron", (const unsigned char*)"Totenglocke");
	dsa_heros_add_liturgie(heros, baradon, (const unsigned char*)"Boron", (const unsigned char*)"Friedvoller Schlaf");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_heros_remove_liturgie(baradon, (const unsigned char*)"Friedvoller Schlaf");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif

	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void	test_heros_add_profession(){
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_add_profession(heros, baradon, (const unsigned char*)"Bauer Erntehelfer/in");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif
	
	dsa_heros_add_pro_calc_group(heros, baradon, (const unsigned char *)"Ausrüstungsvorteil");
	dsa_heros_add_pro_calc_group(heros, baradon, (const unsigned char *)"Astrale Regeneration III");

	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void test_heros_inc_dec_attributes() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	dsa_heros_t * heros = dsa_heros_new_archiv(ar);

	dsa_hero_t *baradon = dsa_hero_new(heros, (const unsigned char*)"Baradon");
	
	dsa_heros_attr_inc(baradon, (const unsigned char*)"MU");
	dsa_heros_attr_inc(baradon, (const unsigned char*)"MU");
	dsa_heros_attr_inc(baradon, (const unsigned char*)"MU");
	dsa_heros_attr_inc(baradon, (const unsigned char*)"MU");

	dsa_heros_attr_inc(baradon, (const unsigned char*)"KL");
	dsa_heros_attr_inc(baradon, (const unsigned char*)"KL");
	dsa_heros_attr_inc(baradon, (const unsigned char*)"KL");

	for (int i = 0; i < 7; ++i) { dsa_heros_attr_inc(baradon, (const unsigned char*)"SO"); }

	dsa_heros_attr_dec(baradon, (const unsigned char*)"SO");
	dsa_heros_attr_dec(baradon, (const unsigned char*)"SO");

	#if debug > 0
		xmlSaveFileEnc("-", baradon->xml->doc,"UTF-8");
	#endif
	
	dsa_hero_free(&baradon);

	dsa_heros_free(&heros);

	assert(heros == NULL);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

int main(int argc, char **argv) {

	DEBUG_LOG(">> Start taw tests:\n");

	srand(time(NULL));

	test_heros_alloc_free();

	test_heros_add_hero();

	test_heros_add_breed();

	test_heros_add_culture();

	test_heros_add_profession();

	test_heros_add_pro();
	
	test_heros_add_contra();

	test_heros_add_specialability();

	test_heros_add_talent();
	
	test_heros_add_spell();
	
	test_heros_add_liturgy();

	test_heros_inc_dec_attributes();

	DEBUG_LOG("<< end taw tests:\n");

	return 0;
}