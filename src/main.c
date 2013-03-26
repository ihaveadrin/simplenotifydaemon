#include <stdlib.h>
#include <signal.h>

#include "dbus.h"
#include "list.h"
#include "init.h"
#include "format.h"

/* For exiting cleanly */
static int g_do_main_loop = 1;
static void die(int signum) {
    g_do_main_loop = 0;
}

/* Main */
void get_args(int argc, char** argv);
int main (int argc, char** argv) {
    get_args(argc, argv);

    // Catch signals
    signal(SIGTERM, &die);
    signal(SIGINT, &die);
    signal(SIGQUIT, &die);

    // Set up dbus connection
    if (!setup_debus()) {
        return EXIT_FAILURE;
    }

    // Wait for messages
    int success = EXIT_SUCCESS;
    DBusMessage* msg;
    while (g_do_main_loop) {
        // Check list for needed updates
        list_walk();

        // Get messages
        dbus_connection_read_write(g_conn, 0); //Non-blocking
        msg = dbus_connection_pop_message(g_conn); //Fetch messages

        if (!msg) {
            // If we haven't gotten any messages, sleep.
            nanosleep(&g_update_interval, NULL);
            continue;
        }

        if (!handle_message(msg)) {
            // Otherwise, handle them
            success = EXIT_FAILURE;
            break;
        }
    }

    /* // Unref dbus stuff
     * To my understanding, libdbus will not unref to 0 and free a
     * shared connection, so g_conn, which is alloced and contains
     * other mapped memory by dbus_bus_get(), will leave references
     * to this memory to be returned when the program terminates.
     * Currently, g_conn references 67 blocks, according to valgrind.
     *
     * Since g_conn should be static following dbus_init(), anything
     * beyond 67 blocks should be checked for heap exhaustion. The
     * unref is still left, though, in case other circumstances arise.
     */
    dbus_connection_unref(g_conn);

    // Free alloced memory
    list_destroy();
    format_clean();
    config_clean();

    // Free list
    return success;
}
