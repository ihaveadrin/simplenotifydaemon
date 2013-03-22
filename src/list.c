#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "list.h"
#include "init.h"

struct ListHead g_text = { .head = NULL, .changed = false };

// Get current time to check against expiration
unsigned long current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// Allocate new node
static NotifyLine* list_new(const char* app, const char* sum, const char* body, dbus_int32_t expires) {
    // New node
    NotifyLine* line = malloc(sizeof(NotifyLine));
    if (!line) return NULL;

    // New string. A little bit of padding, to be safe
    line->line = malloc(
        (sizeof(char) * strlen(app))
    +   (sizeof(char) * strlen(g_seperator))
    +   (sizeof(char) * strlen(sum))
    +   (sizeof(char) * strlen(g_seperator))
    +   (sizeof(char) * strlen(body))
    +   (sizeof(char) * strlen(g_seperator)));
    if (!line->line) return NULL;

    sprintf(line->line, "%s%s%s%s%s", app, g_seperator, sum,  g_seperator, body);

    // Insert expiration date
    if (expires > -1)  line->expires = (unsigned long)expires + current_time();
    else line->expires = current_time() + g_sleep_time;

    // Always appended
    line->next = NULL;

    return line;
}

// Appends a node
static void list_add(NotifyLine* line) {
    if (!g_text.head) {
        g_text.head = line;
        g_text.changed = true;
        return;
    }

    NotifyLine* current_line = g_text.head;
    while (current_line->next) {
        current_line = current_line->next;
    }

    current_line->next = line;
    return;
}

// Makes a new node and appends it
bool list_append(const char* app, const char* sum, const char* body, dbus_int32_t expires) {
    NotifyLine* line = list_new(app, sum, body, expires);
    if (!line) {
        return false;
    }
    list_add(line);
    g_text.changed = true;
    g_text.new = true;
    return true;
}

// Frees a node
static void list_free(NotifyLine* line) {
    free(line->line);
    free(line);
}

// Unlinks a node
static void list_remove(NotifyLine* line) {
    if (!line) {
        return;
    }
    if (line == g_text.head) {
        g_text.head = g_text.head->next;
    } else {
        NotifyLine* current_line = g_text.head;
        while (current_line->next != line) {
            current_line = current_line->next;
            if (!current_line) return;
        }
        current_line->next = current_line->next->next;
    }
    list_free(line);
    g_text.changed = true;
}

// Free whole list
void list_destroy() {
    while (g_text.head) {
        list_remove(g_text.head);
    }
}

// Checks for zombies/removes them/prints if new/deleted lines
void list_walk() {
    unsigned long time = current_time();
    NotifyLine* current_line = g_text.head;
    NotifyLine* to_chopping_block = NULL;

    // Find zombies
    while (current_line) {
        if (current_line->expires < time) {
            to_chopping_block = current_line;
            current_line = current_line->next;
            list_remove(to_chopping_block);
            continue;
        }
        current_line = current_line->next;
    }
    if (!g_text.changed) return;

    // Print stuff!
    // Adjust $LINES
    int for_inc = 1;
    if (!g_dzen && (*g_new_notify_string) && (g_text.new)) {
        fprintf(stdout, " \n");
    }
    if ((*g_pending_notify_string) && (g_text.head)) {
        fprintf(stdout, "%s\n", g_pending_notify_string);
        if (!g_dzen) for_inc += 1;
    }
    if ((*g_no_pending_notify_string) && (!g_text.head)) {
        fprintf(stdout, "%s\n", g_no_pending_notify_string);
        if (!g_dzen) for_inc += 1;
    }
    current_line = g_text.head;
    if (g_lines > 0) {
        // If using $LINES
        for (int x = 0; x < g_lines - for_inc; x++) {
            if (current_line) {
                // Print queue
                fprintf(stdout, "%s\n", current_line->line);
                current_line = current_line->next;
            } else {
                // Print blank lines
                fprintf(stdout, " \n");
            }
        }
    } else {
        // Else just print the group
        while (current_line) {
            fprintf(stdout, "%s\n", current_line->line);
            current_line = current_line->next;
        }
    }
    if ((*g_new_notify_string) && (g_text.new)) {
        fprintf(stdout, "%s%s",
        g_new_notify_string, (g_dzen) ? "\n" : "");
    }
    fflush(stdout);

    // Reset vars
    g_text.changed = false;
    g_text.new = false;
}
