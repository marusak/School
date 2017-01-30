//ppm.c
//Riesenie IJC-DU1, priklad b),7.3.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//modul pre pracu s formatom ppm

#include "ppm.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//nacitanie dat z obrazku do struktury
//vracia ukazatel na strukturu, pri chybe NULL
struct ppm *ppm_read(const char *filename){
    
    char var[4];
    char expected[3] = {"P6\n"};
    int colors;
    unsigned xsize;
    unsigned ysize;

    //kontrola otvorenia suboru
    FILE * subor = fopen(filename,"rb");
    if (subor == NULL)
        FatalError("Subor %s neexistuje",filename);
    
    //kontrola formatu
    fgets(var,3,subor);
    if (!strncmp(expected,var,3)){
        Warning("Ocakavany format P6, ale %s najdene.",var);
	fclose(subor);
        return NULL;}
    
    //nacitanie rozmerov obrazku a farbnej hlbky
    if (fscanf(subor, "%u %u",&xsize, &ysize) != 2 || xsize <=0 || ysize <=0){
        if (fclose(subor) == EOF)
            FatalError("Nepodarilo sa zatvorit obrazok.");
        FatalError("Chybaju udaje o obrazku.");}
    fscanf(subor,"%d",&colors);
    if (colors != 255){
        Warning("Ocakavana farebna hlbka 255 ale %d najdena.",colors);
	fclose(subor);
        return NULL;}
    fgets(var,1,subor);

    //nacitanie dat do struktury
    struct ppm *pic = malloc(sizeof(struct ppm)+3*xsize*ysize*sizeof(char));
        if (pic == NULL)
            if (fclose(subor) == EOF){
                FatalError("Nepodarilo sa zatvorit obrazok.");
            FatalError("Nepodarilo sa naalokovat priestor na disku.");}
   
    pic->xsize = xsize;
    pic->ysize = ysize; 
    for (unsigned i=0;i<3*pic->xsize*pic->ysize;i++){
    if( (pic->data[i]=fgetc(subor)) == EOF){
        free(pic);
            if (fclose(subor) == EOF)
                FatalError("Nepodarilo sa zatvorit obrazok.");
        FatalError("Nedostatok dat v obrazku.");}}
    if (fclose(subor) == EOF){
        free(pic);
        FatalError("Nepodarilo sa zatvorit obrazok.");
    }
    return pic;
    }


int ppm_write(struct ppm *p, const char *filename){
    

 //otvorime subor na zapis
    FILE *subor = fopen(filename,"wb");
    if (subor == NULL){
        free(p);
        FatalError("Nepodarilo sa otvorit %s.",filename);
    }
    //zapiseme hlavicku
    fprintf(subor, "P6 %u %u 255 ",p->xsize,p->ysize);

    //zapiseme udaje
    for (unsigned i = 1; i<3*p->xsize*p->ysize; i++){
        if(fputc(p->data[i],subor) == EOF){
            free(p);
            if (fclose(subor) == EOF){
                Warning("Nepodarilo sa zatvorit obrazok.");
                return -1;}
            FatalError("Nepodarilo sa zapisat data do suboru.");
        }
    }
    if(fprintf(subor,"%c",EOF) != 1){
        free(p);
            if (fclose(subor) == EOF){
                Warning("Nepodarilo sa zatvorit obrazok.");
                return -1;}
            FatalError("Nepodarilo sa zapisat data do suboru.");
        }
    
    if (fclose(subor) == EOF){
        Warning("Nepodarilo sa zatvorit obrazok.");
        return -1;}
    return 0;
}
    
    


    
  

