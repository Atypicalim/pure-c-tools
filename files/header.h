// pure c tools

#ifndef H_PCT_PURE_C_TOOLS
#define H_PCT_PURE_C_TOOLS



#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>

#include <math.h>
#include <time.h>
#include <unistd.h>
#include <setjmp.h>
#include <sys/stat.h>

// object types
#define PCT_OBJ_OBJECT 'O'
#define PCT_OBJ_STRING 'S'
#define PCT_OBJ_ARRAY 'A'
#define PCT_OBJ_CURSOR 'C'
#define PCT_OBJ_STACK 'S'
#define PCT_OBJ_QUEUE 'Q'
#define PCT_OBJ_HASHKEY 'h'
#define PCT_OBJ_HASHMAP 'H'
#define PCT_OBJ_FOLIAGE 'F'
#define PCT_OBJ_BLOCK 'B'

void *pct_mallloc(size_t size)
{
    return malloc(size);
}

void *pct_realloc(void *object, size_t size)
{
    return realloc(object, size);
}

void pct_free(void *object)
{
    free(object);
}

#endif
