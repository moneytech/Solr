#include <solr/method.h>
#include <stdlib.h>

solr_method*
solr_method_primative_new(solr_class* container, char* name, solr_func func){
    solr_method* method = malloc(sizeof(solr_method));
    method->name = solr_symbol_new(name);
    method->class = container;
    method->flags = SOLR_METHOD_PRIMATIVE;
    method->as.pfunc = func;
    return method;
}

solr_method*
solr_method_compound_new(solr_class* container, char* name, solr_opcode* code){
    solr_method* method = malloc(sizeof(solr_method));
    method->name = solr_symbol_new(name);
    method->class = container;
    method->flags = SOLR_METHOD_COMPOUND;
    method->as.cfunc.code = code;
    return method;
}