#include <stdbool.h>

#include <dbus/dbus.h>

#ifndef __SIND_DBUS_H__
#define __SIND_DBUS_H__
bool handle_message(DBusMessage* msg, DBusConnection* connection);
DBusConnection* setup_debus();
#endif
