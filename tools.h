
// ./files/header.h 2024-07-14 15:03:00

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


// ./files/tools.h 2024-07-14 15:03:00

// tools

#ifndef H_PCT_TOOLS
#define H_PCT_TOOLS


// os type
#define PLATFORM_WINDOWS "PLATFORM_WINDOWS"
#define PLATFORM_APPLE "PLATFORM_APPLE"
#define PLATFORM_LINUX "PLATFORM_LINUX"
#define PLATFORM_UNIX "PLATFORM_UNIX"
#define PLATFORM_FREEBSD "PLATFORM_FREEBSD"
#define PLATFORM_UNKNOWN "PLATFORM_UNKNOWN"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define PLATFORM_NAME PLATFORM_WINDOWS
    #define IS_WINDOWS
#elif __APPLE__
    #define PLATFORM_NAME PLATFORM_APPLE
    #define IS_APPLE
#elif __linux__ || __unix || __unix__
    #define PLATFORM_NAME PLATFORM_LINUX
    #define IS_LINUX
#else
    #define PLATFORM_NAME PLATFORM_UNKNOWN
#endif

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int pct_cstr_starts_with(const char *s, const char *test)
{
    return strncmp(s, test, strlen(test)) == 0;
}

/* return 0 for match, nonzero for no match */
int pct_cstr_ends_with(const char *s, const char *test)
{
    size_t slen = strlen(s);
    size_t tlen = strlen(test);
    if (tlen > slen) return 1;
    return strcmp(s + slen - tlen, test);
}

void pct_tools_error(const char* msg, ...) {
    va_list lst;
    va_start(lst, msg);
    printf("[%s] => ", "ERROR");
    vfprintf(stdout, msg, lst);
    printf("\n");
    va_end(lst);
    exit(1);
}

void pct_tools_assert(bool value, const char *msg, ...)
{
    if (value == true) return;
    va_list lst;
    va_start(lst, msg);
    printf("[%s] => ", "ASSERT");
    vfprintf(stdout, msg, lst);
    printf("\n");
    va_end(lst);
    exit(1);
}

void pct_tools_warning(const char* msg, ...) {
    va_list lst;
    va_start(lst, msg);
    printf("[%s] => ", "WARNING");
    vfprintf(stdout, msg, lst);
    printf("\n");
    va_end(lst);
}

char *tools_format(char *msg, ...)
{
    va_list lst;
    va_list lstCopy;
    va_start(lst, msg);
    va_copy(lstCopy, lst);
    int bufsz = vsnprintf(NULL, 0, msg, lst);
    char* t = malloc(bufsz + 1);
    vsnprintf(t, bufsz + 1, msg, lstCopy);
    va_end(lst);
    va_end(lstCopy);
    return t;
}

int char_to_int(char c)
{
    int i = c - '0';
    if (i >= 49) i-= 39;
    if (i < 0 || i > 15) return 0;
    return i;
}

char *arr_to_str(char arr[], int len)
{
    char* ptr = (char *)malloc(len * sizeof(char) + 1);
    for (int i = 0; i < len; i++)
    {
        ptr[i] = arr[i];
    }
    ptr[len] = '\0';
    return ptr;
}

int hex_to_int(char *str)
{
    int len = strlen(str);
    int result = 0;
    int number;
    for (int i = len - 1; i >= 0; i--)
    {
        number = char_to_int(str[len - i - 1]);
        result = result + number * pow(16, i);
    }
    return result;
}

int num_random(int from, int to)
{
    int big = to > from ? to : from;
    int small = to > from ? from : to;
    int count = big - small + 1;
    int num = rand() % count;
    int r = small + num;
    return r;
}

// limit as : 0 ~ size (-size ~ -1)
void limit_range(int size, bool swapXY, int *_from, int *_to)
{
    int from = *_from;
    int to = *_to;
    if (from < 0) from = MAX(0, size + from);
    if (to < 0) to = MAX(0, size + to);
    from = MIN(size - 1, MAX(0, from));
    to = MIN(size - 1, MAX(0, to));
    if (swapXY && from > to) {
        int tmp = from;
        from = to;
        to = tmp;
    }
    *_from = from;
    *_to = to; 
}

bool file_write(char *path, char *data)
{
    if (data == NULL) return false;
    FILE *fp = fopen(path, "ab");
    if (fp == NULL) return false;
    fputs(data, fp);
    fclose(fp);
    return true;
}

char *file_read(char *path)
{
    char *text;
    FILE *file = fopen(path, "rb");
    if (file == NULL) return NULL;
    fseek(file, 0, SEEK_END);
    long lSize = ftell(file);
    text = (char *)malloc(lSize + 1);
    rewind(file);
    fread(text, sizeof(char), lSize, file);
    text[lSize] = '\0';
    fclose(file);
    return text;
}

