#include <stdlib.h>
#include <string.h>

#include "format.h"

char* g_default_format = "%a : %s : %b";

FormatContainer format_container = { 0, 0, 0, false, false, false };
FormatContainer* g_format_container = &format_container;

static int count_specs(char* string) {
    // Count specifiers
    int count = 1;
    char* needle = string;
    while (*needle) {
        if (*needle == '%') {
            count++;
            if ((needle != string)
            && (*(needle + 1))
            && (*(needle + 2))) {
                // If it was between stuff
                count++;
            }
        }
       needle++;
    }
    return count;
}

static void make_seperator(char* needle, char* tail) {
    if (needle != tail) {
        g_format_container->array[g_format_container->len] = malloc(sizeof(FormatContainer));
        g_format_container->array[g_format_container->len]->is_seperator = true;
        g_format_container->array[g_format_container->len]->content.seperator = malloc(sizeof(*tail) * (needle - tail + 1));
        g_format_container->array[g_format_container->len]->content.seperator =
            strncpy(g_format_container->array[g_format_container->len]->content.seperator, tail, needle - tail);
        g_format_container->len++;
    }
}

#define SPECASE(X,Y,Z) \
case X : \
    if (!( Y )) { \
        g_format_container->array[g_format_container->len]->content.specifier = Z ; \
        Y = true; \
    } else { \
        return false; \
    } \
    break;

bool make_format(char* string) {
    int count = count_specs(string); 
    g_format_container->min_size = sizeof(string[0]) * strlen(string);
    g_format_container->array = malloc(sizeof(OutputFormat*) * count); 

    char* needle = string;
    char* tail = needle;

    // Populate format array
    while (*needle) {
        if (*needle == '%') {
            if ((*needle) + 1) {
                // Copy everything preceding as the seperator
                make_seperator(needle, tail);

                // Create format specifier element
                g_format_container->array[g_format_container->len] = malloc(sizeof(*g_format_container->array[0]));;
                g_format_container->array[g_format_container->len]->is_seperator = false;
                needle++;
                switch (*needle) {
                    SPECASE('a',g_format_container->app,SEP_APP)
                    SPECASE('s',g_format_container->sum,SEP_SUMMARY)
                    SPECASE('b',g_format_container->bod,SEP_BODY)
                    default:
                        free(g_format_container->array[g_format_container->len]);
                        return false;
                }
                g_format_container->len++;
                needle++;
                tail = needle;
            } else {
                break;
            }
            continue;
        }
        needle++;
    }
    make_seperator(needle, tail);
    return true;
}
#undef SPECASE

void format_clean() {
    for (int x = 0; x < g_format_container->len; x++) {
        if (g_format_container->array[x]->is_seperator) {
            free(g_format_container->array[x]->content.seperator);
        }
        free(g_format_container->array[x]);
    }
    free(g_format_container->array);
}
