#include <solr/common.h>
#include <stdlib.h>

solr_symbol*
solr_symbol_new(char* data){
    solr_symbol* sym = malloc(sizeof(solr_symbol));
    sym->name = strdup(data);
    sym->value = murmur_str(data);
    return sym;
}

int
solr_symbol_equals(solr_symbol* self, char* name){
    uint32_t hash = murmur_str(name);
    if(self->value == hash){
        return strcmp(self->name, name);
    }

    return 0;
}