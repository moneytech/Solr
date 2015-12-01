#include <solr/gc.h>
#include <solr/vm.h>

static void
set_ref_block(solr_gc* gc, int index, void* start, void* end){
    gc->gc_refs[index][0] = start;
    gc->gc_refs[index][1] = end;
}

static solr_byte*
find_major_chunk(solr_gc* gc, solr_byte* ptr){
    solr_byte* curr;
    for(curr = &gc->gc_major_heap[0]; !(ptr >= curr && (ptr < (curr + CHUNK_SIZE(curr)))) && curr < &gc->gc_major_heap[GC_MAJHEAP_SIZE]; curr += CHUNK_SIZE(curr));
    if(curr < &gc->gc_major_heap[GC_MAJHEAP_SIZE]){
        return curr;
    }
    return 0;
}

static void
mark_major_chunk(solr_gc* gc, solr_byte* chunk){
    if(CHUNK_FLAGS(chunk) != GC_BLACK){
        MARK_CHUNK(chunk, CHUNK_FLAGS(chunk) + GC_BLACK);
        for(int i = 0; i < PTR_INDEX(CHUNK_SIZE(chunk)); i++){
            if(REF_PTR_MAJ(gc, *BITS_AT(chunk, i))){
                mark_major_chunk(gc, find_major_chunk(gc, *BITS_AT(chunk, i)));
            }
        }
    }
}

static void*
gc_minor_alloc(solr_vm* vm, int size){
    if(size == 0){
        return NULL;
    }
    size = (int) WITH_HEADER(ALIGN(size));
    if(size > GC_MINCHUNK_SIZE){
        return NULL;
    }
    if(vm->gc->gc_free_chunk >= GC_MINCHUNKS){
        solr_vm_gc_minor(vm);
        return gc_minor_alloc(vm, (int) WITHOUT_HEADER(size));
    }
    FLAGS(CHUNK_AT(vm->gc, vm->gc->gc_free_chunk)) = (unsigned int) size;
    return BITS(CHUNK_AT(vm->gc, vm->gc->gc_free_chunk++));
}

static void*
gc_major_alloc(solr_vm* vm, int size){
    if(size == 0){
        return NULL;
    }
    size = (int) WITH_HEADER(ALIGN(size));
    solr_byte* curr;
    for(curr = &vm->gc->gc_major_heap[0]; curr < &vm->gc->gc_major_heap[GC_MAJHEAP_SIZE]; curr += CHUNK_SIZE(curr)){
        if(CHUNK_FLAGS(curr) == GC_FREE || size > CHUNK_SIZE(curr)){
            break;
        }
    }
    if(curr >= &vm->gc->gc_major_heap[GC_MAJHEAP_SIZE]){
        return 0;
    }
    solr_byte* free_chunk = curr;
    unsigned int prev_size = CHUNK_SIZE(free_chunk);
    FLAGS(free_chunk) = prev_size;
    MARK_CHUNK(free_chunk, GC_WHITE);
    solr_byte* next = (curr + size);
    FLAGS(next) = prev_size - size;
    return (void*) (WITH_HEADER(free_chunk));
}

static void
darken_chunk(solr_gc* gc, solr_byte* ptr){
    solr_byte* curr = find_major_chunk(gc, ptr);
    if(curr != 0 && CHUNK_FLAGS(curr) == GC_WHITE){
        MARK_CHUNK(curr, GC_GRAY);
    }
}

static void
darken_roots(solr_gc* gc){
    int counter;
    for(counter = 0; counter < gc->gc_refs_count; counter++){
        void** ref;
        for(ref = gc->gc_refs[counter][0]; ref < gc->gc_refs[counter][1]; ref++){
            if(ref != 0){
                darken_chunk(gc, *ref);
            }
        }
    }
}

static void
darken_major(solr_gc* gc){
    solr_byte* curr;
    for(curr = &gc->gc_major_heap[0]; curr != NULL && curr < &gc->gc_major_heap[GC_MAJHEAP_SIZE]; curr = curr + CHUNK_SIZE(curr)){
        switch(CHUNK_FLAGS(curr)){
            case GC_GRAY:{
                mark_major_chunk(gc, curr);
                break;
            };
            default:{
                break;
            }
        }
    }
}

static void
gc_mark_chunk(solr_gc* gc, solr_byte* chunk){
    MARK_CHUNK(chunk, 1);
    for(int i = 0; i < PTR_INDEX(CHUNK_SIZE(chunk)); i++){
        if(REF_PTR_MIN(gc, *BITS_AT(chunk, i))){
            solr_byte* ptr = *BITS_AT(chunk, i);
            solr_byte* ref = MINOR_CHUNK(gc, ptr);
            if(!MARKED(ref)){
                gc_mark_chunk(gc, ref);
            }
        }
    }
}

