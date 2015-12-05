#include <solr/class.h>
#include <solr/vm.h>
#include <stdlib.h>
#include <solr/object.h>
#include <solr/function.h>

typedef struct{
    unsigned int fields_count;
    solr_field** fields;
} field_bucket;

static void bucket_order_fields(field_bucket* bucket, unsigned int size, unsigned int* offset){
    unsigned int tmp = *offset;
    for(unsigned int i = 0; i < bucket->fields_count; i++){
        solr_field* field = bucket->fields[i];
        field->offset = tmp;
        tmp += size;
    }
    *offset = tmp;
}

#define ALIGN(obj, size) ((unsigned int)(((obj)+size)&~size))

static void
buckets_order_fields(field_bucket buckets[], unsigned int *size) {
    unsigned int offset = *size;
    offset = ALIGN(offset, 8);
    bucket_order_fields(&buckets[VT_FIXNUM], FIXNUM_SIZE, &offset);
    bucket_order_fields(&buckets[VT_FLONUM], FLONUM_SIZE, &offset);
    bucket_order_fields(&buckets[VT_CHAR], CHAR_SIZE, &offset);
    *size = offset;
}

solr_class*
solr_define_class(solr_vm* vm, char* name, solr_class* super, int fields_count){
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
    class->fields = malloc(sizeof(solr_field) * fields_count);
    if(super){
        class->object_size = super->object_size;
    }
    solr_vm_define_class(vm, class);
    return class;
}

void
solr_class_init(solr_vm* vm, solr_class* class){
    field_bucket buckets[VT_MAX];
    for(unsigned int i = 0; i < VT_MAX; i++){
        buckets[i].fields_count = 0;
    }
    for(uint16_t i = 0; i < class->fields_count; i++){
        solr_field* field = &class->fields[i];
        field_bucket* bucket = &buckets[field->vtype];
        bucket->fields_count++;
    }
    for(unsigned int i = 0; i < VT_MAX; i++){
        field_bucket* bucket = &buckets[i];
        bucket->fields = solr_vm_gc_alloc(vm, (int) (bucket->fields_count * sizeof(*bucket->fields)));
        bucket->fields_count = 0;
    }
    for(uint16_t i = 0; i < class->fields_count; i++){
        solr_field* field = &class->fields[i];
        field_bucket* bucket = &buckets[field->vtype];
        bucket->fields[bucket->fields_count++] = field;
    }
    buckets_order_fields(buckets, &class->object_size);
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

void
solr_class_define_field(solr_class* class, char* name, solr_field_vtype vtype){
    solr_field field;
    field.class = class;
    field.offset = 0;
    field.name = solr_symbol_new(name);
    field.vtype = vtype;
    class->fields[class->fields_count++] = field;
}

solr_field*
solr_class_get_field(solr_class* class, char* name){
    for(int i = 0; i < class->fields_count; i++){
        if(solr_symbol_equals(class->fields[i].name, name)){
            return &class->fields[i];
        }
    }
    return NULL;
}

solr_field*
solr_class_lookup_field(solr_class* self, char* name){
    solr_class* class = self;
    solr_field* field = NULL;
    while(class){
        if((field = solr_class_get_field(self, name)) != NULL){
            break;
        }

        class = class->super;
    }
    return field;
}