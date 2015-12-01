#ifndef SOLR_VM_H
#define SOLR_VM_H

#include "common.h"

HEADER_BEGIN

typedef struct solr_classpool solr_classpool;

struct solr_vm{
    solr_classpool* classpool;
    solr_class* class_object;
    solr_class* class_string;
    solr_class* class_class;
    solr_class* class_fixnum;
    solr_class* class_flonum;
    solr_class* class_char;
};

solr_vm* solr_vm_new();

solr_class* solr_vm_lookup_class(solr_vm* vm, char* name);
void solr_vm_define_class(solr_vm* vm, solr_class* class);
int solr_vm_has_class(solr_vm* vm, solr_symbol* sym);

HEADER_END

#endif