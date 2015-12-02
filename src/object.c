#include <solr/object.h>
#include <solr/class.h>

solr_object*
solr_object_new(solr_vm* vm, solr_class* class){
    solr_object* res = solr_vm_gc_alloc(vm, (int) (sizeof(solr_object) + class->object_size));
    if(!res){
        printf("Out of memory\n");
        abort();
    }
    res->class = class;
    return res;
}