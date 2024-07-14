// tools

#ifndef H_PCT_TOOLS
#define H_PCT_TOOLS

#include "header.h"  // [M[ IGNORE ]M]

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
