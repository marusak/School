//htab_clear.c
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//funkcia pre vymazanie tabulky

#include "htable.h"

void htab_clear(htab_t *t){
    if (t == NULL)
        return;
    struct htab_listitem *actual=NULL;
    struct htab_listitem *next=NULL;
    for (unsigned i = 0; i<t->htab_size; i++){
        actual = t->record[i];
        while (actual != NULL){
            next = actual->next;
            free(actual->key);
            free(actual);
            actual = next;
        }
    t->record[i] = NULL;
    }

}



