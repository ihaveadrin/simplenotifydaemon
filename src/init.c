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
char* g_default_status = "";
char* g_default_format = "%a%A%s%B%b";
bool g_status_top = false;
struct _FormatStrings g_dformat = {
    .new = "NEW ",
    .no_new = "",
    .pending = "PENDING ",
    .no_pending = "",
    .app = " : ",
    .no_app = "",
    .body = " = ",
    .no_body = "",
};

// Functions
static void print_help(char* path) {
    fprintf(stdout,
"Simple Notification Daemon\n"
"Usage:\t%s [OPTIONS]\n"
"Prints freedesktop notifications to stdout\n"
"\n"
"General Options:\n"
"\t-h, --help\n"
"\t\tShow this message\n"
"\t-c, --conf=\n"
"\t\tRead settings from alternate config file\n"
"\t-d, --dzen\n"
"\t\tWill subtract a line from the total number of lines provided by\n"
"\t\t\'--lines=\', as to keep notifications in the dzen slave window, if\n"
"\t\tyou include the status bar (see FORMAT)\n"
"\t-l, --lines=\n"
"\t\tIf set to $LINES, will give an curses-like output in your terminal.\n"
"\t\tThe format to print each notification. See FORMAT\n"
"\t-t, --timeout=\n"
"\t\tSet default notification expiration in milliseconds. Defaults to 5\n"
"\t\tseconds (5000ms).\n"
"\t-u, --update_interval=\n"
"\t\tSet default update interval for fetching pending notifications, and\n"
"\t\tchecking the list for expirations, in milliseconds. Lower will make\n"
"\t\tthe list seem more responsive, but will eat more CPU. Defaults to\n"
"\t\t500ms.\n"
"\n"
"Format Options:\n"
"\t-f, --format=\n"
"\t\tUsing the -f or --format= option, you can control the output of\n"
"\t\teach line with a date-like format string\n"
"\n"
"\t\tThese options refer to line-format sequences below:\n"
"\t--app_s=\n"
"\t--no_app_s=\n"
"\t--body_s=\n"
"\t--no_body_s=\n"
"\t\tThese are the current line-format sequences:\n"
"\t\t\t%%a\tthe name of the application sending the notification\n"
"\t\t\t%%s\tthe summary of the notification\n"
"\t\t\t%%b\tthe body of the notification\n"
"\t\t\t%%i\tthe id of the notification\n"
"\t\t\t%%A\tif the notification had an APP field, this cooresponds\n"
"\t\t\t\tto --app_s, otherwise --no_app_s\n"
"\t\t\t%%B\tif the notification had a BODY field, this cooresponds\n"
"\t\t\t\tto --body_s, otherwise --no_body_s\n"
"\n"
"\t-s, --status=\n"
"\t\tUsing the -f or --status= option, you can make an extra line\n"
"\t\tcontaining general information about the current notifications\n"
"\t\teither above or below the notification list:\n"
"\t--S, --status_top\n"
"\t\tWill move the status bar from the bottom (default) to the top.\n"
"\n"
"\t\tThese options refer to status-format sequences below:\n"
"\t--new_s=\n"
"\t--no_new_s=\n"
"\t--pending_s=\n"
"\t--no_pending_s=\n"
"\t\tThese are the current status-format sequences:\n"
"\t\t\t%%c\tThe current count of active notifications\n"
"\t\t\t%%N\tif there was a new notification on this update,\n"
"\t\t\t\tthis cooresponds to --new_s, otherwise --no_new_s\n"
"\t\t\t%%P\tif there were pending notification on this update,\n"
"\t\t\t\tthis cooresponds to --pending_s, otherwise\n"
"\t\t\t\t--no_pending_s\n"
"\t\tNote, if --status= is empty or unset, no status bar will be displayed\n"
"\n"
"Configuration File:\n"
"\tSiND will look in ~/.sindrc (or the location specified by --conf) for a\n"
"\tconfiguration file. These provide default values for the above arguments\n"
"\tif they are not included when SiND is executed. The configuration file\n"
"\tcan consist of [KEY]=[VALUE] pairs, with KEY cooresponding to the\n"
"\tlongoptions above, seperated by newlines. It can also consist of comments,\n"
"\tif the line begins with \'#\', and empty lines (with no other whitespace)\n"
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

static struct ConfigArray** config_parse(int* arrc, struct option* options, char* conf_loc) {
    /*Setup File*/
    FILE* conf_file = NULL;
    if (conf_loc) {
        if (*conf_loc == '~') {
            chdir(getenv("HOME")); conf_loc++;
            if (*conf_loc == '/') conf_loc++;
        }
        conf_file = fopen(conf_loc, "r");
    }
    if (!conf_file) {
        chdir(getenv("HOME")); conf_file = fopen(".sindrc", "r");
    }
    if (!conf_file) return NULL;

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
        for (int x = 1; options[x].name != 0; x++) {
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
                break;
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
                break;
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
    bool user_status = false;

    char* nlines = NULL;;
    char* user_timeout = NULL;
    char* user_update_interval = NULL;

    struct option options[] = {
        /* Flags */
        {"help",            no_argument,        0, 'h'}, // Make sure this is first!
        {"dzen",            no_argument,        0, 'd'},
        {"status_top",      no_argument,        0, 'S'}, // Make sure this appears before 'status'
        /* Options with short options */
        {"lines",           required_argument,  0, 'l'},
        {"format",          required_argument,  0, 'f'},
        {"status",          required_argument,  0, 's'},
        {"update_interval", required_argument,  0, 'u'},
        {"timeout",         required_argument,  0, 't'},
        /* Options with only long options */
        {"new_s",           required_argument,  0, 1},
        {"no_new_s",        required_argument,  0, 11},
        {"pending_s",       required_argument,  0, 2},
        {"no_pending_s",    required_argument,  0, 12},
        {"app_s",           required_argument,  0, 3},
        {"no_app_s",        required_argument,  0, 13},
        {"body_s",          required_argument,  0, 4},
        {"no_body_s",       required_argument,  0, 14},
        /* Term */
        { 0 , 0 , 0 , 0 },
    };

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

    /* Parse config file */
    g_conf_arr_len = 0;
    g_conf_arr = config_parse(&g_conf_arr_len, options, conf_loc);

    /* Get arguments */
    while (c = getopt_long(argc, argv, "hdSl:f:s:u:t:", options, &optx)) {
        if ((c = config_getopt(c,&optarg,g_conf_arr_len,g_conf_arr)) == -1) {
            break;
        }
        switch (c) {
            /* Flags */
            case 'h': help = true; break;
            case 'd': if (!(optarg && !strcmp(optarg,"false"))) g_dzen = true; break;
            case 'S': if (!(optarg && !strcmp(optarg,"false"))) g_status_top = true; break;
            /* Short options */
            case 'l': nlines = optarg; break;
            case 'f':
                user_format = true;
                g_default_format = optarg;
                break;
            case 's':
                user_status = true;
                g_default_status = optarg;
                break;
            case 'u': user_update_interval = optarg; break;
            case 't': user_timeout = optarg; break;
            /* Long Options only */
            case 1: g_dformat.new = optarg; break;
            case 11: g_dformat.no_new = optarg; break;
            case 2: g_dformat.pending = optarg; break;
            case 12: g_dformat.no_pending = optarg; break;
            case 3:  g_dformat.app = optarg; break;
            case 13: g_dformat.no_app = optarg; break;
            case 4: g_dformat.body = optarg; break;
            case 14: g_dformat.no_body = optarg; break;

            default:
                help = true;
                helpstat = EXIT_FAILURE;
                break;
        }
    }

    /* Help! */
    if (help) {
        print_help(argv[0]);
        exit(helpstat);
    }

    /* Parse integer inputs */
    if (user_timeout) {
        long tmp_time = strtol(user_timeout, NULL, 10);
        if (tmp_time <= 0) {
            fprintf(stderr, "Error: \'--timeout=%s\' argument must be greater than 0.\n", user_timeout);
            exit(EXIT_FAILURE);
        }
        g_sleep_time = (unsigned long)tmp_time;
    }
    if (nlines) {
        long tmp_lines = strtol(nlines, NULL, 10);
        if (tmp_lines <= 0) {
            fprintf(stderr, "Error: \'--lines=%s\' argument must be greater than 0.\n", nlines);
            exit(EXIT_FAILURE);
        }
        g_lines = (int)tmp_lines;
        for (int x = 0; x <= g_lines; x++) {
            fprintf(stdout,"\n");
        }
    }
    if (user_update_interval) {
        long tmp_ui = strtol(user_update_interval, NULL, 10);
        if (tmp_ui <= 0) {
            fprintf(stderr, "Error: \'--update_interval=%s\' argument must be greater than 0.\n", user_update_interval);
            exit(EXIT_FAILURE);
        }
        g_update_interval.tv_sec = ((unsigned long)tmp_ui) / 1000 ;
        g_update_interval.tv_nsec = (((unsigned long)tmp_ui) % 1000 ) * 1000000 ;
    }

    /* Make format arrays */
    if (!make_format(g_default_format, g_format_container)) {
        fprintf(stderr, "Error: \'--format=%s\', could not read user format.\n", g_default_format);
        format_clean(g_format_container);
        exit(EXIT_FAILURE);
    }
    if (!make_format(g_default_status, g_status_container)) {
        fprintf(stderr, "Error: \'--status=%s\', could not read user format.\n", g_default_status);
        format_clean(g_status_container);
        exit(EXIT_FAILURE);
    }
    fflush(stderr);
    fflush(stdout);
}
