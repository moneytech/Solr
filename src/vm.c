#include <solr/vm.h>
#include <solr/class.h>
#include <stdlib.h>
#include <stdio.h>
#include <solr/gc.h>

typedef struct solr_classpool_entry{
    struct solr_classpool_entry* next;
    solr_class* class;
} solr_classpool_entry;

struct solr_classpool{
    solr_classpool_entry* root;
    long size;
};

solr_vm*
solr_vm_new(){
    solr_vm* vm = malloc(sizeof(solr_vm));

    vm->gc = malloc(sizeof(solr_gc));
    vm->gc->gc_free_chunk = 0;
    vm->gc->gc_refs_count = 0;

    vm->classpool = malloc(sizeof(solr_classpool));
    vm->classpool->root = NULL;
    vm->classpool->size = 0;

    vm->class_object = solr_define_class(vm, "Object", NULL, 1);
    solr_class_init(vm, vm->class_object);
    solr_gc_add_single_ref(vm->gc, &vm->class_object);

    vm->class_string = solr_define_class(vm, "String", vm->class_object, 0);
    solr_class_init(vm, vm->class_string);
    solr_gc_add_single_ref(vm->gc, &vm->class_string);

    vm->class_class = solr_define_class(vm, "Class", vm->class_object, 0);
    solr_class_init(vm, vm->class_class);
    solr_gc_add_single_ref(vm->gc, &vm->class_class);

    vm->class_fixnum = solr_define_class(vm, "Fixnum", vm->class_object, 0);
    solr_class_init(vm, vm->class_fixnum);
    solr_gc_add_single_ref(vm->gc, &vm->class_fixnum);

    vm->class_flonum = solr_define_class(vm, "Flonum", vm->class_object, 0);
    solr_class_init(vm, vm->class_flonum);
    solr_gc_add_single_ref(vm->gc, &vm->class_flonum);

    vm->class_char = solr_define_class(vm, "Character", vm->class_object, 0);
    solr_class_init(vm, vm->class_char);
    solr_gc_add_single_ref(vm->gc, &vm->class_char);
    return vm;
}

static inline int
sym_equal(solr_symbol* sym1, solr_symbol* sym2){
    return sym1->value == sym2->value ? strcmp(sym1->name, sym2->name) == 0 : 0;
}

int
solr_vm_has_class(solr_vm* vm, solr_symbol* sym){
    solr_classpool_entry* entry = vm->classpool->root;
    while(entry){
        if(sym_equal(entry->class->name, sym)){
            return 1;
        }

        entry = entry->next;
    }

    return 0;
}

void
solr_vm_define_class(solr_vm* vm, solr_class* class){
    solr_classpool_entry* entry = vm->classpool->root;
    while(entry){
        if(sym_equal(entry->class->name, class->name)){
            printf("Class '%s' already defined\n", class->name->name);
            abort();
        }

        entry = entry->next;
    }

    entry = malloc(sizeof(solr_classpool_entry));
    entry->class = class;
    entry->next = vm->classpool->root;
    vm->classpool->root = entry;
}

solr_class*
solr_vm_lookup_class(solr_vm* vm, char* name){
    solr_symbol* sym = solr_symbol_new(name);
    solr_classpool_entry* entry = vm->classpool->root;
    while(entry){
        if(sym_equal(entry->class->name, sym)){
            free(sym->name);
            free(sym);
            return entry->class;
        }

        entry = entry->next;
    }

    printf("Couldn't find class: '%s'\n", name);
    return NULL;
}