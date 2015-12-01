#include <solr/class.h>
#include <solr/vm.h>
#include <stdlib.h>

solr_class*
solr_define_class(solr_vm* vm, char* name, solr_class* super){
    solr_symbol* sym = solr_symbol_new(name);
    if(solr_vm_has_class(vm, sym)){
        free(sym->name);
        free(sym);
        return solr_vm_lookup_class(vm, name);
    }
    solr_class* class = malloc(sizeof(solr_class));
    class->super = super;
    class->name = sym;
    solr_vm_define_class(vm, class);
    return class;
}