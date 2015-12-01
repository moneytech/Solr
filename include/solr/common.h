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
 *  uint32_t murmur_str(char*, size_t);
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
typedef struct solr_vm solr_vm;
typedef struct solr_object solr_object;
typedef struct solr_class solr_class;
typedef struct solr_field solr_field;
typedef struct solr_method solr_method;
typedef unsigned char solr_byte;
typedef solr_object* (*solr_func)(solr_vm* vm, solr_object* self);

typedef struct solr_symbol{
    char* name;
    uint32_t value;
} solr_symbol;

solr_symbol* solr_symbol_new(char* sym);
int solr_symbol_equals(solr_symbol* self, char* name);

HEADER_END

#endif