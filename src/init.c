#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>

#include "list.h"
#include "init.h"

// User Settings
struct timespec g_update_interval =
{ 0, 500000000 };
unsigned long g_sleep_time = 5000;
bool g_dzen = false;
int g_lines = 0;
char* g_new_notify_string = "";
char* g_pending_notify_string = "";
char* g_no_pending_notify_string = "";
char* g_seperator = " : ";

// Functions
static void print_help(char* path) {
    fprintf(stdout,
    "Simple Notification Daemon\n"
    "Usage:\t%s [-t TIME]\n"
    "Prints freedesktop notifications to stdout\n"
    "\n"
    "Options:\n"
    "\t-h, --help:\t\tShow this message\n"
    "\t-l, --lines=\t\tIf set to $LINES, will give an curses-like output in your terminal.\n"
    "\t-s, --seperator=\t\tThe default seperates APP SUMMARY & BODY with \' : \'. Change this.\n"
    "\t-n, --new=\t\tThe provided string will be printed after all notifications if there was a new notification added to the queue on this update\n"
    "\t-p, --pending=\t\tThe provided string will be printed before all notifications if there are any notifications on the queue.\n"
    "\t-r, --no_pending=\tThe provided string will be printed before all notifications if there are no notifications on the queue.\n"
    "\t-d, --dzen:\t\tWill subtract two lines (\'-p\' or \'-r\' and \'-n\') from the total number of lines provided by \'-l\', as to keep notifications in the dzen slave window. (If \'-pr\' or \'-n\' are included, of course)\n"
    "\t-u, --update_interval=\tSet default update interval for fetching pending notifications, and checking the list for expirations, in milliseconds. Lower will make the list seem more responsive, but will eat more CPU. Defaults to 500ms.\n"
    "\t-t, --timeout=\t\tSet default notification expiration in milliseconds. Defaults to 5 seconds (5000ms).\n"
    ,
    path);
}

void get_args(int argc, char** argv){
    int c;
    int optx = 0;
    int helpstat = EXIT_SUCCESS;
    bool help = false;
    char* nlines = NULL;;
    char* user_timeout = NULL;
    char* user_update_interval = NULL;

    struct option options[] = {
        {"help", no_argument, 0, 'h'},
        {"lines", required_argument, 0, 'l'},
        {"seperator", required_argument, 0, 's'},
        {"dzen", no_argument, 0, 'd'},
        {"new", required_argument, 0, 'n'},
        {"pending", required_argument, 0, 'p'},
        {"no_pending", required_argument, 0, 'r'},
        {"update_interval", required_argument, 0, 'u'},
        {"timeout", required_argument, 0, 't'},
    };

    while ((c = getopt_long(argc, argv, "hl:dn:p:r:u:t:s:", options, &optx)) != -1) {
        switch (c) {
            case 't':
                user_timeout = optarg;
                break;
            case 's':
                g_seperator = optarg;
                break;
            case 'l':
                nlines = optarg;
                break;
            case 'u':
                user_update_interval = optarg;
                break;
            case 'h':
                help = true;
                break;
            case 'n':
                g_new_notify_string = optarg;
                break;
            case 'p':
                g_pending_notify_string = optarg;
                break;
            case 'r':
                g_no_pending_notify_string = optarg;
                break;
            case 'd':
                g_dzen = true;
                break;
            default:
                help = true;
                helpstat = EXIT_FAILURE;
        }
    }

    if (help) {
        print_help(argv[0]);
        exit(helpstat);
    }

    if (user_timeout) {
        long tmp_time = strtol(user_timeout, NULL, 10);
        if (tmp_time <= 0) {
            fprintf(stderr, "Error: \'-t\' argument must be greater than 0.\n");
            exit(EXIT_FAILURE);
        }
        g_sleep_time = (unsigned long)tmp_time;
    }

    if (nlines) {
        int tmp_lines = strtol(nlines, NULL, 10);
        if (tmp_lines <= 0) {
            fprintf(stderr, "Error: \'-l\' argument must be greater than 0.\n");
            exit(EXIT_FAILURE);
        }
        g_lines = (int)tmp_lines;
    }

    if (user_update_interval) {
        long tmp_ui = strtol(user_update_interval, NULL, 10);
        if (tmp_ui <= 0) {
            fprintf(stderr, "Error: \'-u\' argument must be greater than 0.\n");
            exit(EXIT_FAILURE);
        }
        g_update_interval.tv_sec = ((unsigned long)tmp_ui) / 1000 ;
        g_update_interval.tv_nsec = (((unsigned long)tmp_ui) % 1000 ) * 1000000 ;
    }
}
