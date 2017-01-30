//htab_lookup.h
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//funkcia pre pridanie/zvysenie pocitadla zaznamu


#include "htable.h"
#include "hash_function.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define err_malloc do{ \
    fprintf(stderr,"%s","Chyba pri alokacii\n");\
    return NULL; \
    } while(0)

struct htab_listitem *htab_lookup(htab_t *t, const char *key){
   
    if (t == NULL || key == NULL){
        fprintf(stderr,"%s","Neplatna tabulka alebo prazdny kluc\n");
        return NULL;
    }

    struct htab_listitem *actual_ptr = NULL;

    unsigned hash = hash_function(key,t->htab_size); //vypocitame hash
    
    if ( t->record[hash] == NULL ){ //este neexistuje nic pod danym hashom
       
        struct htab_listitem *new_item = malloc(sizeof(struct htab_listitem));//vytvorime pomocnu polozku
        if (new_item == NULL)    
            err_malloc;
    
        new_item->key = malloc(strlen(key)+1);//naalokujem priestor pre kluc o jedna vacsi pre \0
        if (new_item->key == NULL){
            free (new_item);
            err_malloc;
        }
        new_item->data = 1; //pocitadlo nastavim na 1
        new_item->next = NULL; // uzemnim
        strcpy(new_item->key,key); // ulozim kluc
        new_item->key[strlen(key)] = '\0'; // pre istotu ulozim na koniec klucu \0

        t->record[hash] = new_item; //vlozim do tabulky

        return new_item; 
    }


    else { // uz nieco existuje pod danym klucom
    
        actual_ptr = t->record[hash];
        while (1){//prechadzam zoznam pod jednym hashom
            if (strcmp(key, actual_ptr->key) == 0){//ak najdem kluc ...
                actual_ptr->data += 1;//zvysom pocitadlo...
                return actual_ptr;// a vratim ukazatel
            }
            if (actual_ptr->next == NULL)//ak sme na poslednej polozke...
                break;//tak vyskocim z cyklu...
            else actual_ptr = actual_ptr->next;//inak idem na dalsiu polozku
        }

        //vyskocil som z cyklu, teda musim pridat na koniec
        struct htab_listitem *to_add = malloc(sizeof(struct htab_listitem));
        if (to_add == NULL)
            err_malloc;
        //actual_ptr je ukazatel na posledny v liste

        to_add->key = malloc(strlen(key)+1);//naalokujem priestor pre kluc o jedna vacsi pre \0
        if (to_add->key == NULL){
            free(to_add);
            err_malloc;
        }
    
        to_add->data = 1; //pocitadlo nastavim na 1
        to_add->next = NULL; // uzemnim
        strcpy(to_add->key,key); // ulozim kluc
        to_add->key[strlen(key)] = '\0'; // pre istotu ulozim na koniec klucu \0

        actual_ptr->next = to_add;//pridam na koniec zoznamu
        return to_add;
    }
    return NULL; // sem by sa funkcia nikdy nemala dostat
}


