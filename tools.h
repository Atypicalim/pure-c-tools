
// ./files/header.h 2024-08-15 13:00:31

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


// \033[1;31mThis is red text.\033[0m\n
#define __PCT_COLOR_TAG_BEGIN "\033[1;"
#define PCT_COLOR_TAG_GREEN __PCT_COLOR_TAG_BEGIN "32m"
#define PCT_COLOR_TAG_BLUE __PCT_COLOR_TAG_BEGIN "32m"
#define PCT_COLOR_TAG_YELLOW __PCT_COLOR_TAG_BEGIN "33m"
#define PCT_COLOR_TAG_RED __PCT_COLOR_TAG_BEGIN "31m"
#define PCT_COLOR_TAG_END "\033[0m"

char PCT_TAG_DEBUG[] = "[DEBUG]";
char PCT_TAG_INFO[]  = "[INFO ]";
char PCT_TAG_WARN[]  = "[WARN ]";
char PCT_TAG_ERROR[] = "[ERROR]";

#endif


// ./files/log.h 2024-08-15 13:00:31

// log

// https://github.com/rxi/log.c

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define PCT_LOG_VERSION "0.1.0"

#define MAX_CALLBACKS 32

typedef struct {
  int level;
  int line;
  const char *file;
  const char *fmt;
  va_list args;
  struct tm *time;
  void *target;
} log_Event;

typedef void (log_Func)(log_Event *ev);

static struct {
  bool quiet;
  int level;
  int color;
  FILE *file;
  log_Func *callbacks;
} L;

enum { PCT_LOG_DEBUG, PCT_LOG_INFO, PCT_LOG_WARN, PCT_LOG_ERROR };

static const char *level_strings[] = {
  PCT_TAG_DEBUG, PCT_TAG_INFO, PCT_TAG_WARN, PCT_TAG_ERROR
};
static const char *level_colors[] = {
  "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m"
};

////////////////////////////////////////////////////////////////////////////////

static void _log_stdio_callback(log_Event *ev) {
  char buf[64];
  buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
  if (L.color) {
    fprintf(
      ev->target,
      "%s %s%-2s\x1b[0m \x1b[90m%s:%03d:\x1b[0m ",
      buf, level_colors[ev->level], level_strings[ev->level],
      ev->file, ev->line
    );
  } else {
    fprintf(
      ev->target,
      "%s %-2s %s:%d: ",
      buf, level_strings[ev->level], ev->file, ev->line
    );
  }
}


static void _log_file_callback(log_Event *ev) {
  if (!L.file) return;
  char buf[128];
  buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
  fprintf(
    ev->target,
    "%s %-2s %s:%d: ",
    buf, level_strings[ev->level], ev->file, ev->line
  );
}

static void _log_fill_args(log_Event *ev) {
  vfprintf(ev->target, ev->fmt, ev->args);
  fprintf(ev->target, "\n");
  fflush(ev->target);
}

static void init_event(log_Event *ev, void *target) {
  if (!ev->time) {
    time_t t = time(NULL);
    ev->time = localtime(&t);
  }
  ev->target = target;
}

void __pct_log(int level, const char *file, int line, const char *fmt, ...) {
  log_Event ev = {
    .fmt   = fmt,
    .file  = file,
    .line  = line,
    .level = level,
  };
  //
  if (!L.quiet && level >= L.level) {
    va_start(ev.args, fmt);
    //
    init_event(&ev, stderr);
    _log_stdio_callback(&ev);
    _log_fill_args(&ev);
    //
    init_event(&ev, L.file);
    _log_file_callback(&ev);
    _log_fill_args(&ev);
    //
    va_end(ev.args);
  }

    log_Func *func = L.callbacks;
    if (func != NULL && level >= L.level) {
      init_event(&ev, NULL);
      va_start(ev.args, fmt);
      func(&ev);
      va_end(ev.args);
    }

}

////////////////////////////////////////////////////////////////////////////////

