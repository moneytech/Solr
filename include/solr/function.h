#ifndef SOLR_FUNCTION_H
#define SOLR_FUNCTION_H

#include "common.h"
#include "opcode.h"

HEADER_BEGIN

struct solr_native{
    char* class_name;
    char* meth_name;
    void* ptr;
};

solr_native* solr_native_new(char* class, char* meth, void* ptr);
void solr_native_free(solr_native* self);
int solr_register_native(solr_vm* mv, solr_native* self);
void* solr_lookup_native(solr_vm* vm, char* class_name, char* meth_name);

HEADER_END

#endif