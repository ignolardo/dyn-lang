#ifndef VALUE_H_
#define VALUE_H_

#include <stdbool.h>

typedef enum {
    VAL_NUMBER,
    VAL_BOOL,
    VAL_NIL,
} ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        bool boolean;
    } as;
} Value;

#define NUMBER_VAL(value)   ((Value){.type=VAL_NUMBER, .as={.number=value}})
#define BOOL_VAL(value)     ((Value){.type=VAL_BOOL, .as={.boolean=value}})
#define NIL_VAL             ((Value){.type=VAL_NIL, .as={.number=0}})

#define AS_NUMBER(value)    ((value).as.number)
#define AS_BOOL(value)      ((value).as.boolean)

#define IS_NUMBER(value)    ((value).type == VAL_NUMBER)
#define IS_BOOL(value)      ((value).type == VAL_BOOL)
#define IS_NIL(value)       ((value).type == VAL_NIL)

typedef struct {
    int count;
    int capacity;
    Value *values;
} ValueArray;

void init_value_array(ValueArray *array);
int write_value_array(ValueArray *array, Value value);
void free_value_array(ValueArray *array);
void print_value(Value value);

#endif