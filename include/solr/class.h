#ifndef SOLR_CLASS_H
#define SOLR_CLASS_H

#include "common.h"

struct solr_class{
    solr_class* super;
    solr_class** prototypes;
    int prototypes_count;
    solr_symbol* name;
};

solr_class* solr_define_class(solr_vm* vm, char* name, solr_class* super);
int solr_class_assignable(solr_class* self, solr_class* other);

static inline char*
solr_class_name(solr_class* class){
    return class->name->name;
}

#endif