#include <dbus/dbus.h>

#ifndef __SIND_LIST_H__
#define __SIND_LIST_H__

// The structure for all pending messages
struct _NotifyLine;
typedef struct _NotifyLine {
    char* line;                 // What the line says
    unsigned long expires;      // The expiration date of the line
    dbus_uint32_t nid;          // The ID of the notification
    struct _NotifyLine* next;   // The next line on the list
    bool deleted;               // True if deleted via NotificationClosed
} NotifyLine;

// The container for the list
struct ListHead {
    bool changed;               // True if we should flush the output
    bool new;                   // True if we have new notifications
    NotifyLine* head;           // The start of the list
};

// Adds a line to the list
bool list_append(const char* app, const char* sum, const char* body, dbus_int32_t expires, dbus_uint32_t nid);
// Checks for zombies/removes them/prints if new/deleted lines
void list_walk();
// Free whole list
void list_destroy();
// Search for matching nid, if it exists, update that line
bool list_update(const char* app, const char* sum, const char* body, dbus_uint32_t nid, bool to_delete);

#endif
