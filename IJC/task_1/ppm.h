//ppm.h
//Riesenie IJC-DU1, priklad b),7.3.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//rozhranie pre pracu s formatom ppm

#ifndef USE_PPM_
#define USE_PPM_

#define MAX_RES (5000*5000*3)
struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];
};

struct ppm * ppm_read(const char * filename);

int ppm_write(struct ppm *, const char * filename);
#endif

