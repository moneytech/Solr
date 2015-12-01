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
    solr_class* class = solr_vm_gc_alloc(vm, sizeof(solr_class));
    class->super = super;
    class->name = sym;
    class->prototypes_count = 0;
    solr_vm_define_class(vm, class);
    return class;
}

int
solr_class_assignable(solr_class* self, solr_class* other){
    solr_class* class = self;
    while(class){
        if(solr_symbol_equal(class->name, other->name)){
            return 1;
        }

        for(int i = 0; i < class->prototypes_count; i++){
            if(class->prototypes[i]){
                if(solr_symbol_equal(class->prototypes[i]->name, other->name)){
                    return 1;
                }
            }
        }

        class = class->super;
    }

    return 0;
}