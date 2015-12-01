#include <solr/common.h>
#include <stdio.h>
#include <solr/class.h>
#include <solr/vm.h>
#include <solr/method.h>

solr_object*
solr_tostring(solr_vm* vm, solr_object* self){
    return NULL;
}

int main(int argc, char** argv){
    solr_vm* vm = solr_vm_new();
    solr_method* tostring = solr_method_primative_new(vm->class_object, "tostring", &solr_tostring);
    printf("Is Primative: %d\n", SOLR_METHOD_IS_PRIMATIVE(tostring));
    return 0;
}