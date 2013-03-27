#include <stdlib.h>
#include <string.h>

#include "format.h"

/* Defaults */
FormatContainer format_container = { 0, 0, 0, 0 };
FormatContainer* g_format_container = &format_container;
FormatContainer status_container = { 0, 0, 0, 0 };
FormatContainer* g_status_container = &status_container;

/* Functions */
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

static void make_seperator(char* needle, char* tail, FormatContainer* format) {
    if (needle != tail) {
        format->array[format->len] = malloc(sizeof(FormatContainer));
        format->array[format->len]->is_seperator = true;
        format->array[format->len]->content.seperator =
            strndup(tail, needle - tail);
        format->len++;
    }
}

#define SPECASE(X,Z) \
case X : \
    if (!( format->has & Z )) { \
        format->array[format->len]->content.specifier = Z ; \
        format->has |= Z; \
    } else { \
        return false; \
    } \
    break;

bool make_format(char* string, FormatContainer* format) {
    int count = count_specs(string);
    format->min_size = sizeof(string[0]) * strlen(string);
    format->array = malloc(sizeof(OutputFormat*) * count);

    char* needle = string;
    char* tail = needle;

    // Populate format array
    while (*needle) {
        if (*needle == '%') {
            if ((*needle) + 1) {
                // Copy everything preceding as the seperator
                make_seperator(needle, tail, format);

                // Create format specifier element
                format->array[format->len] = malloc(sizeof(*format->array[0]));;
                format->array[format->len]->is_seperator = false;
                needle++;
                switch (*needle) {
                    SPECASE('a',FORM_LINE_APP)
                    SPECASE('s',FORM_LINE_SUM)
                    SPECASE('b',FORM_LINE_BOD)
                    SPECASE('i',FORM_LINE_ID)
                    SPECASE('A',FORM_LINE_IF_APP)
                    SPECASE('B',FORM_LINE_IF_BOD)

                    SPECASE('N',FORM_STAT_NEW)
                    SPECASE('P',FORM_STAT_PEND)
                    SPECASE('c',FORM_STAT_NUM)
                    default:
                        free(format->array[format->len]);
                        return false;
                }
                format->len++;
                needle++;
                tail = needle;
            } else {
                break;
            }
            continue;
        }
        needle++;
    }
    make_seperator(needle, tail, format);
    if ((   (format == g_format_container)
            &&  (format->has & FORM_STAT_NEW & FORM_STAT_PEND & FORM_STAT_NUM))
        ||  (format->has & FORM_LINE_APP & FORM_LINE_SUM & FORM_LINE_BOD
            & FORM_LINE_ID & FORM_LINE_IF_APP & FORM_LINE_IF_BOD) ) {
        return false;
    }
    return true;
}
#undef SPECASE

void format_clean(FormatContainer* format) {
    for (int x = 0; x < format->len; x++) {
        if (format->array[x]->is_seperator) {
            free(format->array[x]->content.seperator);
        }
        free(format->array[x]);
    }
    free(format->array);
}
