//eratosthenes.c
//Riesenie IJC-DU1, priklad b),7.3.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//modul pre vypocet Eratostenovho sita

#include "bit-array.h"
#include <math.h>
#include <limits.h>
#include "eratosthenes.h"
#include "error.h"


void Eratosthenes(BitArray_t pole){
    if (BA_size(pole) < 2){
        Warning("V rozsahu 0..%d nie je prvocislo.",(signed)BA_size(pole));
    }
    int max =ceil(sqrt(BA_size(pole)));
    signed i;

    BA_set_bit(pole,0,1);
    BA_set_bit(pole,1,1);

    for (i = 2; i<=max; i++){
        if (!BA_get_bit(pole,i)) {
            int j = i+i;
            do{
            BA_set_bit(pole,j,1);}
            while ((j = j+i)<(signed)BA_size(pole));
        }
    }
}
