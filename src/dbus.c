#include <stdio.h>
#include <stdbool.h>

#include "dbus.h"
#include "list.h"

static char* g_server_info[4] = {"snd", "snd", "2013", "2013"};
static dbus_uint32_t g_index;

static bool notify(DBusMessage* msg, DBusConnection* conn) {
    // Get message contents
    DBusMessageIter args;
    const char *appname;
    const char *summary;
    const char *body;
    dbus_uint32_t nid=0;
    dbus_int32_t expires=-1;
    void* to_fill = NULL;

    dbus_message_iter_init(msg, &args);
    for (int x = 0; x < 8; x++) {
        switch (x) {
            case 0: to_fill = &appname; break;
            case 1: to_fill = &nid; break;
            case 3: to_fill = &summary; break;
            case 4: to_fill = &body; break;
            case 8: to_fill = &expires; break;
            default: to_fill = NULL; break;
        }
        if (to_fill) {
            dbus_message_iter_get_basic(&args, to_fill);
        }
        dbus_message_iter_next( &args );
    }

    if (!list_append(appname, summary, body, expires)) {
        return false;
    }

    // Send reply
    g_index++;
    DBusMessage* reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &nid)
    ||  !dbus_connection_send(conn, reply, &g_index)) {
        return true;
    }
    dbus_message_unref(reply);
    return true;
}

static void getserverinfo(DBusMessage* msg, DBusConnection* conn) {
    DBusMessage* reply = dbus_message_new_method_return(msg);
    DBusMessageIter args;

    g_index++;
    dbus_message_iter_init_append(reply, &args);
    if (dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &g_server_info[0])
    &&  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &g_server_info[1])
    &&  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &g_server_info[2])
    &&  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &g_server_info[3])
    &&  dbus_connection_send(conn, reply, &g_index)) {
        dbus_message_unref(reply);
    }
}

bool handle_message(DBusMessage* msg, DBusConnection* connection) {
    if (
    dbus_message_is_method_call(msg,
    "org.freedesktop.Notifications", "Notify")
    ) {
        if (!notify(msg, connection)) {
            return false;
        }
    } else if (
    dbus_message_is_method_call(msg,
    "org.freedesktop.Notifications", "GetServerInformation")
    ) {
        getserverinfo(msg, connection);
    }

    dbus_message_unref(msg);
    dbus_connection_flush(connection);

    return true;
}

DBusConnection* setup_debus() {
    DBusConnection* connection;
    // Set up error structure
    DBusError dbus_error;
    DBusError* err = &dbus_error;
    dbus_error_init(err);

    // Connect to session
    connection = dbus_bus_get(DBUS_BUS_SESSION, err);
    if (dbus_error_is_set(err)) {
        fprintf(stderr, "%s\n", err->message);
        dbus_error_free(err);
        return NULL;
    }
    if (!connection) {
        return NULL;
    }

    // Try to register as notification daemon
    int server_success =  dbus_bus_request_name(connection,
        "org.freedesktop.Notifications", DBUS_NAME_FLAG_REPLACE_EXISTING, err);
    if (dbus_error_is_set(err)) {
        fprintf(stderr, "%s\n", err->message);
        dbus_error_free(err);
        return NULL;
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != server_success) {
        fprintf(stderr, "There is a notification daemon already running!");
        return NULL;
    }

    dbus_error_free(err);

    return connection;
}