bool file_copy(char *path, char *to)
{
    int BUF_SIZE = 1024;
    FILE *src, *dst;
    size_t in, out;
    src = fopen(path, "rb");
    dst = fopen(to, "wb");
    if (src == NULL || dst == NULL) {
        if (src != NULL) return fclose(src);
        if (dst != NULL) return fclose(dst);
        return false;
    }
    char *buf = (char*) malloc(BUF_SIZE * sizeof(char));
    while (1) {
        in = fread(buf, sizeof(char), BUF_SIZE, src);
        if (0 == in) break;
        out = fwrite(buf, sizeof(char), in, dst);
        if (0 == out) break;
    }
    fclose(src);
    fclose(dst);
    free(buf);
    return true;
}

int file_rename(char *path, char *to)
{
    return rename(path, to);
}

int file_remove(char *path)
{
    return unlink(path);
}

bool file_exist(char *path)
{
    return access(path, F_OK) == 0;
}

int file_mkdir(const char* name)
{
    #ifdef __linux__
        return mkdir(name, 777); /* Or what parameter you need here ... */
    #else
        return mkdir(name);
    #endif
}

bool file_is_file(char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

bool file_is_directory(char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int file_create_directory(char *path)
{
    char tmp[1024];
    char *p = NULL;
    size_t len;
    snprintf(tmp, sizeof(tmp),"%s",path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/') tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
    {
        if (*p != '/') continue;
        *p = 0;
        file_mkdir(tmp);
        *p = '/';
    }
    return file_mkdir(tmp);
}

#endif


// ./files/object.h 2024-07-14 15:03:00


#ifndef H_PCT_UG_OBJECT
#define H_PCT_UG_OBJECT

void pct_object_free_by_type(char type, void *object);

typedef struct _Object {
    char objType;
    int referenceCount;
} Object;

void Object_init(void *_this, char _objType)
{
    Object *this = _this;
    this->objType = _objType;
    this->referenceCount = 1;
    #ifdef H_PCT_OBJECT_CALLBACKS
    Object_initByType(this->objType, this);
    #endif
}

Object *Object_new()
{
    Object *object = (Object *)pct_mallloc(sizeof(Object));
    Object_init(object, PCT_OBJ_OBJECT);
    return object;
}


void Object_free(void *_this)
{
    Object *this = _this;
    pct_free(this);
}

void Object_retain(void *_this)
{
    if (_this == NULL) pct_tools_error("null pointer to object retain");
    Object *this = _this;
    this->referenceCount++;
}

void Object_release(void *_this)
{
    if (_this == NULL) pct_tools_error("null pointer to object release");
    Object *this = _this;
    this->referenceCount--;
    if (this->referenceCount <= 0) {
        #ifdef H_PCT_OBJECT_CALLBACKS
        Object_freeByType(this->objType, this);
        #else
        pct_object_free_by_type(this->objType, this);
        #endif
    }
}

void Object_print(void *_this)
{
    if (_this == NULL) pct_tools_error("null pointer to object print");
    Object *this = _this;
    #ifdef H_PCT_OBJECT_CALLBACKS
    Object_printByType(this->objType, this);
    #else
    printf("<Object t:%c p:%d>\n", this->objType, this);
    #endif
}

#endif


// ./files/string.h 2024-07-14 15:03:00

// string

#ifndef H_PCT_UG_STRING
#define H_PCT_UG_STRING

#define STRING_MIN_CAPACITY 128

typedef struct _String {
    struct _Object;
    int length;
    int capacity;
    char *data;
} String;

String *String_new()
{
    String *string = (String *)pct_mallloc(sizeof(String));
    Object_init(string, PCT_OBJ_STRING);
    string->length = 0;
    string->capacity = STRING_MIN_CAPACITY + 1;
    string->data = pct_mallloc(string->capacity);
    string->data[string->length] = '\0';
    return string;
}

void String_free(String *this)
{
    pct_free(this->data);
    Object_free(this);
}

void _string_check_capacity(String *this, int length)
{
    if (this->capacity >= length + 1) return;
    while (this->capacity < length + 1 && this->capacity * 2 < INT_MAX) {
        this->capacity *= 2;
        if (this->capacity <= 0) this->capacity--;
    }
    this->data = pct_realloc(this->data, this->capacity);
}

String *String_appendChar(String *this, char c)
{
    _string_check_capacity(this, this->length + 1);
    this->data[this->length] = c;
    this->length++;
    this->data[this->length] = '\0';
    return this;
}

String *String_prependChar(String *this, char c)
{
    _string_check_capacity(this, this->length + 1);
    memmove(this->data + 1, this->data, this->length);
    this->data[0] = c;
    this->length++;
    this->data[this->length] = '\0';
    return this;
}

String *String_appendArr(String *this, char arr[])
{
    if (arr == NULL) return this;
    int len = strlen(arr);
    if (len == 0) return this;
    _string_check_capacity(this, this->length + len);
    memmove(this->data + this->length, arr, len);
    this->length += len;
    this->data[this->length] = '\0';
    return this;
}

String *String_prependArr(String *this, char arr[])
{
    if (arr == NULL) return this;
    int len = strlen(arr);
    if (len == 0) return this;
    memmove(this->data + len, this->data, this->length);
    memmove(this->data, arr, len);
    this->length += len;
    this->data[this->length] = '\0';
    return this;
}

String *String_appendStr(String *this, char *str)
{
    if (str == NULL || *str == '\0') return this;
    int len = strlen(str);
    if (len == 0) return this;
    _string_check_capacity(this, this->length + len);
    memmove(this->data + this->length, str, len);
    this->length += len;
    this->data[this->length] = '\0';
    return this;
}

String *String_prependStr(String *this, char *str)
{
    if (str == NULL || *str == '\0') return this;
    int len = strlen(str);
    if (len == 0) return this;
    _string_check_capacity(this, this->length + len);
    memmove(this->data + len, this->data, this->length);
    memmove(this->data, str, len);
    this->length += len;
    this->data[this->length] = '\0';
    return this;
}

String *String_append(String *this, char *str)
{
    return String_appendStr(this, str);
}

String *String_prepend(String *this, char *str)
{
    return String_prependStr(this, str);
}

String *String_insert(String *this, int at, String *that)
{
    if (at < 0 || at > this->length || that == NULL || that->length <= 0) return this;
    _string_check_capacity(this, this->length + that->length);
    char *temp = pct_mallloc(this->length - at);
    memmove(temp, this->data + at, this->length - at);
    memmove(this->data + at, that->data, that->length);
    memmove(this->data + at + that->length, temp, this->length - at);
    pct_free(temp);
    this->length += that->length;
    this->data[this->length] = '\0';
    return this;
}

String *String_delete(String *this, int from, int to)
{
    limit_range(this->length, true, &from, &to);
    int len = this->length;
    char *dst = this->data + from;
    char *src = this->data + to + 1;
    size_t cnt = this->length - to - 1;
    memmove(dst, src, cnt);
    this->length -= (to - from + 1);
    this->data[this->length] = '\0';
    return this;
}

String *String_deleteStarting(String *this, int to)
{
    return String_delete(this, 0, to);
}

String *String_deleteEnding(String *this, int from)
{
    return String_delete(this, from, this->length);
}

String *String_clear(String *this)
{
    this->length = 0;
    this->data[this->length] = '\0';
    return this;
}

unsigned long String_hash(String *this)
{
    char *str = this->data;
    unsigned long hash = 5381;
    int c;
    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

int String_findNext(String *this, int from, char *target)
{
    int len = strlen(target);
    if (this->length <= 0 || from < 0 || len <= 0) return -1;
    char *ptr = strstr(this->data + from, target);
    if (ptr == NULL) return -1;
    int pos = ptr - this->data;
    return pos;
}

int String_findLast(String *this, int to, char *target)
{
    int len = strlen(target);
    if (this->length <= 0 || to > this->length || len <= 0) return -1;
    int containIndex = to - len;
    int nextIndex = this->length - 1;
    int foundIndex = -1;
    char *ptr = NULL;
    while (nextIndex >= 0) {
        if ((ptr = strstr(this->data + nextIndex, target)) != NULL && (foundIndex = ptr - this->data) <= containIndex) return foundIndex;
        nextIndex--;
    }
    return -1;
}

// [N, index1, index2, ... indexN]
int *String_findAll(String *this, char *target)
{
    int size = 128;
    int *result = (int *)pct_mallloc(sizeof(int) * size);
    result[0] = 0;
    int foundIndex = -1;
    int nextIndex = 0;
    int lastRecorded = -1;
    while ((foundIndex = String_findNext(this, nextIndex, target)) >= 0) {
        nextIndex++;
        if (foundIndex != lastRecorded) {
            result[0]++;
            if (result[0] > size - 1) {
                size <<= 2;
                result = (int *)pct_realloc(result, size);
            }
            result[result[0]] = foundIndex;
            lastRecorded = foundIndex;
        }
    }
    return result;
}

int String_capacity(String *this)
{
    return this->capacity;
}

int String_length(String *this)
{
    return this->length;
}

char *String_get(String *this)
{
    return this->data;
}

String *String_set(String *this, char *str)
{
    String_delete(this, 0, -1);
    String_appendStr(this, str);
    return this;
}

void String_print(String *this)
{
    printf("<STRING:%d,%lu,%s>\n", this->length, String_hash(this), this->data);
}

char *String_dump(String *this)
{
    char *s = pct_mallloc(this->length + 1);
    memcpy(s, this->data, this->length + 1);
    return s;
}

String *String_clone(String *this)
{
    String *s = String_new();
    String_appendStr(s, String_get(this));
    return s;
}

String *String_format(char *template, ...)
{
    va_list lst;
    va_list lstCopy;
    va_start(lst, template);
    va_copy(lstCopy, lst);
    int bufsz = vsnprintf(NULL, 0, template, lst);
    char *t = pct_mallloc(bufsz + 1);
    vsnprintf(t, bufsz + 1, template, lstCopy);
    va_end(lst);
    va_end(lstCopy);
    String *s = String_new();
    String_appendStr(s, t);
    pct_free(t);
    return s;
}

String* String_repeat(String *this, int count)
{
    if (count <= 0 || this->length <= 0) return this;
    char *data = String_dump(this);
    while (--count > 0) {
        String_appendStr(this, data);
    }
    pct_free(data);
    return this;
}

String *String_subString(String *this, int from, int to)
{
    limit_range(this->length, true, &from, &to);
    String *s = String_new();
    int len = to - from + 1;
    char *temp = pct_mallloc(len + 1);
    memmove(temp, this->data + from, len);
    temp[len] = '\0';
    String_appendStr(s, temp);
    pct_free(temp);
    return s; 
}

int String_compare(String *this, String *that)
{
    return strcmp(this->data, that->data);
}

bool String_equal(String *this, String *that)
{
    return String_compare(this, that) == 0;
}

char String_getChar(String *this, int index)
{
    if (index < 0 || index >= this->length) return '\0';
    return this->data[index];
}

String *String_setChar(String *this, int index, char c)
{
    if (index < 0 || index >= this->length || c == '\0') return this;
    this->data[index] = c;
    return this;
}

String *String_reverse(String *this)
{
    int left = 0;
    int right = this->length - 1;
    char temp;
    while(left < right) {
        temp = this->data[left];
        this->data[left] = this->data[right];
        this->data[right] = temp;
        left++;
        right--;
    }
}

String *String_upper(String *this)
{
    int index = 0;
    while(index < this->length) this->data[index++] = toupper(this->data[index]);
}

String *String_lower(String *this)
{
    int index = 0;
    while(index < this->length) this->data[index++] = tolower(this->data[index]);
}

bool String_startsWith(String *this, char *target)
{
    return String_findNext(this, 0, target) == 0;
}

bool String_endsWith(String *this, char *target)
{
    return String_findLast(this, -1, target) == this->length - strlen(target);
}

bool String_contains(String *this, char *target)
{
    return String_findNext(this, 0, target) >= 0;
}

String *String_replace(String *this, char *target, char *relacement, int from, int to, int count)
{
    limit_range(this->length, true, &from, &to);
    int thisLen = String_length(this);
    int targetLen = strlen(target);
    int replacementLen = strlen(relacement);
    if (targetLen <= 0 || count == 0) return this;
    int fromIndex = from;
    int foundIndex = -1;
    int replaceCount = 0;
    String *result = String_new();
    String *temp = NULL;
    while((foundIndex = String_findNext(this, fromIndex, target)) >= 0) {
        if (foundIndex >= to) break;
        temp = String_subString(this, fromIndex, foundIndex - 1);
        String_append(result, String_get(temp));
        String_free(temp);
        String_append(result, relacement);
        fromIndex = foundIndex + targetLen;
        replaceCount++;
        if (count > 0 && replaceCount >= count) break;
    }
    temp = String_subString(this, fromIndex, this->length - 1);
    String_append(result, String_get(temp));
    String_free(temp);
    String_set(this, String_get(result));
    String_free(result);
    return this;
}

String *_string_trim_both(String *this, bool isLeft, bool isRight)
{
    if (this->length == 0) return this;
    if (isLeft) {
        int start = 0;
        while(start < this->length && isspace(this->data[start])) start++;
        if (start > 0)  String_deleteStarting(this, start);
    }
    if (isRight) {
        int end = this->length - 1;
        while(end >= 0 && isspace(this->data[end])) end--;
        if (end < this->length - 1) String_deleteEnding(this, end + 1);
    }
    return this;
}

String *String_trimLeft(String *this)
{
    return _string_trim_both(this, true, false);
}

String *String_trimRight(String *this)
{
    return _string_trim_both(this, false, true);
}

String *String_trim(String *this)
{
    return _string_trim_both(this, true, true);
}

#endif


// ./files/cursor.h 2024-07-14 15:03:00

// cursor

#ifndef H_PCT_CURSOR
#define H_PCT_CURSOR

typedef struct _Cursor {
    struct _Object;
    void *cursor;
} Cursor;

Cursor *Cursor_new(void *cursor)
{
    Cursor *queue = (Cursor *)pct_mallloc(sizeof(Cursor));
    Object_init(queue, PCT_OBJ_CURSOR);
    queue->cursor = cursor;
    return queue;
}

void Cursor_set(Cursor *this, void *cursor)
{
    this->cursor = cursor;
}

void *Cursor_get(Cursor *this)
{
    return this->cursor;
}

void Cursor_print(Cursor *this)
{
    printf("[(CURSOR) => p:%s, c:%s]\n", this, this->cursor);
}

void Cursor_free(Cursor *this)
{
    this->cursor = NULL;
    Object_free(this);
}

#endif


// ./files/hashkey.h 2024-07-14 15:03:00

// Hashkey

#ifndef H_PCT_HASHKEY
#define H_PCT_HASHKEY

typedef struct _Hashkey {
    struct _Object;
    String *key;
    void *value;
    struct _Hashkey *next;
} Hashkey;


Hashkey *Hashkey_set(Hashkey *this, void *value)
{
    this->value = value;
    return this;
}

Hashkey *Hashkey_new(String *key, void *value)
{
    Hashkey *hashkey = (Hashkey *)pct_mallloc(sizeof(Hashkey));
    hashkey->key = key;
    hashkey->value = value;
    hashkey->next = NULL;
    Object_retain(key);
    Object_init(hashkey, PCT_OBJ_HASHKEY);
    return hashkey;
}

void Hashkey_free(void *_this)
{
    Hashkey *this = _this;
    Object_release(this->key);
    this->key = NULL;
    this->value = NULL;
    this->next = NULL;
    Object_free(this);
}

#endif


// ./files/hashmap.h 2024-07-14 15:03:00

// hashmap

#ifndef H_PCT_HASHMAP
#define H_PCT_HASHMAP

#define HASHMAP_DEFAULT_CAPACITY 4097

typedef struct _Hashmap {
    struct _Object;
    int size;
    Hashkey *position;
} Hashmap;

Hashmap* Hashmap_new() {
    Hashmap *map = (Hashmap *)pct_mallloc(sizeof(Hashmap) * HASHMAP_DEFAULT_CAPACITY);
    map->size = HASHMAP_DEFAULT_CAPACITY;
    for (int i = 0; i < map->size; ++i ) map[i].position = NULL;
    Object_init(map, PCT_OBJ_HASHMAP);
    return map;
}

// TODO: release removed value
void Hashmap_free(Hashmap *this) {
    assert(this != NULL);
    Hashkey *ptr;
    Hashkey *tmp;
    for (int i = 0; i < HASHMAP_DEFAULT_CAPACITY; ++i) {
        ptr = this[i].position;
        while (ptr != NULL) {
            tmp = ptr;
            ptr = ptr->next;
            Object_release(tmp);
        }
    }
    Object_free(this);
}

void *Hashmap_set(Hashmap *this, char *_key, void *value) {
    assert(this != NULL);
    assert(_key != NULL);
    assert(value != NULL);
    String *key = String_format(_key);
    int pos = String_hash(key) % HASHMAP_DEFAULT_CAPACITY;
    //
    void *tmp = NULL;
    Hashkey *ptr = this[pos].position;
    if (ptr == NULL) {
        this[pos].position = Hashkey_new(key, value);
        Object_release(key);
        return NULL;
    }
    while (ptr != NULL) {
        if (String_equal(key, ptr->key)) {
            tmp = ptr->value;
            Hashkey_set(ptr, value);
            Object_release(key);
            // TODO: release tmp
            return tmp;
        }
        ptr = ptr->next;
    }
    Hashkey *pnode = Hashkey_new(key, value);
    Object_release(key);
    pnode->next = this[pos].position;
    this[pos].position = pnode;
    return NULL;
}

void *Hashmap_get(Hashmap *this, char *_key) {
    assert(this != NULL);
    assert(_key != NULL);
    String *key = String_format(_key);
    int pos = String_hash(key) % HASHMAP_DEFAULT_CAPACITY;
    //
    Hashkey *ptr = this[pos].position;
    while (ptr != NULL) {
        if (String_equal(key, ptr->key)) {
            Object_release(key);
            return ptr->value;
        }
        ptr = ptr->next;
    }
    Object_release(key);
    return NULL;
}

// TODO: release removed value
void *Hashmap_del(Hashmap *this, char *_key) {
    assert(this != NULL);
    assert(_key != NULL);
    String *key = String_format(_key);
    int pos = String_hash(key) % HASHMAP_DEFAULT_CAPACITY;
    //
    void *tmp = NULL;
    Hashkey *ptr = this[pos].position;
    if (ptr == NULL) {
        Object_release(key);
        return NULL;
    }
    Hashkey *pre = NULL;
    while (ptr != NULL) {
        if (String_equal(key, ptr->key)) {
            if (pre == NULL) {
                this[pos].position = NULL;
            } else {
                pre->next = ptr->next;
            }
            tmp = ptr->value;
            Object_release(ptr);
            break;
        }
        pre = ptr;
        ptr = ptr->next;
    }
    Object_release(key);
    return tmp;
}

char *Hashmap_toString(Hashmap *this)
{
    return tools_format("[Hashmap => p:%p s:%i]", this, this->size);
}

#endif


// ./files/foliage.h 2024-07-14 15:03:00

// token

#ifndef H_PCT_FOLIAGE
#define H_PCT_FOLIAGE

typedef struct _Foliage {
    struct _Object;
    void *data;
    void *left;
    void *right;
} Foliage;

void Foliage_init(void *_foliage, void *data)
{
    Foliage *foliage = _foliage;
    foliage->data = data;
    foliage->left = NULL;
    foliage->right = NULL;
}

Foliage *Foliage_new(void *data)
{
    Foliage *foliage = (Foliage *)pct_mallloc(sizeof(Foliage));
    Object_init(foliage, PCT_OBJ_FOLIAGE);
    Foliage_init(foliage, data);
    return foliage;
}

void Foliage_print(void *_this)
{
    if (_this == NULL)
    {
        printf("[(Foliage) => NULL]\n");
    }
    else
    {
        Foliage *this = _this;
        printf("[(Foliage) => address:%d, data:%d]\n", this, this->data);
    }
}

void Foliage_free(Foliage *this)
{
    this->left = NULL;
    this->right = NULL;
    if (this->data != NULL)
    {
        pct_free(this->data);
        this->data = NULL;
    }
    Object_free(this);
}

#endif


// ./files/block.h 2024-07-14 15:03:00

// token

#ifndef H_PCT_BLOCK
#define H_PCT_BLOCK

typedef struct _Block {
    struct _Object;
    void *data;
    void *next;
    void *last;
} Block;

void Block_init(void *_block, void *data)
{
    Block *block = _block;
    Object_retain(data);
    block->data = data;
    block->next = NULL;
    block->last = NULL;
}

Block *Block_new(void *data)
{
    Block *block = (Block *)pct_mallloc(sizeof(Block));
    Object_init(block, PCT_OBJ_BLOCK);
    Block_init(block, data);
    return block;
}

void Block_print(void *_this)
{
    if (_this == NULL)
    {
        printf("[(Block) => NULL]\n");
    }
    else
    {
        Block *this = _this;
        printf("[(Block) => address:%d, data:%d]\n", this, this->data);
    }
}

void Block_link(void *_first, void *_second)
{
    Block *first = _first;
    Block *second = _second;
    first->next = second;
    second->last = first;
}

void Block_append(void *_this, void *_other)
{
    Block_link(_this, _other);
}

void Block_prepend(void *_this, void *_other)
{
    Block_link(_other, _this);
}

void Block_remove(void *_this)
{
    Block *this = _this;
        Block *next = this->next;
        Block *last = this->last;
    if (next != NULL && last != NULL)
    {
        last->next = next;
        next->last = last;
    }
    else if (next != NULL)
    {
        next->last = NULL;
    }
    else if (last != NULL)
    {
        last->next = NULL;
    }
    this->next = NULL;
    this->last = NULL;
}

void *Block_next(void *_this)
{
    Block *this = _this;
    return this->next;
}

void *Block_last(void *_this)
{
    Block *this = _this;
    return this->last;
}

void Block_free(void *_this)
{
    Block *this = _this;
    Block *tmpNext = this->next;
    Block *tmpLast =this->last;
    if (tmpNext != NULL) tmpNext->last = NULL;
    if (tmpLast != NULL) tmpLast->next = NULL;
    this->next = NULL;
    this->last = NULL;
    tmpNext = NULL;
    tmpLast = NULL;
    if (this->data != NULL)
    {
        Object_release(this->data);
        this->data = NULL;
    }
    Object_free(this);
}

#endif


// ./files/queue.h 2024-07-14 15:03:00

// queue

#ifndef H_PCT_QUEUE
#define H_PCT_QUEUE


typedef struct _Queue {
    struct _Object;
    int size;
    Block *head;
    Block *tail;
    Block *cursor;
} Queue;

Queue *Queue_new()
{
    Queue *queue = (Queue *)pct_mallloc(sizeof(Queue));
    Object_init(queue, PCT_OBJ_QUEUE);
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    queue->cursor = NULL;
    return queue;
}

void Queue_print(Queue *this)
{
    printf("[(QUEUE_START) => address:%d]\n", this);
    Block *current = this->head;
    while (current != NULL)
    {
        Block_print(current);
        current = current->next;
    }
    printf("[(QUEUE_END) => address:%d]\n", this);
}

void Queue_push(Queue *this, void *data)
{
    this->size++;
    Block *block = Block_new(data);
    if (this->head == NULL)
    {
        this->head = block;
    }
    else
    {
        Block_append(this->tail, block);
    }
    this->tail = block;
}

void *Queue_pop(Queue *this)
{
    if (this->head == NULL)
    {
        this->size = 0;
        return NULL;
    }
    else
    {
        void *data = this->head->data;
        Block *head = this->head;
        if (this->head == this->tail)
        {
            this->size = 0;
            this->head = NULL;
            this->tail = NULL;
        }
        else
        {
            this->size--;
            this->head = this->head->next;
            this->head->last = NULL;
        }
        Object_release(head);
        return data;
    }
}

void Queue_clear(Queue *this)
{
    void *data = Queue_pop(this);
    while (data != NULL)
    {
        data = Queue_pop(this);
    }
}

void Queue_free(Queue *this)
{
    Block *head = this->head;
    while (head != NULL)
    {
        this->head = head->next;
        Object_release(head);
        head = this->head;
    }
    Object_free(this);
}

Cursor *Queue_reset(Queue *this)
{
    return Cursor_new(this->head);
}

void *Queue_next(Queue *this, Cursor *cursor)
{
    Block *temp = Cursor_get(cursor);
    if (temp == NULL) return NULL;
    Cursor_set(cursor, temp->next);
    return temp->data;
}

#endif


// ./files/stack.h 2024-07-14 15:03:00

// stack

#ifndef H_PCT_STACK
#define H_PCT_STACK


typedef struct _Stack {
    struct _Object;
    int size;
    Block *head;
    Block *tail;
    Block *cursor;
} Stack;

Stack *Stack_new()
{
    Stack *stack = (Stack *)pct_mallloc(sizeof(Stack));
    Object_init(stack, PCT_OBJ_STACK);
    stack->size = 0;
    stack->head = NULL;
    stack->tail = NULL;
    stack->cursor = NULL;
    return stack;
}

void Stack_print(Stack *this)
{
    printf("[(STACK_START) => address:%d]\n", this);
    Block *current = this->head;
    while (current != NULL)
    {
        Block_print(current);
        current = current->next;
    }
    printf("[(STACK_END) => address:%d]\n", this);
}

void Stack_push(Stack *this, void *data)
{
    this->size++;
    Block *block = Block_new(data);
    if (this->head == NULL)
    {
        this->head = block;
    }
    else
    {
        Block_append(this->tail, block);
    }
    this->tail = block;
}

void *Stack_pop(Stack *this)
{
    if (this->tail == NULL)
    {
        this->size = 0;
        return NULL;
    }
    else
    {
        void *data = this->tail->data;
        Block *tail = this->tail;
        if (this->tail == this->head)
        {
            this->size = 0;
            this->head = NULL;
            this->tail = NULL;
        }
        else
        {
            this->size--;
            this->tail = this->tail->last;
            this->tail->next = NULL;
        }
        Object_release(tail);
        return data;
    }
}

void Stack_clear(Stack *this)
{
    void *data = Stack_pop(this);
    while (data != NULL)
    {
        data = Stack_pop(this);
    }
}

void Stack_free(Stack *this)
{
    Block *tail = this->tail;
    while (tail != NULL)
    {
        this->tail = tail->last;
        Object_release(tail);
        tail = this->tail;
    }
    Object_free(this);
}

Cursor *Stack_reset(Stack *this)
{
    return Cursor_new(this->tail);
}

void *Stack_next(Stack *this, Cursor *cursor)
{
    Block *temp = Cursor_get(cursor);
    if (temp == NULL) return NULL;
    Cursor_set(cursor, temp->last);
    return temp->data;
}

void Stack_reverse(Stack *this)
{
    Queue *queue = Queue_new();
    //
    Cursor *cursor = Stack_reset(this);
    void *data1 = Stack_next(this, cursor);
    while (data1 != NULL)
    {
        Queue_push(queue, data1);
        data1 = Stack_next(this, cursor);
    }
    Cursor_free(cursor);
    Stack_clear(this);
    //
    cursor = Queue_reset(queue);
    void *data = Queue_next(queue, cursor);
    while (data != NULL)
    {
        Stack_push(this, data);
        data = Queue_next(queue, cursor);
    }
    Cursor_free(cursor);
    Object_release(queue);
}

#endif


// ./files/array.h 2024-07-14 15:03:00

// array

#ifndef H_PCT_ARRAY
#define H_PCT_ARRAY

#define ARRAY_DEFAULT_CAPACITY 1024
#define ARRAY_INVALID_INDEX 0

typedef int (* ArraySortFunction)(void const*, void const*);
typedef bool (* ArrayFindFunction)(void const*);

typedef struct _Array {
    struct _Object;
    void **elements;
    int length;
    int capacity;
    bool nullable;
} Array;

Array *Array_new()
{
    Array *array = (Array *)pct_mallloc(sizeof(Array));
    Object_init(array, PCT_OBJ_ARRAY);
    array->capacity = ARRAY_DEFAULT_CAPACITY;
    array->length = 0;
    array->nullable = false;
    array->elements = (void *)pct_mallloc(sizeof(void *) * array->capacity);
    for (int i = array->length; i < array->capacity; i++) array->elements[i] = NULL;
    return array;
}

void Array_clear(Array *this)
{
    for (int i = 0; i < this->length; i++) {
        Object_release(this->elements[i]);
        this->elements[i] = NULL;
    }
    this->length = 0;
}

void Array_free(Array *this)
{
    Array_clear(this);
    pct_free(this->elements);
    this->elements = NULL;
    Object_free(this);
}

bool _array_check_resize(Array *this, int length)
{
    if (length <= this->capacity) return true;
    int capacity = this->capacity;
    while (length > capacity) capacity = capacity + ARRAY_DEFAULT_CAPACITY;
    void **elements = pct_realloc(this->elements, sizeof(void *) * capacity);
    if (elements == NULL) return false;
    this->capacity = capacity;
    this->elements = elements;
    for (int i = this->length; i < this->capacity; i++) this->elements[i] = NULL;
    return true;
}

bool Array_set(Array *this, int index, void *element)
{
    if (!this->nullable && element == NULL) return false;
    if (index < 0 || (!this->nullable && index >= this->length)) return false;
    int length = index < this->length ? this->length : index + 1;
    bool isOk = _array_check_resize(this, length);
    if (!isOk) return false;
    if (this->elements[index] != NULL) {
        Object_release(this->elements[index]);
    }
    Object_retain(element);
    this->length = length;
    this->elements[index] = element;
    return true;
}

void *Array_get(Array *this, int index)
{
    if (index < 0 || index >= this->length) return NULL;
    return this->elements[index];
}

void *Array_getLast(Array *this)
{
    return Array_get(this, this->length - 1);
}

void *Array_getFirst(Array *this)
{
    return Array_get(this, 0);
}

void *Array_del(Array *this, int index)
{
    if (index < 0 || index >= this->length) return NULL;
    void *item = this->elements[index];
    for(int i = index; i < this->length; i++)
    {
        this->elements[i] = i != (this->length - 1) ? this->elements[i + 1] : NULL;
    }
    this->elements[this->length] = NULL;
    this->length = this->length - 1;
    Object_release(item);
    return item;
}

void *Array_delLast(Array *this)
{
    return Array_del(this, this->length - 1);
}

void *Array_delFirst(Array *this)
{
    return Array_del(this, 0);
}

bool _array_insert(Array *this, int index, void *element, bool isBefore)
{
    if (!this->nullable && element == NULL) return false;
    // validate
    if (index < 0) {
        index = 0;
        isBefore = true;
    } else if (index >= this->length) {
        index = this->length - 1;
        isBefore = false;
    }
    Object_retain(element);
    // 
    if (this->length == 0)
    {
        this->elements[0] = element;
        this->length = 1;
        return true;
    }
    // resize
    bool isOk = _array_check_resize(this, this->length + 1);
    if (!isOk) return false;
    // insert
    int i = this->length;
    int to = isBefore ? index : index + 1;
    for (i = this->length - 1; i >= to; i--)
    {
        this->elements[i + 1] = this->elements[i];
    }
    this->elements[to] = element;
    this->length = this->length + 1;
    return true;
}

bool Array_insertBefore(Array *this, int at, void *element)
{
    return _array_insert(this, at, element, true);
}

bool Array_insertAfter(Array *this, int at, void *element)
{
    return _array_insert(this, at, element, false);
}

bool Array_prepend(Array *this, void *element)
{
    return Array_insertBefore(this, 0, element);
}

bool Array_append(Array *this, void *element)
{
    return Array_insertAfter(this, this->length - 1, element);
}

bool Array_push(Array *this, void *element)
{
    return Array_insertAfter(this, this->length - 1, element);
}

void *Array_pop(Array *this)
{
    return Array_del(this, this->length - 1);
}

bool Array_unshift(Array *this, void *element)
{
    return Array_insertBefore(this, 0, element);
}

void *Array_shift(Array *this)
{
    return Array_del(this, 0);
}

// int compare(const void *num1, const void *num2) { return 0; }
void Array_sort(Array *this, ArraySortFunction func)
{
    qsort(this->elements, this->length, sizeof(void *), func);
}

// int search(const void *num2) { return true; }
int Array_find(Array *this, int from, int to, bool isReverse, ArrayFindFunction func)
{
    if (from < 0 || to > this->length || from >= to) return -1;
    int var = isReverse ? -1 : 1;
    if (isReverse) {
        int temp = from;
        from = to - 1;
        to = temp + 1;
    }
    void *item;
    bool result;
    for (int i = from; i < to; i + var) {
        item = Array_get(this, i);
        result = func(item);
        if (result) return i;
    }
    return -1;
}

Array *Array_slice(Array *this, int from, int to)
{
    Array *other = Array_new();
    if (from < 0 || to > this->length || from >= to) return other;
    for (int i = from; i < to; i++) Array_append(other, Array_get(this, i));
    return other;
}

char *Array_toString(Array *this)
{
    return tools_format("<Array p:%p s:%i>", this, this->length);
}

#endif


// ./files/helpers.h 2024-07-14 15:03:00

// helpers

#ifndef H_PCT_HELPERS
#define H_PCT_HELPERS


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
