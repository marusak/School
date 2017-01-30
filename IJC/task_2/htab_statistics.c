//hlavicka

#include "htable.h"

int count_record(struct htab_listitem *rcd){
    int size = 0;
    struct htab_listitem *actual = rcd;
    while (actual != NULL){
        size++;
        actual = actual->next;
    }
    return size;
}


void htab_statistics(htab_t *t){
    int max = 0;
    int min = count_record(t->record[0]);
    int sum = min;
    int not_empty;
    if (min > 0) 
        not_empty = 1;
    else
        not_empty = 0;
    for ( int i = 1; i<t->htab_size; i++){
        int new = count_record(t->record[i]);
        if (new > max) max = new;
        if (new < min) min = new;
        if (new != 0) not_empty += 1;
        sum += new;
    }
    printf("Maximalna dlzka zoznamu v tabulke je %d.\n",max);
    printf("Minimalna dlzka zoznamu v tabulke je %d.\n",min);
    if (not_empty == 0) 
        printf("Priemerna dlzka zoznamu v tabulke je %e.\n",(double)sum);
    else
        printf("Priemerna dlzka zoznamu v tabulke je %e.\n",(double)sum/not_empty);
}


