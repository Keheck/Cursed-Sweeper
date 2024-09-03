#include "int_list.h"

typedef struct int_list_node int_list_node_t;

void add_to_list(int_list_t *list, uint32_t value);


struct int_list {
    int_list_node_t *head;
    int_list_node_t *tail;
};

struct int_list_node {
    int_list_node_t *next;
    uint32_t value;
};


int_list_t *create_list(size_t size) {
    int_list_t *result = (int_list_t*)malloc(sizeof(*result));
    result->head = result->tail = 0;
    
    for(int i = 0; i < size; i++) {
        add_to_list(result, i);
    }

    return result;
}

void add_to_list(int_list_t *list, uint32_t value) {
    int_list_node_t *result = (int_list_node_t*)malloc(sizeof(*result));
    result->next = 0;
    result->value = value;

    if(list->head == 0 && list->tail == 0) {
        list->head = list->tail = result;
    }
    else {
        list->tail->next = result;
        list->tail = result;
    }
}

uint32_t get_index(int_list_t *list, size_t index) {
    int_list_node_t *curr = list->head;

    for(int i = 0; i < index && curr != NULL; i++) {
        curr = curr->next;
    }

    return curr->value;
}

void remove_index(int_list_t *list, size_t index) {
    if(list->head == NULL) return;

    if(list->head == list->tail) {
        if(index > 0)
            return;
        
        free(list->head);
        list->head = list->tail = 0;
        return;
    }  

    int_list_node_t *prev = 0;
    int_list_node_t *curr = list->head;

    for(int i = 0; i < index; i++) {
        if(curr == NULL) return;

        prev = curr;
        curr = curr->next;
    }

    if(curr == list->tail) {
        list->tail = prev;
    }

    if(curr == list->head) {
        list->head = curr->next;
        return;
    }

    prev->next = curr->next;
    free(curr);
}

void free_list(int_list_t *list) {
    int_list_node_t *curr = list->head;

    while(curr != NULL) {
        int_list_node_t *next = curr->next;
        free(curr);
        curr = next;
    }

    free(list);
}