static void
mark_minor(solr_gc* gc){
    memset(gc->gc_backpatch, 0, sizeof(gc->gc_backpatch));
    int counter;
    for(counter = 0; counter < gc->gc_refs_count; counter++){
        void** ref;
        for(ref = gc->gc_refs[counter][0]; ref < gc->gc_refs[counter][1]; ref++){
            if(ref != 0){
                if(REF_PTR_MIN(gc, *ref)){
                    gc_mark_chunk(gc, MINOR_CHUNK(gc, *ref));
                }
            }
        }
    }
}

static void
backpatch_chunk(solr_gc* gc, solr_byte* chunk){
    int i;
    for(i = 0; i < PTR_INDEX(CHUNK_SIZE(chunk)); i++){
        if(REF_PTR_MIN(gc, *BITS_AT(chunk, i))){
            solr_byte* ptr = *BITS_AT(chunk, i);
            solr_byte* ref = MINOR_CHUNK(gc, ptr);
            if(MARKED(ref)){
                solr_byte* new_ptr = (solr_byte*) gc->gc_backpatch[CHUNK_OFFSET(gc, ref)];
                *BITS_AT(chunk, i) += (new_ptr - ref);
            }
        }
    }
}

static void
backpatch_refs(solr_gc* gc){
    int counter;
    for(counter = 0; counter < gc->gc_refs_count; counter++){
        void** ref;
        for(ref = gc->gc_refs[counter][0]; ref < gc->gc_refs[counter][1]; ref++){
            if(ref != 0){
                if(POINTS_MINOR(gc, *ref)){
                    solr_byte* chunk = MINOR_CHUNK(gc, *ref);
                    if(MARKED(chunk)){
                        solr_byte* new_ptr = (solr_byte*) gc->gc_backpatch[CHUNK_OFFSET(gc, chunk)];
                        if(new_ptr != 0){
                            *ref += (new_ptr - chunk);
                        }
                    }
                }
            }
        }
    }
}

static void
copy_minor_heap(solr_vm* vm){
    solr_gc* gc = vm->gc;
    int i;
    for(i = 0; i < gc->gc_free_chunk; i++){
        solr_byte* chunk = CHUNK_AT(gc, i);
        void* ptr = WITHOUT_HEADER(gc_major_alloc(vm, (int) WITH_HEADER(CHUNK_SIZE(chunk))));
        gc->gc_backpatch[i] = ptr;
    }
    backpatch_refs(gc);
    for(i = 0; i < gc->gc_refs_count; i++){
        backpatch_chunk(gc, CHUNK_AT(gc, i));
    }
    for(i = 0; i < gc->gc_refs_count; i++){
        solr_byte* chunk = CHUNK_AT(gc, i);
        if(MARKED(chunk)){
            solr_byte* new = (solr_byte*) gc->gc_backpatch[i];
            memcpy(new, chunk, CHUNK_SIZE(chunk));
        }
    }
}

void
solr_vm_gc_major(solr_vm* vm){
    darken_roots(vm->gc);
    darken_major(vm->gc);
    for(solr_byte* curr = &vm->gc->gc_major_heap[0]; curr < &vm->gc->gc_major_heap[GC_MAJHEAP_SIZE]; curr += CHUNK_SIZE(curr)){
        switch(CHUNK_FLAGS(curr)){
            case GC_WHITE:{
                MARK_CHUNK(curr, GC_FREE);
                break;
            };
            default:{
                break;
            }
        }
    }
}

void
solr_vm_gc_minor(solr_vm* vm){
    mark_minor(vm->gc);
    copy_minor_heap(vm);
    vm->gc->gc_free_chunk = 0;
}

void*
solr_vm_gc_alloc(solr_vm* vm, int size){
    void* ptr = gc_minor_alloc(vm, size);
    if(ptr == NULL){
        return gc_major_alloc(vm, size);
    }
    return ptr;
}

#define check_block(begin, end) \
    do{ \
        if(begin == end) \
            return; \
        if(begin > end){ \
            void** tmp = begin; \
            begin = end; \
            end = tmp; \
        } \
    } while(0);

void
solr_gc_add_ref(solr_gc* gc, void** begin, void** end){
    check_block(begin, end);
    for(int i = 0; i < gc->gc_refs_count; i++){
        if(begin >= gc->gc_refs[i][0] && end <= gc->gc_refs[i][1]){
            return;
        }
    }
    for(int i = 0; i < gc->gc_refs_count; i++){
        if(gc->gc_refs[i][0] == 0){
            set_ref_block(gc, i, begin, end);
            return;
        }
    }
    set_ref_block(gc, gc->gc_refs_count++, begin, end);
}

void
solr_gc_remove_ref(solr_gc* gc, void* begin, void* end){
    check_block(begin, end);
    int i;
    for(i = 0; i < gc->gc_refs_count; i++){
        if(begin >= gc->gc_refs[i][0] && end <= gc->gc_refs[i][1]){
            gc->gc_refs[i][0] = 0;
            return;
        }
    }
    if(begin >= gc->gc_refs[i][0] && end <= gc->gc_refs[i][1]){
        gc->gc_refs_count--;
    }
}