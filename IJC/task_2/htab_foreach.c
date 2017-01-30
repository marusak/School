//htab_lookup.h
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//implementacia funkcie ktora aplikuje funckia na vsetky polozky tabulky

#include "htable.h"
#include <stdio.h>

void htab_foreach(htab_t *t,void (*function)(const char *, unsigned int)){
    if (t == NULL){
        fprintf(stderr,"%s","Neplatna tabulka\n");
        return;
    }
    if (function == NULL){
        fprintf(stderr,"%s","Neplatna funkcia\n");
        return;
    }

    struct htab_listitem *actual = NULL;
    for (unsigned int i = 0; i< t->htab_size; i++){
        actual = t->record[i];
        while (actual != NULL){
            function(actual->key, actual->data);
            actual = actual->next;
        }
    }
}

