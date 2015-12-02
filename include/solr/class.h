#ifndef SOLR_CLASS_H
#define SOLR_CLASS_H

#include "common.h"
#include "object.h"

struct solr_class{
    solr_class* super;
    solr_class** prototypes;
    int prototypes_count;
    solr_symbol* name;
    unsigned int object_size;
    solr_field* fields;
    unsigned int fields_count;
};

solr_class* solr_define_class(solr_vm* vm, char* name, solr_class* super, int fields_count);
void solr_class_init(solr_vm* vm, solr_class* class);
void solr_class_define_field(solr_class* class, char* name, solr_field_vtype vtype);
int solr_class_assignable(solr_class* self, solr_class* other);
solr_field* solr_class_get_field(solr_class* self, char* name);
solr_field* solr_class_lookup_field(solr_class* self, char* name);

#define solr_define_basic_class(vm, name, fcount) (solr_define_class(vm, name, vm->class_object, fcount))

static inline char*
solr_class_name(solr_class* class){
    return class->name->name;
}

#endif