#include "eratosthenes.h"
#include "bit-array.h"
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "error.h" 

int main(){
    
    BA_create(my_array,201000000);
    Eratosthenes(my_array);
    int j = 0;
    int i = 0;
    for (i = (signed)BA_size(my_array)-1; j < 10 ;i--){
        if (!BA_get_bit(my_array,i)){
            j++;}}
    for (i=i; i<(signed)BA_size(my_array)-1; i++){
        if (!BA_get_bit(my_array,i)) {
            printf("%d\n",i);

            }}
    return 0;
}

