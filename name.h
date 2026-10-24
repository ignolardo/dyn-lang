#ifndef NAME_H_
#define NAME_H_

#include "value.h"
#include <stddef.h>

typedef struct {
    const char* str;
    Value value;
} Name;

typedef struct {
    int count;
    int capacity;
    Name *names;
} NameArray;

void init_name_array(NameArray *array);
int write_name_array(NameArray *array, Name name);
int write_name_array_index(NameArray *array, int index, Value value);
Value read_name_array(NameArray *array, const char *str);
Value read_name_array_index(NameArray *array, int index);
int index_name_array(NameArray *array, const char *str);
void free_name_array(NameArray *array);
void print_name(Name name);

#endif