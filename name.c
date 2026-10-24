#include "name.h"
#include "memory.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOT_FOUND_EXIT

void init_name_array(NameArray *array) {
    array->count = 0;
    array->capacity = 0;
    array->names = NULL;
}

int write_name_array(NameArray *array, Name name) {
    // Check if the value is already stored.
    for (int i = 0; i < array->count; i++) if (!strcmp(array->names[i].str, name.str)) {
        array->names[i].value = name.value;
        return i;
    }
    
    if (array->count >= array->capacity) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->names = GROW_ARRAY(Name, array->names, oldCapacity, array->capacity);
    }

    array->names[array->count++] = name;
    return array->count - 1;
}

int write_name_array_index(NameArray *array, int index, Value value) {
    if (array->count <= index) {
#ifdef NOT_FOUND_EXIT
        printf("ERROR: Invalid name\n");
        exit(1);
#endif
        return -1;
    }
    array->names[index].value = value;
    return index;
}

Value read_name_array(NameArray *array, const char *str) {
    for (int i = 0; i < array->count; i++) 
        if (!strcmp(array->names[i].str, str)) 
            return array->names[i].value;
#ifdef NOT_FOUND_EXIT
    printf("ERROR: Invalid name\n");
    exit(1);
#endif
    return NIL_VAL;
}

Value read_name_array_index(NameArray *array, int index) {
    if (array->count <= index) {
#ifdef NOT_FOUND_EXIT
        printf("ERROR: Invalid name\n");
        exit(1);
#endif
        return NIL_VAL;
    }
    return array->names[index].value;
}

int index_name_array(NameArray *array, const char *str) {
    for (int i = 0; i < array->count; i++) 
        if (!strcmp(array->names[i].str, str))
            return i;
#ifdef NOT_FOUND_EXIT
    printf("ERROR: Invalid name\n");
    exit(1);
#endif
    return -1;
}

void free_name_array(NameArray *array) {
    FREE_ARRAY(Name, array->names, array->capacity);
    init_name_array(array);
}

void print_name(Name Name) {
    printf("%s", Name.str);
}