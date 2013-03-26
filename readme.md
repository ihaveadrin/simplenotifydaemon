SiND -- Simple Notification Daemon
==================================
Prints freedesktop notifications to stdout

Can print notifications in a fixed terminal, like an ncurses program.
Can be piped to dzen2 for popdown notifications (see dzen.sh).
Or can just print to stdout plainly.

It relies only on libdbus and gcc to build.

Building & Usage
----------------
Building:

    $ make && sudo make install

Usage:

    $ sind [OPTIONS]

Pretty easy!

Options
-------
```
Options:
	-h, --help		        Show this message
	-c, --conf=		        Read settings from alternate config file
	-l, --lines=		    If set to $LINES, will give an curses-like
                            output in your terminal.
	-f, --format=		    The format to print each notification. see
                            FORMAT
	-n, --new=		        The provided string will be printed after
                            all notifications if there was a new
                            notification added to the queue on this update
	-p, --pending=		    The provided string will be printed before
                            all notifications if there are any
                            notifications on the queue.
	-r, --no_pending=	    The provided string will be printed before
                            all notifications if there are no
                            notifications on the queue.
	-d, --dzen		        Will subtract two lines ('-p' or '-r' and
                            '-n') from the total number of lines provided
                            by '-l', as to keep notifications in the dzen
                            slave window. (If '-pr' or '-n' are included,
                            of course)
	-u, --update_interval=  Set default update interval for fetching
                            pending notifications, and checking the list
                            for expirations, in milliseconds. Lower will
                            make the list seem more responsive, but will
                            eat more CPU. Defaults to 500ms.
	-t, --timeout=		    Set default notification expiration in
                            milliseconds. Defaults to 5 seconds (5000ms).

Configuration File:
	SiND will look in ~/.sindrc (or the location specified by --conf) for
a configuration file. These provide default values for the above arguments
if they are not included when SiND is executed. The configuration file can
consist of [KEY]=[VALUE] pairs, with KEY cooresponding to the longoptions
above, seperated by newlines. It can also consist of comments, if the line
begins with '#', and empty lines (with no other whitespace)

Format:
	When used with the -f --format= option, you can control the output of
    each line with a date-like format string

	These are the current interpreted sequences:
		%a	the name of the application sending the notification
		%s	the summary of the notification
		%b	the body of the notification
		%i	the id of the notification
```

License
-------
This software is release under an MIT license.

Copyright (c) 2013 Tyler Thomas Hart

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

