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
void pct_object_free(void *_this)
{
    if (_this == NULL) tools_error("null pointer to object free");
    Object *this = _this;
    int type = this->objType;
    if (type == PCT_OBJ_OBJECT) return Object_free(this);
    if (type == PCT_OBJ_STRING) return String_free(this);
    if (type == PCT_OBJ_ARRAY) return Array_free(this);
    if (type == PCT_OBJ_CURSOR) return Cursor_free(this);
    if (type == PCT_OBJ_STACK) return Stack_free(this);
    if (type == PCT_OBJ_QUEUE) return Queue_free(this);
    if (type == PCT_OBJ_HASHKEY) return Hashkey_free(this);
    if (type == PCT_OBJ_HASHMAP) return Hashmap_free(this);
    if (type == PCT_OBJ_FOLIAGE) return Foliage_free(this);
    if (type == PCT_OBJ_BLOCK) return Block_free(this);
    Object_free(this);
}

void pct_object_print(void *_this)
{
    if (_this == NULL) tools_error("null pointer to object print");
    Object *this = _this;
    int type = this->objType;
    int count = this->referenceCount;
    printf("<Object t:%c c:%i p:%p>\n", type, count, this);
}

#endif
