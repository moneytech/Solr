#include <solr/function.h>
#include <solr/vm.h>
#include <solr/class.h>

struct solr_method{
    solr_symbol* name;
    void* ptr;
};

static void*
lookup_method(solr_class* class, char* method_name){
    for(int i = 0; i < class->methods_count; i++){
        solr_method* method = class->methods + i;
        if(solr_symbol_equals(method->name, method_name)){
            return method->ptr;
        }
    }
    return NULL;
}

void*
solr_lookup_native(solr_vm* vm, char* class_name, char* method_name){
    solr_class* class = solr_vm_lookup_class(vm, class_name);
    if(!class){
        return NULL;
    }
    return lookup_method(class, method_name);
}

static int
insert_native(solr_class* class, char* name, void* ptr){
    class->methods_count++;
    unsigned long new_size = class->methods_count * sizeof(solr_method);
    class->methods = realloc(class->methods, new_size);
    if(!class->methods){
        return -1;
    }
    solr_method* method = class->methods + class->methods_count - 1;
    method->name = solr_symbol_new(name);
    method->ptr = ptr;
    return 0;
}

int
solr_register_native(solr_vm* vm, solr_native* native){
    solr_class* class = solr_vm_lookup_class(vm, native->class_name);
    if(!class){
        fprintf(stderr, "Class '%s' not found\n", native->class_name);
        abort();
    }
    return insert_native(class, native->meth_name, native->ptr);
}

solr_native*
solr_native_new(char* class, char* meth, void* ptr){
    solr_native* native = malloc(sizeof(solr_native));
    native->meth_name = strdup(meth);
    native->class_name = strdup(class);
    native->ptr = ptr;
    return native;
}

void
solr_native_free(solr_native* self){
    if(self){
        free(self->class_name);
        free(self->meth_name);
        free(self);
    }
}