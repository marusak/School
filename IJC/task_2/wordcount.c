//wordcount.c
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//pocitanie slov na stdin

#include "htable.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>

void print_one(const char *key, unsigned int data){
    if (printf("%s\t%d\n",key,data) < 0)
        fprintf(stderr,"%s","Chyba pri vypise na stdeout\n");
}

int main(){
    htab_t *m = htab_init(HTAB_SIZE);
    if ( m == NULL){
        fprintf(stderr,"%s","Nepodarilo sa naalokovat priestor.\n");
        return 1;
    }

    char buffer[MAX_LEN+1]={'\0'};
    int returned_v = -1;
    int err_count = 0;
    while ((returned_v = fgetw(buffer,MAX_LEN,stdin)) != EOF){
        if (returned_v == MAX_LEN+1 && err_count == 0){
            err_count++;
            fprintf(stderr,"%s","Dlhe slovo\n");
        }

        if (returned_v == 0 ){
            fprintf(stderr,"%s","Chyba pri citani\n");
            break;
        }
        
        if (htab_lookup(m, buffer) == NULL) //nedari sa alokovat, koncime
            break;
           
    }
     
    htab_foreach(m,print_one);
    htab_statistics(m);
    htab_free(m);

    return 0;
}
