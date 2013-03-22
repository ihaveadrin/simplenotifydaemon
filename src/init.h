#include <time.h>

#ifndef __SIND_INIT_H__
#define __SIND_INIT_H__

// User settings
extern unsigned long g_sleep_time;
extern bool g_dzen;
extern int g_lines;
extern char* g_new_notify_string;
extern char* g_pending_notify_string;
extern char* g_no_pending_notify_string;
extern char* g_seperator;

// Functions
void get_args(int argc, char** argv);
extern struct timespec g_update_interval;

#endif
