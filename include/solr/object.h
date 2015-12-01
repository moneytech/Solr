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
    VT_FLONUM
} solr_field_vtype;

struct solr_field{
    solr_field_vtype vtype;
    solr_class* class;
    solr_symbol* name;
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

solr_field* solr_field_primative_new(solr_vm* vm, solr_field_vtype vtype, char* name);

HEADER_END

#endif