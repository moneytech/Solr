#ifndef SOLR_COMMON_H
#define SOLR_COMMON_H

#ifdef __cplusplus
#define HEADER_BEGIN extern "C" {
#edfine HEADER_END };
#else
#define HEADER_BEGIN
#define HEADER_END
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>

HEADER_BEGIN

/*
 * ------------ Murmur Hash ----------------
 *
 * Murmur-Hash for symbols preferably use:
 *  uint32_t murmur_str(char*);
 *
 * ========================================
 */
uint32_t murmur(char* data, size_t len);

static inline uint32_t
murmur_str(char* data){
    return murmur(data, strlen(data));
}
// =========================================


/*
 * ------------ Common Defs ----------------
 *
 * Common Definitions of types.
 *
 * =========================================
 */
typedef struct solr_gc solr_gc;
typedef struct solr_vm solr_vm;
typedef struct solr_object solr_object;
typedef struct solr_class solr_class;
typedef struct solr_field solr_field;
typedef struct solr_method solr_method;
typedef struct solr_native solr_native;
typedef unsigned char solr_byte;

typedef struct solr_symbol{
    char* name;
    uint32_t value;
} solr_symbol;

solr_symbol* solr_symbol_new(char* sym);
int solr_symbol_equals(solr_symbol* self, char* name);
int solr_symbol_equal(solr_symbol* self, solr_symbol* other);

#define FIXNUM_SIZE (sizeof(long))
#define FLONUM_SIZE (sizeof(double))
#define CHAR_SIZE (sizeof(char))

HEADER_END

#endif