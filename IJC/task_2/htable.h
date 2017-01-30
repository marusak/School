//htable.h
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//rozhranie pre pracu s hashovaciou tabulkou


#ifndef HTABLE_INCLUDED
#define HTABLE_INCLUDED

//Snaha je vybrat prvocislo, aby sme zabranili zhlukovaniu zaznamu na urcitych indexoch.
//Dalej je vhodne vybrat take prvocislo, ktore je dostatocne vzdialene od mocnin 2 (z oboch stran)
//Nakoniec je rozhodujuca velkost vstupu - pre mna neznama, tak som sa rozhodol pre cislo 12 289
//zdroj: http://planetmath.org/goodhashtableprimes

#define HTAB_SIZE  12289
#define MAX_LEN 127

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_function.h"

//polozka hashovacej tabulky
struct htab_listitem{
    char *key; //ukazatel na stringovy kluc
    int data; // pocet 
    struct htab_listitem *next; 
};

//novy datovy typ hashovacej tabulky
typedef struct Htab_t{
    unsigned long htab_size;
    struct htab_listitem *record[];
}htab_t;

//vyhlada zaznam s klucom key - ak nenajde prida, ak najde zvysi pocitadlo
struct htab_listitem *htab_lookup(htab_t *t, const char *key);

//inicializuje hashovaciu tabulku
htab_t *htab_init(unsigned int size);

//aplikuje funckiu function pre kazdy prvok tabulky
void htab_foreach(htab_t *t,void (*function)(const char *, unsigned int));

//vyhladanie kluca v tabulke - zvysenie pocitadla alebo pridanie
struct htab_listitem *htab_lookup(htab_t *t, const char *key);

//vymaze vsetky prvky tabulky
void htab_clear(htab_t *t);

//uvolni celu tabulku - vola htab_clear
void htab_free(htab_t *t);

//odstrani prvok tabulky t s klucom key
void htab_remove(htab_t *t, const char * key);

//vypise min,max a priemernu dlzku zaznamu
void htab_statistics(htab_t *t);
#endif
