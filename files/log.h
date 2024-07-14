// log

// https://github.com/rxi/log.c

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define PCT_LOG_VERSION "0.1.0"
// #define PCT_LOG_USE_COLOR

#define MAX_CALLBACKS 32

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} log_Event;

typedef void (*log_LockFn)(bool lock, void *udata);
typedef void (*log_LogFn)(log_Event *ev);

typedef struct {
  log_LogFn fn;
  void *udata;
  int level;
} Callback;

static struct {
  void *udata;
  log_LockFn lock;
  int level;
  bool quiet;
  Callback callbacks[MAX_CALLBACKS];
} L;

enum { PCT_LOG_DEBUG, PCT_LOG_INFO, PCT_LOG_WARN, PCT_LOG_ERROR };

static const char *level_strings[] = {
  PCT_TAG_DEBUG, PCT_TAG_INFO, PCT_TAG_WARN, PCT_TAG_ERROR
};
static const char *level_colors[] = {
  "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m"
};

////////////////////////////////////////////////////////////////////////////////

static void lock(void)   {
  if (L.lock) { L.lock(true, L.udata); }
}


static void unlock(void) {
  if (L.lock) { L.lock(false, L.udata); }
}

void log_set_lock(log_LockFn fn, void *udata) {
  L.lock = fn;
  L.udata = udata;
}

////////////////////////////////////////////////////////////////////////////////

static void stdout_callback(log_Event *ev) {
  char buf[16];
  buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
#ifdef PCT_LOG_USE_COLOR
  fprintf(
    ev->udata,
    "%s %s%-2s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
    buf, level_colors[ev->level], level_strings[ev->level],
    ev->file, ev->line
  );
#else
  fprintf(
    ev->udata,
    "%s %-2s %s:%d: ",
    buf, level_strings[ev->level], ev->file, ev->line
  );
#endif
  vfprintf(ev->udata, ev->fmt, ev->ap);
  fprintf(ev->udata, "\n");
  fflush(ev->udata);
}


static void file_callback(log_Event *ev) {
  char buf[64];
  buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
  fprintf(
    ev->udata,
    "%s %-2s %s:%d: ",
    buf, level_strings[ev->level], ev->file, ev->line
  );
  vfprintf(ev->udata, ev->fmt, ev->ap);
  fprintf(ev->udata, "\n");
  fflush(ev->udata);
}

int log_add_callback(log_LogFn fn, void *udata, int level) {
  for (int i = 0; i < MAX_CALLBACKS; i++) {
    if (!L.callbacks[i].fn) {
      L.callbacks[i] = (Callback) { fn, udata, level };
      return 0;
    }
  }
  return -1;
}

static void init_event(log_Event *ev, void *udata) {
  if (!ev->time) {
    time_t t = time(NULL);
    ev->time = localtime(&t);
  }
  ev->udata = udata;
}

void __pct_log(int level, const char *file, int line, const char *fmt, ...) {
  log_Event ev = {
    .fmt   = fmt,
    .file  = file,
    .line  = line,
    .level = level,
  };

  lock();

  if (!L.quiet && level >= L.level) {
    init_event(&ev, stderr);
    va_start(ev.ap, fmt);
    stdout_callback(&ev);
    va_end(ev.ap);
  }

  for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++) {
    Callback *cb = &L.callbacks[i];
    if (level >= cb->level) {
      init_event(&ev, cb->udata);
      va_start(ev.ap, fmt);
      cb->fn(&ev);
      va_end(ev.ap);
    }
  }

  unlock();
}

////////////////////////////////////////////////////////////////////////////////

#define log_debug(...) __pct_log(PCT_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  __pct_log(PCT_LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  __pct_log(PCT_LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) __pct_log(PCT_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////

int log_set_file(char *path, int level) {
    FILE *fp = fopen(path, "w");;
  return log_add_callback(file_callback, fp, level);
}

void log_set_level(int level) {
  L.level = level;
}


void log_set_quiet(bool enable) {
  L.quiet = enable;
}
