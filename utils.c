#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

int listInit(List* l, int max_elmet_size){
    l->max_element_size = max_elmet_size;
    l->max_size = 10;
    l->size = 0;
    l->data = malloc(max_elmet_size * l->max_size);
    return 0;
}

void listAddEnd(List* l, void* elmt){
    // check for expansion
    if (l->size+1 >= l->max_size){
        l->max_size = l->max_size * 2;
        // copy data
        void* tmp = malloc(l->max_element_size * l->max_size);
        memcpy(tmp, l->data, l->max_element_size * l->size);
        free(l->data);
        l->data = tmp;
    }

    // Add data to end of the list 
    memcpy((char*)l->data+l->size * l->max_element_size, elmt, l->max_element_size);

    l->size++;
}

/**
 * Returns a pointer to the data in the list
 */
void *listGet(List* l, int index){
    // pointer to data array
    char* tmp = (char*)l->data;
    // position = pointer + offset
    return tmp+index;
}

