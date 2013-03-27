#include <time.h>

#ifndef __SIND_INIT_H__
#define __SIND_INIT_H__

// User settings
extern unsigned long g_sleep_time;
extern bool g_dzen;
extern int g_lines;

extern char* g_default_format;
extern char* g_default_status;
extern bool g_status_top;
struct _FormatStrings {
	char* new;
    char* no_new;
	char* pending;
	char* no_pending;
    char* app;
    char* no_app;
    char* body;
    char* no_body;
};
extern struct _FormatStrings g_dformat;

// Other
extern struct timespec g_update_interval;

// Functions
void get_args(int argc, char** argv);
void config_clean();

#endif
