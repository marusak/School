//htab_init.c
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//funkcia pre inicializaciu tabulky

#include "htable.h"

htab_t *htab_init(unsigned size){
    htab_t *table = malloc(sizeof(htab_t)+size*sizeof(struct htab_listitem *));
    if (table == NULL)
        return NULL;
    for (unsigned i = 0; i < size; i++)//vyNULLujem kazdy prvok
       table->record[i] = NULL;
    table->htab_size = size;//ulozim velkost
    return table;
}
