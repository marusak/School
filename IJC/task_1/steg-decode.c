//steg-decode.c
//Riesenie IJC-DU1, priklad b),7.3.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//modul pre precitanie zasifrovanej spravy v ppm obrazku

#include "ppm.h"
#include "error.h"
#include "bit-array.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "eratosthenes.h"

int main(int argc, char *argv[]){

    //skontrolujeme prikazovu riadku
    if (argc != 2)
        FatalError("Ocakavany jeden argument, %d najdenych",argc-1);

    //nacitame data do struktury
    struct ppm *pic = ppm_read(argv[1]);
        if (pic == NULL)
            FatalError("Chyba pri nacitani obrazku");

    BA_create(ppm_eras,MAX_RES);
    Eratosthenes(ppm_eras);    
    
    int letter = 0;
    int j = 0;
    //nacitavam postupne, nastavujem si int letter, ked precitam 8 znakov,
    //vytlacim (pretypujem na signed - urcite tam nebude zaporne cislo) 
    for (int i = 2; i<(signed)(3*pic->xsize*pic->ysize);i++){
        if (!BA_get_bit(ppm_eras,i)){
            DU1_SET_BIT_(letter,j,DU1_GET_BIT_(pic->data[i+1],0));
            if (j < CHAR_BIT-1){
                j++;}
            else{
                if (letter == 0){
                    free(pic);
                    putchar(letter);
                    putchar('\n');
                    return 0;}
                if (!isprint(letter)){
                    free(pic);
                    FatalError("Netlacitelny znak");
                }
                putchar(letter);
                letter = 0;
                j = 0;
            }
        
        }
    }
    free(pic);
    FatalError("Retazec neukonceny.");
return 0;
}

