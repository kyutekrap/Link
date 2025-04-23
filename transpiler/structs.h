#ifndef structs_H
#define structs_H

#include "./enums.h"
#include "../utils/utils.h"

typedef struct {
    PropertyType property_type;
    char *property_value;
} Property;

typedef struct {
    char *function_type;
    char *function_value;
} Function;

typedef struct {
    char *name;
    StrList imports;
    YesNo debug;
    IdentifierType identifier_type;
} TranspilerSummary;

typedef struct {
    ComparatorType type;
    char *value;
} Comparator;

#endif