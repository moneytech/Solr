#ifndef SOLR_OBJECT_H
#define SOLR_OBJECT_H

#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "vm.h"

HEADER_BEGIN

struct solr_object{
    solr_class* class;
};

typedef enum{
    VT_STRING,
    VT_CHAR,
    VT_FIXNUM,
    VT_FLONUM,
    VT_MAX
} solr_field_vtype;

struct solr_field{
    solr_field_vtype vtype;
    solr_class* class;
    solr_symbol* name;
    unsigned int offset;
};

static inline solr_class*
solr_vtype_box(solr_vm* vm, solr_field_vtype vtype){
    switch(vtype){
        case VT_STRING: return vm->class_string;
        case VT_CHAR: return vm->class_char;
        case VT_FLONUM: return vm->class_flonum;
        case VT_FIXNUM: return vm->class_fixnum;
        default:{
            printf("Unknown Primative: %d\n", vtype);
            abort();
        }
    }
}

#define SOLR_OBJECT_OFFSET (sizeof(solr_object))

static inline uint8_t*
solr_object_fields(solr_object* obj){
    return (uint8_t*) obj + SOLR_OBJECT_OFFSET;
}

#define DEC_FIELD_SETTER(type) \
    static inline void \
    solr_field_set_ ## type(solr_object* obj, solr_field* field, type value){ \
        uint8_t* fields = solr_object_fields(obj); \
        *(type*) &fields[field->offset] = value; \
    }

#define DEC_FIELD_GETTER(type) \
    static inline type \
    solr_field_get_ ## type(solr_object* obj, solr_field* field){ \
        uint8_t* fields = solr_object_fields(obj); \
        return *(type*) &fields[field->offset]; \
    }

DEC_FIELD_GETTER(long);
DEC_FIELD_GETTER(char);
DEC_FIELD_GETTER(double);

DEC_FIELD_SETTER(long);
DEC_FIELD_SETTER(char);
DEC_FIELD_SETTER(double);

solr_object* solr_object_new(solr_vm* vm, solr_class* class);

HEADER_END

#endif