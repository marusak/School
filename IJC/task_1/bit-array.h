//bit-array.h
//Riesenie IJC-DU1, priklad a),7.3.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
// rozhranie prepracu s bitovym polom

#include "error.h"
#include "limits.h"

#ifndef BIT_ARRAY_MACROS
#define BIT_ARRAY_MACROS

typedef unsigned long* BitArray_t;//typ bitoveho pola

//Pocet bitov v unsigned long 
#define bits_in_ul (sizeof(unsigned long)*CHAR_BIT)

//Pocet bitov v type, akeho su prvky pola 
#define bits_in_type(variable) (sizeof(variable[0])*CHAR_BIT)

//vypise i-ty byt z pola p
#define DU1_GET_BIT_(p,i) (p& (1l<<i)) != 0

//nastavy i-ty byt pola p na hodnotu b
#define DU1_SET_BIT_(p,i,b) b? (p |= 1l<<i) : (p &= ~(1l<<i))

//Vytvori pole jmeno_pole pre velikost bitov
#define BA_create(jmeno_pole,velikost) unsigned long  jmeno_pole [((velikost % bits_in_ul == 0) ?  \
                                                                  ((velikost / bits_in_ul)+1):      \
                                                                  ((velikost / bits_in_ul)+2))]= {velikost,0}



/*#define BA_create(jmeno_pole,velikost) do {\
   if (velikost < 1) ( FatalError("Pole velkosti %ld nie je platnym polom",velikost));\
   else BA_create_no_control(jmeno_pole,velikost);}\
    while (0)
    */

#ifndef USE_INLINE //pouzijeme makra

//vrati velkost bitoveho pola v argumente
#define BA_size(jmeno_pole) jmeno_pole[0]


//nastavi index pola na vyraz - vyraz len 0-1
#define BA_set_bit(jmeno_pole,index,vyraz) ( (index>=0 && index < (signed)(long)BA_size(jmeno_pole))?\
                        ( (vyraz < 2 && vyraz >= 0) ? \
                        (DU1_SET_BIT_(jmeno_pole[index/bits_in_type(jmeno_pole)+1],index%bits_in_type(jmeno_pole),vyraz)): \
                        (FatalError("Vyraz %d mimo rozsah 0..1", vyraz ),1)):\
                        (FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)BA_size(jmeno_pole)),1) )


//vrati hodnotu bitu na pozicii index
#define BA_get_bit(jmeno_pole,index) ((index >=0 && index < (signed)(BA_size(jmeno_pole))) ? \
                        (DU1_GET_BIT_(jmeno_pole[index/bits_in_type(jmeno_pole)+1],index%bits_in_type(jmeno_pole))): \
                        (FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)BA_size(jmeno_pole)),1))

#else 

//inline verzia BA_size
inline long unsigned BA_size(BitArray_t jmeno_pole){
    return (signed)jmeno_pole[0];
}


//inline verzia BA_set_bit
inline void BA_set_bit(BitArray_t jmeno_pole, int index, int vyraz){
    if (index < (signed)BA_size(jmeno_pole) && index >= 0)
        if (vyraz >=0 && vyraz < 2) 
            {DU1_SET_BIT_(jmeno_pole[index/bits_in_type(jmeno_pole)+1],index%bits_in_type(jmeno_pole),vyraz);}
        else FatalError("Vyraz %d mimo rozsah 0..1", vyraz );
    else {FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)BA_size(jmeno_pole));}
    }


//inline verzia BA_get_bit
inline int BA_get_bit(BitArray_t jmeno_pole, int index){
    if (index < (signed)BA_size(jmeno_pole) && index >=0 ){
        return (DU1_GET_BIT_(jmeno_pole[index/bits_in_type(jmeno_pole)+1],index%bits_in_type(jmeno_pole)));
        }
    else {(FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)BA_size(jmeno_pole)));
        return 0;
    }
    return 0;
}


#endif // pre USE_INLINE
#endif //pre cele BIT_ARRAY_MACROS
