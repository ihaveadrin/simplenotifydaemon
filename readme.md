Simple Notification Daemon
==========================
Prints freedesktop notifications to stdout

Can print notifications in a fix terminal, like an ncurses program.
Can be piped to dzen2 for popdown notifications (see dzen.sh).
Or can just print to stdout plainly.

It relies only on libdbus (almost up to freedesktop spec), libc
and a handful of POSIX functions.

See its impact below:

```
(generated on first commit):
http://cloc.sourceforge.net v 1.56  T=0.5 s (18.0 files/s, 1160.0 lines/s)
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                4             55             35            379
C/C++ Header                     3             11              7             38
Bourne Shell                     1              6              0             26
make                             1              7              0             16
-------------------------------------------------------------------------------
SUM:                             9             79             42            459
-------------------------------------------------------------------------------
```


Building & Usage
----------------
Just `make` and run the executable! There is no `make install` rule yet...


Options
-------
	-l, --lines=		    If set to $LINES, will give an curses-like output in your terminal.
	-s, --seperator=	    The default seperates APP SUMMARY & BODY with ' : '. Change this.
	-n, --new=		        The provided string will be printed after all notifications if there was a new notification added to the queue on this update
	-p, --pending=		    The provided string will be printed before all notifications if there are any notifications on the queue.
	-r, --no_pending=	    The provided string will be printed before all notifications if there are no notifications on the queue.
	-d, --dzen:		        Will subtract two lines ('-p' or '-r' and '-n') from the total number of lines provided by '-l', as to keep notifications in the dzen slave window. (If '-pr' or '-n' are included, of course)
	-u, --update_interval=	Set default update interval for fetching pending notifications, and checking the list for expirations, in milliseconds. Lower will make the list seem more responsive, but will eat more CPU. Defaults to 500ms.
	-t, --timeout=		    Set default notification expiration in milliseconds. Defaults to 5 seconds (5000ms).


License
-------
This software is release under an MIT license.

Copyright (c) 2013 Tyler Thomas Hart

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
