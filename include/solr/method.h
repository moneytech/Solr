#ifndef SOLR_METHOD_H
#define SOLR_METHOD_H

#include "common.h"
#include "opcode.h"

HEADER_BEGIN

#define SOLR_METHOD_PRIMATIVE (1 << 1)
#define SOLR_METHOD_COMPOUND (1 << 2)

#define SOLR_METHOD_IS_PRIMATIVE(m) ((m)->flags & SOLR_METHOD_PRIMATIVE)
#define SOLR_METHOD_IS_COMPOUND(m) ((m)->flas & SOLR_METHOD_COMPOUND)

struct solr_method{
    solr_class* class;
    solr_symbol* name;
    int flags;
    union{
        solr_func pfunc;
        struct{
            solr_opcode* code;
        } cfunc;
    } as;
};

solr_method* solr_method_primative_new(solr_class* container, char* name, solr_func func);
solr_method* solr_method_compound_new(solr_class* container, char* name, solr_opcode* code);

HEADER_END

#endif