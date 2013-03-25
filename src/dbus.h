#include <stdbool.h>

#include <dbus/dbus.h>

#ifndef __SIND_DBUS_H__
#define __SIND_DBUS_H__
extern DBusConnection* g_conn;

bool handle_message(DBusMessage* msg);
bool setup_debus();
void signal_notificationclose(dbus_uint32_t nid, dbus_uint32_t reason);
#endif
