#ifndef INT_LIST_H
#define INT_LIST_H

#include <stdint.h>
#include <stdlib.h>

typedef struct int_list int_list_t;

int_list_t *create_list(size_t size);
uint32_t get_index(int_list_t *list, size_t index);
void remove_index(int_list_t *list, size_t index);
void free_list(int_list_t *list);

#endif