#define log_debug(...) __pct_log(PCT_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  __pct_log(PCT_LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  __pct_log(PCT_LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) __pct_log(PCT_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////

void log_set_quiet(bool enable) {
  L.quiet = enable;
}

void log_set_level(int level) {
  L.level = level;
}

void log_set_color(bool enabled) {
  L.color = enabled;
}

int log_set_file(char *path) {
  if (!path) {
    L.file = NULL;
  } else {
    L.file = fopen(path, "w");
  }
}

int log_set_func(log_Func *func) {
  L.callbacks = *func;
}


// ./files/tools.h 2024-08-15 13:00:31

// tools

#ifndef H_PCT_TOOLS
#define H_PCT_TOOLS


// os type
#define PLATFORM_WINDOWS "PLATFORM_WINDOWS"
#define PLATFORM_APPLE "PLATFORM_APPLE"
#define PLATFORM_LINUX "PLATFORM_LINUX"
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
    #define IS_UNKNOWM
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


void tools_error(const char* msg, ...) {
    va_list lst;
    va_start(lst, msg);
    printf("%s%s%s => ", PCT_COLOR_TAG_RED, PCT_TAG_ERROR, PCT_COLOR_TAG_END);
    vfprintf(stdout, msg, lst);
    printf("\n");
    va_end(lst);
    exit(1);
}

void tools_assert(bool value, const char *msg, ...)
{
    if (value == true) return;
    va_list lst;
    va_start(lst, msg);
    printf("%s%s%s => ", PCT_COLOR_TAG_RED, PCT_TAG_ERROR, PCT_COLOR_TAG_END);
    vfprintf(stdout, msg, lst);
    printf("\n");
    va_end(lst);
    exit(1);
}

void tools_warn(const char* msg, ...) {
    va_list lst;
    va_start(lst, msg);
    printf("%s%s%s => ", PCT_COLOR_TAG_YELLOW, PCT_TAG_WARN, PCT_COLOR_TAG_END);
    vfprintf(stdout, msg, lst);
    printf("\n");
    va_end(lst);
}

void tools_log(const char* msg, ...) {
    va_list lst;
    va_start(lst, msg);
    printf("%s%s%s => ", PCT_COLOR_TAG_GREEN, PCT_TAG_INFO, PCT_COLOR_TAG_END);
    vfprintf(stdout, msg, lst);
    printf("\n");
    va_end(lst);
}

