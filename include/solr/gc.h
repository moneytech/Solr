#ifndef SOLR_GC_H
#define SOLR_GC_H

#include "common.h"

HEADER_BEGIN

#define GC_MAX_REFS 65536
#define GC_MINCHUNK_SIZE 256
#define GC_MINHEAP_SIZE (1024 * 64)
#define GC_MAJHEAP_SIZE (1024 * 1024 * 32)
#define GC_MINCHUNKS (GC_MINHEAP_SIZE / GC_MINCHUNK_SIZE)
#define WITH_HEADER(size) ((size) + sizeof(int))
#define WITHOUT_HEADER(size) ((size) - sizeof(int))
#define FLAGS(chunk) (*((unsigned int*)(chunk)))
#define CHUNK_SIZE(chunk) (FLAGS((chunk)) & (~3))
#define MARK_CHUNK(chunk, c) ((FLAGS(chunk)) = CHUNK_SIZE(chunk)|c)
#define PTR_INDEX(ofs) ((ofs) / sizeof(void*))
#define BITS(chunk) (WITH_HEADER(chunk))
#define BITS_AT(chunk, idx) ((((void**)(BITS((chunk)) + (idx) * sizeof(void*)))))
#define MEM_TAG(ptr) (!(((unsigned int)(ptr))&1))
#define POINTS_MINOR(gc, ptr) (((solr_byte*)(ptr)) >= &(gc)->gc_minor_heap[0] && ((solr_byte*)(ptr)) < &(gc)->gc_minor_heap[GC_MINHEAP_SIZE])
#define POINTS_MAJOR(gc, ptr) (((solr_byte*)(ptr)) >= &gc->gc_major_heap[0] && ((solr_byte*)(ptr)) < &gc->gc_major_heap[GC_MAJHEAP_SIZE])
#define REF_PTR_MIN(gc, ptr) (MEM_TAG((ptr)) && POINTS_MINOR(gc, (ptr)))
#define REF_PTR_MAJ(gc, ptr) (MEM_TAG((ptr)) && POINTS_MAJOR(gc, (ptr)))
#define MINOR_CHUNK(gc, ptr) (((((solr_byte*)(ptr) - &(gc)->gc_minor_heap[0]) / GC_MINCHUNK_SIZE) * GC_MINCHUNK_SIZE) + &(gc)->gc_minor_heap[0])
#define MARKED(chunk) (FLAGS(chunk)&1)
#define CHUNK_FLAGS(chunk) (FLAGS(chunk)&(3))
#define ALIGN(ptr) (((ptr)+3)&~3)
#define CHUNK_AT(gc, i) (&gc->gc_minor_heap[(i) * GC_MINCHUNK_SIZE])
#define CHUNK_OFFSET(gc, chunk) ((((chunk)) - &gc->gc_minor_heap[0]) / GC_MINCHUNK_SIZE)

typedef enum{
    GC_FREE,
    GC_WHITE,
    GC_GRAY,
    GC_BLACK
} solr_gc_color;

struct solr_gc{
    solr_byte gc_minor_heap[GC_MINHEAP_SIZE];
    solr_byte gc_major_heap[GC_MAJHEAP_SIZE];
    int gc_free_chunk;
    int gc_refs_count;
    void* gc_backpatch[GC_MINCHUNKS];
    void** gc_refs[GC_MAX_REFS][2];
};

void solr_gc_add_ref(solr_gc* gc, void** begin, void** end);
void solr_gc_remove_ref(solr_gc* gc, void* begin, void* end);

static inline void
solr_gc_add_single_ref(solr_gc* gc, void* ref){
    solr_gc_add_ref(gc, ref, ref + sizeof(void*));
}

static inline void
solr_gc_remove_single_ref(solr_gc* gc, void* ref){
    solr_gc_remove_ref(gc, ref, ref + sizeof(void*));
}

HEADER_END

#endif