#include <stdbool.h>
#include <inttypes.h>

#ifndef __SIND_FORMAT_H__
#define __SIND_FORMAT_H__

#define FORM_LINE_APP 1
#define FORM_LINE_SUM 2
#define FORM_LINE_BOD 4
#define FORM_LINE_ID 8
#define FORM_LINE_IF_APP 16
#define FORM_LINE_IF_BOD 32

#define FORM_STAT_NEW 64
#define FORM_STAT_PEND 128
#define FORM_STAT_NUM 256

typedef struct _OutputFormat {
    bool is_seperator;
    union {
        char* seperator;
        uint_fast16_t specifier;
    } content;
} OutputFormat;

typedef struct _FormatContainer {
    int len;
    OutputFormat** array;
    size_t min_size;
    uint_fast16_t has;
} FormatContainer;

extern FormatContainer* g_format_container;
extern FormatContainer* g_status_container;

bool make_format(char* string, FormatContainer* format);
void format_clean(FormatContainer* format);

#endif
