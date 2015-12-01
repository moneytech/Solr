#include <solr/vm.h>
#include <solr/class.h>
#include <stdio.h>

int main(int argc, char** argv){
    solr_vm* vm = solr_vm_new();
    solr_class* new_class = solr_define_class(vm, "Test", vm->class_object);
    if(solr_class_assignable(new_class, vm->class_object)){
        printf("Success\n");
    } else{
        printf("Failure\n");
    }
    return 0;
}