void tools_debug(const char* msg, ...) {
    va_list lst;
    va_start(lst, msg);
    printf("%s%s%s => ", PCT_COLOR_TAG_BLUE, PCT_TAG_DEBUG, PCT_COLOR_TAG_END);
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


// ./files/object.h 2024-08-15 13:00:31


#ifndef H_PCT_UG_OBJECT
#define H_PCT_UG_OBJECT

void pct_object_free(void *object);
void pct_object_print(void *object);

typedef struct _Object {
    char objType;
    int gcCount;
    char gcMark;
    char gcFreeze;
    void* gcNext;
} Object;

void Object_init(void *_this, char _objType)
{
    Object *this = _this;
    this->objType = _objType;
    this->gcCount = 1;
    this->gcMark = 0;
    this->gcFreeze = true;
    this->gcNext = NULL;
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
    if (_this == NULL) tools_error("null pointer to object retain");
    Object *this = _this;
    this->gcCount++;
}

void Object_release(void *_this)
{
    if (_this == NULL) tools_error("null pointer to object release");
    Object *this = _this;
    this->gcCount--;
    if (this->gcCount <= 0) {
        #ifdef H_PCT_OBJECT_CALLBACKS
        Object_freeByType(this->objType, this);
        #else
        pct_object_free(this);
        #endif
    }
}

void Object_print(void *_this)
{
    if (_this == NULL) tools_error("null pointer to object print");
    Object *this = _this;
    #ifdef H_PCT_OBJECT_CALLBACKS
    Object_printByType(this->objType, this);
    #else
    pct_object_print(this);
    #endif
}

#endif


// ./files/cstring.h 2024-08-15 13:00:31


// HEADER ---------------------------------------------------------------------

#ifndef STDSTRING_H
#define STDSTRING_H

#include <stdarg.h>
#include <stdint.h>
#include <wchar.h>
#include <stdbool.h>
#include <math.h>  /* remember to compile with -lm */
#include <setjmp.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#ifndef AVA
   // code annonations
   #define HEAP           /* heap pointer. must free() after use */
   #define TEMP           /* temporary stack pointer. do not free() after use */
   #define INOUT          /* both input and output argument */
   #define OPTIONAL       /* optional argument */
   // mem wrappers
   #define REALLOC(p, sz) realloc((p), (sz))
   #define MALLOC(sz)     REALLOC(0, (sz))
   #define CALLOC(n, m)   memset(MALLOC((n)*(m)), 0, (n)*(m))
   #define FREE(p)        REALLOC(p, 0)
   #define STRDUP(s)      strdup(s)
   // compiler stuff
   #ifdef _MSC_VER
   #define builtin(x)     __declspec(x)
   #else
   #define builtin(x)     __##x
   #endif
   // raii perf/profiler
   #define $
#endif

// IMPLEMENTATION -------------------------------------------------------------

// stricmp() is common, but not standard, so I provide my own
static int istrcmp(const char *p, const char *q) {
    for(; tolower(p[0]) == tolower(q[0]) && p[0]; p++, q++);
    return tolower(p[0]) - tolower(q[0]);
}

// ## rob pike's regexp match (apparently public domain).
// [ref] https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
// @todo: evaluate @kokke/tiny-regex-c instead.

int regstar(const char *string, const char *re, int c);

int reghere(const char *string, const char *re) {
    if( re[0] == '\0' ) return 1;
    if( re[1] == '*' ) return regstar(string, re+2, re[0]);
    if( re[0] == '$' && re[1] == '\0' ) return *string == '\0';
    if( *string!='\0' && (re[0]=='?' || re[0]==*string) ) return reghere(string+1, re+1);
    return 0;
}

int regstar(const char *string, const char *re, int c) {
    do { /* a * matches zero or more instances */
        if( reghere(string, re) ) return 1;
    } while( *string != '\0' && (*string++ == c || c == '?') );
    return 0;
}

int strregex(const char *string, const char *re) {
    if( re[0] == '^' ) return reghere(string, re+1);
    do { /* must look even if string is empty */
        if( reghere(string, re) ) return 1;
    } while (*string++ != '\0');
    return 0;
}

bool strmatch( const char *text, const char *pattern ) { $
    if( *pattern=='\0' ) return !*text;
    if( *pattern=='*' )  return strmatch(text, pattern+1) || (*text && strmatch(text+1, pattern));
    if( *pattern=='?' )  return *text && (*text != '.') && strmatch(text+1, pattern+1);
    return (*text == *pattern) && strmatch(text+1, pattern+1);
}

// ## C-style formatting
// - rlyeh, public domain.

const char *strsub( const char *str, int pos ) { $
    int size = strlen(str);
    pos = pos && size ? (pos > 0 ? pos % size : size-1 + ((pos+1) % size)) : 0;
    return str + pos;
}

const char* strfindl(const char *text, const char *substring) { $
    const char *found = strstr( text, substring );
    return found ? found : text + strlen(text);
}
const char* strfindr(const char *text, const char *substring) { $
    char *found = 0;
    while(1) {
        char *found2 = strstr(text, substring);
        if( !found2 ) break;
        found = found2;
        text = found2 + 1;
    }
    return found ? found : text + strlen(text);
}

bool strbegin( const char *text, const char *substring ) { $
    // also, return strncmp(string, substr, strlen(substr)) == 0;
    int s1 = strlen(text), s2 = strlen(substring);
    return s1 >= s2 ? 0 == memcmp( &text[       0 ], substring, s2 ) : false;
}
bool strend( const char *text, const char *substring ) { $
    int s1 = strlen(text), s2 = strlen(substring);
    return s1 >= s2 ? 0 == memcmp( &text[ s1 - s2 ], substring, s2 ) : false;
}

bool streq( const char *string, const char *substr ) { $
    return !strcmp( string, substr );
}
bool streqi( const char *string, const char *substr ) { $
    while( *string && *substr ) {
        int eqi = (*string++ | 32) == (*substr++ | 32);
        if( !eqi ) return 0;
    }
    return *string == 0 && *substr == 0;
}

// ## string conversion unit utils
// - rlyeh, public domain.

int64_t strint( const char *string ) { $
    int64_t v = 0, n = 1;
    if( string ) {
        while( *string == '-' ) n *= -1, string++;
        while( *string >= '0' && *string <= '9') v = (v * 10) + *string++ - '0';
    }
    return n * v;
}

// ## string transform utils
// First four functions are based on code by Bob Stout (public domain).
// - rlyeh, public domain.

char *strlower(char *string) {
    if( string ) for( char *s = string; *s; ++s ) *s = tolower(*s);
    return string;
}
char *strupper(char *string) {
    if( string ) for( char *s = string; *s; ++s ) *s = toupper(*s);
    return string;
} 
char *strrev(char *string) {
    if(string && *string)
    for( char *p1 = string, *p2 = p1 + strlen(p1) - 1; p2 > p1; ++p1, --p2 ) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return string;
}
char *strdel(char *string, const char *substring) {
    if( string ) {
        char *p = strstr(string, substring);
        if( p ) {
            for( int len = strlen(substring); p[len] ; ++p ) {
                p[0] = p[len];
            }
            *p = 0;
        }
    }
    return string;
}

// ## string transform utils
// - rlyeh, public domain.

char* strtrimbff(char *string, const char *substring) { $
    char *found = strstr(string, substring);
    if( found ) {
        int L = strlen(substring);
        memmove(string, found+L, strlen(string) - L);
    }
    return string;
}
char* strtrimffe(char *string, const char *substring) { $
    ((char *)strfindl(string, substring))[0] = 0;
    return string;
}
char* strtrimblf(char *string, const char *substring) { $
    const char *found = strfindr(string, substring);
    int L = strlen(substring);
    memmove( string, found + L, strlen(found) - L + 1);
    return string;
}
char* strtrimlfe(char *string, const char *substring) { $
    ((char *)strfindr(string, substring))[0] = 0;
    return string;
}
char *strtrimws(char *str) {
    char *ibuf, *obuf;
    if( str ) {
        for( ibuf = obuf = str; *ibuf; ) {
            while( *ibuf && isspace(*ibuf)  )  (ibuf++);
            if(    *ibuf && obuf != str     ) *(obuf++) = ' ';
            while( *ibuf && !isspace(*ibuf) ) *(obuf++) = *(ibuf++);
        }
        *obuf = 0;
    }
    return str;
}

// ## replace substring in a string
// - rlyeh, public domain.


// ## split strings into tokens
// - rlyeh, public domain.

int strchop( const char *string, const char *delimiters, int avail, const char *tokens[] ) { $
    assert( avail >= 4 && 0 == ( avail % 2 ) );
    for( avail /= 2; *string && avail-- > 0; ) {
        int n = strcspn( string += strspn(string, delimiters), delimiters );
        *tokens++ = (*tokens++ = (const char *)(uintptr_t)n) ? string : "";
        string += n;
    }
    return *tokens++ = 0, *tokens = 0, avail > 0;
}

HEAP char **strsplit( const char *string, const char *delimiters ) { $
    int L = strlen(string), len = sizeof(char *) * (L/2+1+1), i = 0;
    char **res = (char **)CALLOC(1, len + L + 1 );
    char *buf = strcpy( (char *)res + len, string );
    for( char *token = strtok(buf, delimiters); token; token = strtok(NULL, delimiters) ) {
        res[i++] = token;
    }
    return res;
}

// ## string interning (quarks)
// - rlyeh, public domain.

static builtin(thread) int quarklen = 0, quarkcap = 0;
static builtin(thread) char *quarks = 0;

int strput( const char *string ) { $
    if( !quarks ) {
        // init buffer on first time
        quarks = (char*)REALLOC( quarks, (1+1) );
        quarkcap += 1;
        // copy null string
        quarks[0] = 0;
        quarklen += 1;
        quarkcap -= 1;
    }
    if( string && string[0] ) {
        int len = strlen(string)+1;
        if( quarkcap < len ) {
            int extend = (int)(len * 1.5f); // 2.33f
            // printf("alloc += %d\n", extend);
            quarks = (char*)REALLOC( quarks, quarklen + extend );
            quarkcap += extend;
        }
        memcpy( quarks + quarklen, string, len );
        quarklen += len;
        quarkcap -= len;
        // printf("%d/%d\n", quarklen, quarklen+quarkcap);
        return quarklen - len;
    }
    return 0;
}
const char *strget( int key ) {
    assert( quarks );
    return quarks + key;
}

// ## Fuzzy string matching
// Ideas from https://blog.forrestthewoods.com/reverse-engineering-sublime-text-s-fuzzy-match-4cffeed33fdb
// - rlyeh, public domain.


int strscore( const char *str1, const char *str2 ) {
    int score = 0, consecutive = 0, maxerrors = 0;
    while( *str1 && *str2 ) {
        int is_leading = (*str1 & 64) && !(str1[1] & 64);
        if( (*str1 & ~32) == (*str2 & ~32) ) {
            int had_separator = (str1[-1] <= 32);
            int x = had_separator || is_leading ? 10 : consecutive * 5;
            consecutive = 1;
            score += x;
            ++str2;
        } else {
            int x = -1, y = is_leading * -3;
            consecutive = 0;
            score += x;
            maxerrors += y;
        }
        ++str1;
    }
    return score + (maxerrors < -9 ? -9 : maxerrors);
}

const char *strfuzzy( const char *str, int num, const char *words[] ) {
    int scoremax = 0;
    const char *best = 0;
    for( int i = 0; i < num; ++i ) {
        int score = strscore( words[i], str );
        int record = ( score >= scoremax );
        int draw = ( score == scoremax );
        if( record ) {
            scoremax = score;
            if( !draw ) best = words[i];
            else best = best && strlen(best) < strlen(words[i]) ? best : words[i];
        }
    }
    return best ? best : "";
}

// ## utf8 and unicode
// Based on code by @ddiakopoulos (unlicensed).
// Based on code by @nothings (public domain).
// - rlyeh, public domain.

uint32_t strutf32(INOUT const char **p) { $
    if( (**p & 0x80) == 0x00 ) {
        int a = *((*p)++);
        return a;
    }
    if( (**p & 0xe0) == 0xc0 ) {
        int a = *((*p)++) & 0x1f;
        int b = *((*p)++) & 0x3f;
        return (a << 6) | b;
    }
    if( (**p & 0xf0) == 0xe0 ) {
        int a = *((*p)++) & 0x0f;
        int b = *((*p)++) & 0x3f;
        int c = *((*p)++) & 0x3f;
        return (a << 12) | (b << 6) | c;
    }
    if( (**p & 0xf8) == 0xf0 ) {
        int a = *((*p)++) & 0x07;
        int b = *((*p)++) & 0x3f;
        int c = *((*p)++) & 0x3f;
        int d = *((*p)++) & 0x3f;
        return (a << 18) | (b << 12) | (c << 8) | d;
    }
    return 0;
}
static builtin(thread) uint8_t utf[8] = {0};
TEMP char *strutf8(uint32_t cp) { $
    int n = 0;
    /**/ if( cp <        0x80 ) n = 1;
    else if( cp <       0x800 ) n = 2;
    else if( cp <     0x10000 ) n = 3;
    else if( cp <    0x200000 ) n = 4;
    else if( cp <   0x4000000 ) n = 5;
    else if( cp <= 0x7fffffff ) n = 6;
    switch (n) {
        case 6: utf[5] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0x4000000;
        case 5: utf[4] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0x200000;
        case 4: utf[3] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0x10000;
        case 3: utf[2] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0x800;
        case 2: utf[1] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0xc0;
        case 1: utf[0] = cp; default:;
    }
    return utf[n] = '\0', (char *)utf;
}

wchar_t *strwiden_(wchar_t *buffer, const char *ostr, int n) { $
   const uint8_t *str = (const uint8_t *) ostr;
   uint32_t c;
   int i=0;
   --n;
   while (*str) {
      if (i >= n)
         return NULL;
      if (!(*str & 0x80))
         buffer[i++] = *str++;
      else if ((*str & 0xe0) == 0xc0) {
         if (*str < 0xc2) return NULL;
         c = (*str++ & 0x1f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         buffer[i++] = c + (*str++ & 0x3f);
      } else if ((*str & 0xf0) == 0xe0) {
         if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return NULL;
         if (*str == 0xed && str[1] > 0x9f) return NULL; // str[1] < 0x80 is checked below
         c = (*str++ & 0x0f) << 12;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         buffer[i++] = c + (*str++ & 0x3f);
      } else if ((*str & 0xf8) == 0xf0) {
         if (*str > 0xf4) return NULL;
         if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return NULL;
         if (*str == 0xf4 && str[1] > 0x8f) return NULL; // str[1] < 0x80 is checked below
         c = (*str++ & 0x07) << 18;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 12;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f);
         // utf-8 encodings of values used in surrogate pairs are invalid
         if ((c & 0xFFFFF800) == 0xD800) return NULL;
         if (c >= 0x10000) {
            c -= 0x10000;
            if (i + 2 > n) return NULL;
            buffer[i++] = 0xD800 | (0x3ff & (c >> 10));
            buffer[i++] = 0xDC00 | (0x3ff & (c      ));
         }
      } else
         return NULL;
   }
   buffer[i] = 0;
   return buffer;
}

char *strshorten_(char *buffer, const wchar_t *str, int n) { $
   int i=0;
   --n;
   while (*str) {
      if (*str < 0x80) {
         if (i+1 > n) return NULL;
         buffer[i++] = (char) *str++;
      } else if (*str < 0x800) {
         if (i+2 > n) return NULL;
         buffer[i++] = 0xc0 + (*str >> 6);
         buffer[i++] = 0x80 + (*str & 0x3f);
         str += 1;
      } else if (*str >= 0xd800 && *str < 0xdc00) {
         uint32_t c;
         if (i+4 > n) return NULL;
         c = ((str[0] - 0xd800) << 10) + ((str[1]) - 0xdc00) + 0x10000;
         buffer[i++] = 0xf0 + (c >> 18);
         buffer[i++] = 0x80 + ((c >> 12) & 0x3f);
         buffer[i++] = 0x80 + ((c >>  6) & 0x3f);
         buffer[i++] = 0x80 + ((c      ) & 0x3f);
         str += 2;
      } else if (*str >= 0xdc00 && *str < 0xe000) {
         return NULL;
      } else {
         if (i+3 > n) return NULL;
         buffer[i++] = 0xe0 + (*str >> 12);
         buffer[i++] = 0x80 + ((*str >> 6) & 0x3f);
         buffer[i++] = 0x80 + ((*str     ) & 0x3f);
         str += 1;
      }
   }
   buffer[i] = 0;
   return buffer;
}

TEMP wchar_t *strwiden(const char *str) {
   int len = strlen(str) * 6;
   wchar_t *buffer = (wchar_t*)MALLOC( len );
   return strwiden_(buffer, str, len);
}

TEMP char *strshorten(const wchar_t *str) {
   int len = wcslen(str) * 6;
   char *buffer = (char*)MALLOC( len );
   return strshorten_(buffer, str, len);
}

uint64_t strhash(const char *str) {
    uint64_t hash = 0;
    while( *str++ ) {
        hash = ( str[-1] ^ hash ) * 131ull;
    }
    return hash;
}

#endif // STDSTRING_H



// ./files/string.h 2024-08-15 13:00:31

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


// ./files/cursor.h 2024-08-15 13:00:31

// cursor

#ifndef H_PCT_CURSOR
#define H_PCT_CURSOR

typedef struct _Cursor {
    struct _Object;
    void *target;
} Cursor;

Cursor *Cursor_new(void *target)
{
    Cursor *cursor = (Cursor *)pct_mallloc(sizeof(Cursor));
    Object_init(cursor, PCT_OBJ_CURSOR);
    cursor->target = target;
    return cursor;
}

void Cursor_set(Cursor *this, void *target)
{
    this->target = target;
}

void *Cursor_get(Cursor *this)
{
    return this->target;
}

void Cursor_print(Cursor *this)
{
    printf("[(CURSOR) => p:%s, t:%s]\n", this, this->target);
}

void Cursor_free(Cursor *this)
{
    this->target = NULL;
    Object_free(this);
}

#endif


// ./files/hashkey.h 2024-08-15 13:00:31

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


// ./files/hashmap.h 2024-08-15 13:00:31

// hashmap

#ifndef H_PCT_HASHMAP
#define H_PCT_HASHMAP

#define HASHMAP_DEFAULT_CAPACITY 4096

typedef struct _Hashmap {
    struct _Object;
    int size;
    bool retain;
    Hashkey *bucket[HASHMAP_DEFAULT_CAPACITY];
} Hashmap;

Hashmap* Hashmap_new(bool isRetainValue) {
    Hashmap *map = (Hashmap *)pct_mallloc(sizeof(Hashmap));
    Object_init(map, PCT_OBJ_HASHMAP);
    map->retain = isRetainValue;
    map->size = HASHMAP_DEFAULT_CAPACITY;
    for (int i = 0; i < map->size; ++i ) map->bucket[i] = NULL;
    return map;
}

// TODO: release removed value
void Hashmap_free(Hashmap *this) {
    assert(this != NULL);
    Hashkey *ptr;
    Hashkey *tmp;
    for (int i = 0; i < this->size; ++i) {
        ptr = this->bucket[i];
        while (ptr != NULL) {
            tmp = ptr;
            ptr = ptr->next;
            if (this->retain) {
                Object_release(tmp->value);
            }
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
    int pos = String_hash(key) % this->size;
    if (this->retain) {
        Object_retain(value);
    }
    //
    void *tmp = NULL;
    Hashkey *ptr = this->bucket[pos];
    if (ptr == NULL) {
        this->bucket[pos] = Hashkey_new(key, value);
        Object_release(key);
        return NULL;
    }
    while (ptr != NULL) {
        if (String_equal(key, ptr->key)) {
            tmp = ptr->value;
            if (this->retain) {
                Object_release(ptr->value);
            }
            Hashkey_set(ptr, value);
            Object_release(key);
            // TODO: release tmp
            return tmp;
        }
        ptr = ptr->next;
    }
    Hashkey *pnode = Hashkey_new(key, value);
    Object_release(key);
    pnode->next = this->bucket[pos];
    this->bucket[pos] = pnode;
    return NULL;
}

void *Hashmap_get(Hashmap *this, char *_key) {
    assert(this != NULL);
    assert(_key != NULL);
    String *key = String_format(_key);
    int pos = String_hash(key) % this->size;
    //
    Hashkey *ptr = this->bucket[pos];
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
    int pos = String_hash(key) % this->size;
    //
    void *tmp = NULL;
    Hashkey *ptr = this->bucket[pos];
    if (ptr == NULL) {
        Object_release(key);
        return NULL;
    }
    Hashkey *pre = NULL;
    while (ptr != NULL) {
        if (String_equal(key, ptr->key)) {
            if (pre == NULL) {
                this->bucket[pos] = NULL;
            } else {
                pre->next = ptr->next;
            }
            tmp = ptr->value;
            if (this->retain) {
                Object_release(ptr->value);
            }
            Object_release(ptr);
            break;
        }
        pre = ptr;
        ptr = ptr->next;
    }
    Object_release(key);
    return tmp;
}

typedef void (*HASHMAP_FOREACH_FUNC)(Hashkey *, void *);

void Hashmap_foreachItem(Hashmap *this, HASHMAP_FOREACH_FUNC func, void *arg) {
    Hashkey *ptr;
    for (int i = 0; i < this->size; ++i) {
        ptr = this->bucket[i];
        while (ptr != NULL) {
            func(ptr, arg);
            ptr = ptr->next;
        }
    }
}

char *Hashmap_toString(Hashmap *this)
{
    return tools_format("[Hashmap => p:%p s:%i]", this, this->size);
}

#endif


// ./files/foliage.h 2024-08-15 13:00:31

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


// ./files/block.h 2024-08-15 13:00:31

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


// ./files/queue.h 2024-08-15 13:00:31

// queue

#ifndef H_PCT_QUEUE
#define H_PCT_QUEUE


typedef struct _Queue {
    struct _Object;
    int size;
    Block *head;
    Block *tail;
    Cursor *cursor;
} Queue;

Queue *Queue_new()
{
    Queue *queue = (Queue *)pct_mallloc(sizeof(Queue));
    Object_init(queue, PCT_OBJ_QUEUE);
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    queue->cursor = Cursor_new(NULL);
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
    Object_free(this->cursor);
    Object_free(this);
}

void Queue_RESTE(Queue *this) {
    Cursor_set(this->cursor, this->head);
}

void *Queue_NEXT(Queue *this) {
    Block *temp = Cursor_get(this->cursor);
    if (temp == NULL) return NULL;
    Cursor_set(this->cursor, temp->next);
    return temp->data;
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


// ./files/stack.h 2024-08-15 13:00:31

// stack

#ifndef H_PCT_STACK
#define H_PCT_STACK


typedef struct _Stack {
    struct _Object;
    int size;
    Block *head;
    Block *tail;
    Cursor *cursor;
} Stack;

Stack *Stack_new()
{
    Stack *stack = (Stack *)pct_mallloc(sizeof(Stack));
    Object_init(stack, PCT_OBJ_STACK);
    stack->size = 0;
    stack->head = NULL;
    stack->tail = NULL;
    stack->cursor = Cursor_new(NULL);;
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
    Object_free(this->cursor);
    Object_free(this);
}

void Stack_RESTE(Stack *this) {
    Cursor_set(this->cursor, this->tail);
}

void *Stack_NEXT(Stack *this) {
    Block *temp = Cursor_get(this->cursor);
    if (temp == NULL) return NULL;
    Cursor_set(this->cursor, temp->last);
    return temp->data;
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


typedef void (*STACK_FOREACH_FUNC)(void *, void *);

void Stack_foreachItem(Stack *this, STACK_FOREACH_FUNC func, void *arg) {
    Cursor *cursor = Stack_reset(this);
    void *ptr = NULL;
    while ((ptr = Stack_next(this, cursor)) != NULL) {
        func(ptr, arg);
    }
    Cursor_free(cursor);
}

#endif


// ./files/array.h 2024-08-15 13:00:31

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


// ./files/helpers.h 2024-08-15 13:00:31

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
    int count = this->gcCount;
    printf("<Object t:%c c:%i p:%p>\n", type, count, this);
}

#endif
