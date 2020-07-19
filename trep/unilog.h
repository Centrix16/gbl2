#ifndef __UNILOG_H_INCLUDED__
#define __UNILOG_H_INCLUDED__

static int is_log = 1;
static FILE *log_file = NULL;
static char *mode = "a";

#define make_log(val) is_log = 1
#define target(name) log_file = fopen(name, mode); if (!log_file) {perror("log"); exit(0);}
#define log(...) if (is_log) fprintf(log_file, __VA_ARGS__)
#define end() fclose(log_file)
#define log_mode(new_mode) mode = new_mode

#endif
