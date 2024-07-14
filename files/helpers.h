// helpers

#ifndef H_PCT_HELPERS
#define H_PCT_HELPERS

#include "header.h"  // [M[ IGNORE ]M]

void pct_print_some_object()
{
    printf("test...\n");
}

// Object_initByType
// Object_printByType
// Object_freeByType
void pct_object_free_by_type(char type, void *object)
{
    if (type == PCT_OBJ_OBJECT) return Object_free(object);
    if (type == PCT_OBJ_STRING) return String_free(object);
    if (type == PCT_OBJ_ARRAY) return Array_free(object);
    if (type == PCT_OBJ_CURSOR) return Cursor_free(object);
    if (type == PCT_OBJ_STACK) return Stack_free(object);
    if (type == PCT_OBJ_QUEUE) return Queue_free(object);
    if (type == PCT_OBJ_HASHKEY) return Hashkey_free(object);
    if (type == PCT_OBJ_HASHMAP) return Hashmap_free(object);
    if (type == PCT_OBJ_FOLIAGE) return Foliage_free(object);
    if (type == PCT_OBJ_BLOCK) return Block_free(object);
    Object_free(object);
}

#endif
