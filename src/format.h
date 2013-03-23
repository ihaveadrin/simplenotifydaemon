#include <stdbool.h>

#ifndef __SIND_FORMAT_H__
#define __SIND_FORMAT_H__

enum {
    SEP_APP,
    SEP_SUMMARY,
    SEP_BODY,
};

typedef struct _OutputFormat {
    bool is_seperator;
    union {
        char* seperator;
        int specifier;
    } content;
} OutputFormat;

typedef struct _FormatContainer {
    int len;
    OutputFormat** array;
    size_t min_size;
    bool app;
    bool sum;
    bool bod;
} FormatContainer;

extern FormatContainer* g_format_container;
extern char* g_default_format;

bool make_format(char* string);
void format_clean();

#endif
