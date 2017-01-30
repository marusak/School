//hash_function.h
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//hashovacia funkcia

#include "hash_function.h"

//hashovacia funcia podla zadania
unsigned int hash_function(const char *str, unsigned htab_size) {
    unsigned int h = 0;
    const unsigned char *p;
    for (p = (const unsigned char*)str; *p!='\0'; p++)
        h=65599*h+*p;
    return h % htab_size;
}
