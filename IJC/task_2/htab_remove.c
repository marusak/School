//htab_remove.h
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//funkcia pre zmazanie zaznamu

#include "htable.h"

void htab_remove(htab_t *t, const char * key){
    if (t == NULL || key == NULL){
        fprintf(stderr,"%s","Neplatna tabulka alebo kluc\n");
        return ;
    }
    unsigned hash = hash_function(key,t->htab_size);

    struct htab_listitem *actual = t->record[hash];
    struct htab_listitem *previous = t->record[hash];
    
    if (actual == NULL){
        fprintf(stderr,"%s","Kluc sa v tabulke nevyskytuje\n");
        return ;
    }


    while (1){
        if (strcmp(key, actual->key) == 0){
            if (previous == actual) // som na prvom v riadku
                t->record[hash] = actual->next;

            previous->next = actual->next;
            free(actual->key);
            free(actual);
            return;
        }
        else
            previous = actual;
            actual = actual->next;
        if (actual == NULL){
            fprintf(stderr,"%s","Kluc sa v tabulke nevyskytuje\n");
            return ;
        }
    }
}
