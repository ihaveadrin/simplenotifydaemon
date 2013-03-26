#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "list.h"
#include "init.h"
#include "format.h"

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
    "Usage:\t%s [OPTIONS]\n"
    "Prints freedesktop notifications to stdout\n"
    "\n"
    "Options:\n"
    "\t-h, --help\t\tShow this message\n"
    "\t-c, --conf=\t\tRead settings from alternate config file\n"
    "\t-l, --lines=\t\tIf set to $LINES, will give an curses-like output in your terminal.\n"
    "\t-f, --format=\t\tThe format to print each notification. See FORMAT\n"
    "\t-n, --new=\t\tThe provided string will be printed after all notifications if there was a new notification added to the queue on this update\n"
    "\t-p, --pending=\t\tThe provided string will be printed before all notifications if there are any notifications on the queue.\n"
    "\t-r, --no_pending=\tThe provided string will be printed before all notifications if there are no notifications on the queue.\n"
    "\t-d, --dzen\t\tWill subtract two lines (\'-p\' or \'-r\' and \'-n\') from the total number of lines provided by \'-l\', as to keep notifications in the dzen slave window. (If \'-pr\' or \'-n\' are included, of course)\n"
    "\t-u, --update_interval=\tSet default update interval for fetching pending notifications, and checking the list for expirations, in milliseconds. Lower will make the list seem more responsive, but will eat more CPU. Defaults to 500ms.\n"
    "\t-t, --timeout=\t\tSet default notification expiration in milliseconds. Defaults to 5 seconds (5000ms).\n"
    "\n"
    "Configuration File:\n"
    "\tSiND will look in ~/.sindrc (or the location specified by --conf) for a configuration file. These provide default values for the above arguements if they are not included when SiND is executed. The configuration file can consist of [KEY]=[VALUE] pairs, with KEY cooresponding to the longoptions above, seperated by newlines. It can also consist of comments, if the line begins with \'#\', and empty lines (with no other whitespace)\n"
    "\n"
    "Format:\n"
    "\tWhen used with the -f --format= option, you can control the output of each line with a date-like format string\n"
    "\tThese are the current interpreted sequences:\n"
    "\t\t%%a\tthe name of the application sending the notification\n"
    "\t\t%%s\tthe summary of the notification\n"
    "\t\t%%b\tthe body of the notification\n"
    "\t\t%%i\tthe id of the notification\n"
    ,
    path);
}

struct ConfigArray {
    bool used;
    char* optarg;
    char key;
};
static int g_conf_arr_len;
static struct ConfigArray** g_conf_arr;

static struct ConfigArray** config_parse(int* arrc, struct option* options, int optc, char* conf_loc) {
    /*Setup File*/
    FILE* conf_file = NULL;
    if (conf_loc) {
        if (*conf_loc == '~') {
            chdir(getenv("HOME"));
            conf_loc ++;
            if (*conf_loc == '/') conf_loc++;
        }
        conf_file = fopen(conf_loc, "r");
    }
    if (!conf_file) {
        chdir(getenv("HOME"));
        conf_file = fopen(".sindrc", "r");
    }
    if (!conf_file) {
        return NULL;
    }

    /*Setup Array*/
    struct ConfigArray** arr = NULL;
    int arr_len = 0;

    /*Setup getline*/
    size_t line_size = 30;
    char* line = malloc(sizeof(char) * line_size);

    while (getline(&line,&line_size,conf_file) != -1) {
        // Skip comments and blank lines
        if ((*line == '#') || (*line == '\n')) continue;

        // Start at 1 to skip --help
        for (int x = 1; x < optc; x++) {
            if (!strncmp(line,options[x].name,strlen(options[x].name))) {
                char* value = strpbrk(line, "=");
                if (value) {
                    value++;
                    arr = realloc(arr, sizeof(struct ConfigArray*) * (arr_len + 1));
                    arr[arr_len] = malloc(sizeof(struct ConfigArray));
                    arr[arr_len]->used = false;
                    arr[arr_len]->key = options[x].val;
                    arr[arr_len]->optarg = strndup(value,strlen(value) - 1);
                    arr_len++;
                }
            }
        }
    }

    fclose(conf_file);
    free(line);

    *arrc = arr_len;
    return arr;
}

static char config_getopt(char c, char** optarg, int arrc, struct ConfigArray** arr) {
    // If the argument wasn't found, check config file
    if (c == -1) {
        char new_c = -1;
        for (int x = 0; x < arrc; x++) {
            if (!arr[x]->used) {
                new_c = arr[x]->key;
                *optarg = arr[x]->optarg;
                arr[x]->used = true;
            }
        }
        return new_c;
    } //...Else c was found...
    // Mark c as found so we don't replace it.
    for (int x = 0; x < arrc; x++) {
        if (arr[x]->key == c) {
            arr[x]->used = true;
        }
    }
    return c;
}

void config_clean() {
    for (int x = 0; x < g_conf_arr_len; x++) {
        free(g_conf_arr[x]->optarg);
        free(g_conf_arr[x]);
    }
    free(g_conf_arr);
}

void get_args(int argc, char** argv){
    int c;
    int optx = 0;
    int helpstat = EXIT_SUCCESS;
    bool help = false;
    bool user_format = false;
    char* nlines = NULL;;
    char* user_timeout = NULL;
    char* user_update_interval = NULL;

    struct option options[] = {
        {"help", no_argument, 0, 'h'}, // Make sure this is first!
        {"lines", required_argument, 0, 'l'},
        {"format", required_argument, 0, 'f'},
        {"dzen", no_argument, 0, 'd'},
        {"new", required_argument, 0, 'n'},
        {"pending", required_argument, 0, 'p'},
        {"no_pending", required_argument, 0, 'r'},
        {"update_interval", required_argument, 0, 'u'},
        {"timeout", required_argument, 0, 't'},
    };
    int optc = (sizeof(options) / sizeof(options[0]));

    // Check for conf file argument first
    char* conf_loc = NULL;
    for (int x = 0; x < argc; x++) {
        if ((!strcmp(argv[x],"-c"))
        &&  (argv[x + 1] != 0))
        {
            conf_loc = argv[x + 1];
            for (int y = x; x < (argc - 2); x++) {
                argv[y] = argv[y + 2];
            }
            argc -= 2;
            break;
        }
        if (!strncmp(argv[x],"--conf=",7)) {
            conf_loc = strpbrk(argv[x], "=") + 1;;
            for (int y = x; x < (argc - 1); x++) {
                argv[y] = argv[y + 1];
            }
            argc -= 1;
            break;
        }
    }

    g_conf_arr_len = 0;
    g_conf_arr = config_parse(&g_conf_arr_len, options, optc, conf_loc);

    while (c = getopt_long(argc, argv, "hl:dn:p:r:u:t:f:", options, &optx)) {
        if ((c = config_getopt(c,&optarg,g_conf_arr_len,g_conf_arr)) == -1) {
            break;
        }
        switch (c) {
            case 't':
                user_timeout = optarg;
                break;
            case 'f':
                user_format = true;
                g_default_format = optarg;
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

    if (!make_format(g_default_format)) {
        fprintf(stderr, "Error: \'-f\', could not read user format.\n");
        format_clean();
        exit(EXIT_FAILURE);
    }
}
