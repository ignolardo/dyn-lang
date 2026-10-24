#include "value.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

void init_value_array(ValueArray *array) {
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

int write_value_array(ValueArray *array, Value value) {
    // Check if the value is already stored.
    //for (int i = 0; i < array->count; i++) if (array->values[i] == value) return i;
    
    if (array->count >= array->capacity) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count++] = value;
    return array->count - 1;
}

void free_value_array(ValueArray *array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    init_value_array(array);
}

void print_value(Value value) {
    printf("%g", AS_NUMBER(value));
}