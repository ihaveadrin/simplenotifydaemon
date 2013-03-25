#include <stdio.h>
#include <stdbool.h>

#include "dbus.h"
#include "list.h"

static char* g_server_info[] = {"SiND", "Tylo", "0.1", "1.2", NULL};
static char* g_capabilities[] = {"body", NULL};
static dbus_uint32_t g_index = 0;
DBusConnection* g_conn = NULL;

static bool notify(DBusMessage* msg) {
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
            case 7: to_fill = &expires; break;
            default: to_fill = NULL; break;
        }
        if (to_fill) {
            dbus_message_iter_get_basic(&args, to_fill);
        }
        dbus_message_iter_next( &args );
    }

    if (nid == 0) {
        g_index++;
        if (!list_append(appname, summary, body, expires, g_index)) {
            return false;
        }
        nid = g_index;
    } else list_update(appname, summary, body, nid, false);

    // Send reply
    DBusMessage* reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &args);
    if (dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &nid)) {
        dbus_connection_send(g_conn, reply, &g_index);
    }
    dbus_message_unref(reply);
    return true;
}

static void dbus_array_reply(DBusMessage* msg, char* array[]) {
    DBusMessage* reply = dbus_message_new_method_return(msg);
    DBusMessageIter args;

    bool success = true;
    dbus_message_iter_init_append(reply, &args);
    for (int i = 0; array[i] != 0; i++) {
        if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &array[i])) {
            success = false;
        }
    }
    if (success && dbus_connection_send(g_conn, reply, &g_index)) {
        dbus_message_unref(reply);
    }
}

static void getserverinfo(DBusMessage* msg) {
    g_index++;
    dbus_array_reply(msg, g_server_info);
}

static void getcapabilities(DBusMessage* msg) {
    g_index++;
    dbus_array_reply(msg, g_capabilities);
}

static void closenotification(DBusMessage* msg) {
    DBusError* geterror = NULL;
    dbus_uint32_t nid;
    if (!dbus_message_get_args(msg, geterror,
        DBUS_TYPE_UINT32, &nid,
        DBUS_TYPE_INVALID)
    ) {
        if (dbus_error_is_set(geterror)) {
            dbus_error_free(geterror);
        }
        return;
    }
    if (!list_update(NULL,NULL,NULL,nid,true)) {
        DBusMessage* reply = dbus_message_new_error(msg,
        "org.freedesktop.Notifications.NotificationClosed.Error", 0);
        if (dbus_connection_send(g_conn, reply, NULL)) {
            dbus_message_unref(reply);
        }
    }
}

bool handle_message(DBusMessage* msg) {
    if (
        dbus_message_is_method_call(msg,
        "org.freedesktop.Notifications", "Notify")
    ) {
        if (!notify(msg)) {
            return false;
        }
    } else if (
        dbus_message_is_method_call(msg,
        "org.freedesktop.Notifications", "GetServerInformation")
    ) {
        getserverinfo(msg);
    } else if (
        dbus_message_is_method_call(msg,
        "org.freedesktop.Notifications", "GetCapabilities")
    ) {
        getcapabilities(msg);
    } else if (
        dbus_message_is_method_call(msg,
        "org.freedesktop.Notifications", "CloseNotification")
    ) {
        closenotification(msg);
    }

    dbus_message_unref(msg);
    dbus_connection_flush(g_conn);

    return true;
}

void signal_notificationclose(dbus_uint32_t nid, dbus_uint32_t reason) {
    DBusMessage* signal = dbus_message_new_signal(
        "/org/freedesktop/Notifications/NotificationClosed",
        "org.freedesktop.Notifications.NotificationClosed",
        "NotificationClosed");
    if ((dbus_message_append_args(signal,
        DBUS_TYPE_UINT32, &nid,
        DBUS_TYPE_UINT32, &reason,
        DBUS_TYPE_INVALID))
    &&  (dbus_connection_send(g_conn, signal, NULL))) {
        dbus_message_unref(signal);
    }
}

bool setup_debus() {
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
        return false;
    }
    if (!connection) {
        return false;
    }

    // Try to register as notification daemon
    int server_success =  dbus_bus_request_name(connection,
        "org.freedesktop.Notifications", DBUS_NAME_FLAG_REPLACE_EXISTING, err);
    if (dbus_error_is_set(err)) {
        fprintf(stderr, "%s\n", err->message);
        dbus_error_free(err);
        return false;
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != server_success) {
        fprintf(stderr, "There is a notification daemon already running!");
        return false;
    }

    dbus_error_free(err);
    g_conn = connection;
    return true;
}
