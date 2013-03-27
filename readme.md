SiND -- Simple Notification Daemon
==================================
Prints freedesktop notifications to stdout

Keeps a queue of current notifications in a list, and simply prints them
out to stdout in the format you describe. It prints the whole list whenever
anything is updated, and can keep the list with in a specified number of lines
to give an ncurses like feel.

You can also add a special status bar that displays information such as the
current number of notifications as well custom text if you have new ones.

SiND is intended to either work as a simple console-based notification center, or
be piped into other simple programs like dzen:
https://github.com/robm/dzen. See ./dzen.sh for an example.

Currently, SiND is nearly fully freedesktop compliant https://developer.gnome.org/notification-spec/. What it is
missing: it does not touch html tags, but passes them through literally. This is intentional: it allows you to
filter OR display them in your end-of-pipe process by parsing them in the pipeline.

SiND relies only on libdbus and gcc to build, is small, but easily customizable. Feel free to ask questions
or contribute.

Building & Usage
----------------
Building:

    $ make && sudo make install

Usage:

    $ sind [OPTIONS]

Pretty easy!

Options & Configuring
---------------------

All options are configurable as arguments, as well as specified in a configuration file. In the event that an
option appears in both, the argument takes precedence. Configuration follows the familar .ini style, of
[key]=[value] pairs, using '#''s as comments. All [key]'s correspond to the long options provided on the
command line. 

SiND will first look in --conf= for a config file, then try $HOME/.sindrc.

**General Options:**
* -h, --help
	* Show this message
* -c, --conf=
	* Read settings from alternate config file
* -d, --dzen
	* Will subtract a line from the total number of lines provided by
	'--lines=', as to keep notifications in the dzen slave window, if
	you include the status bar (see FORMAT)
* -l, --lines=
	* Restricts the number of lines to print at a time. If set to $LINES,
	  will give an curses-like output in your terminal.
* -t, --timeout=
	* Set default notification expiration in milliseconds. Defaults to 5
	seconds (5000ms).
* -u, --update_interval=
	* Set default update interval for fetching pending notifications, and
	checking the list for expirations, in milliseconds. Lower will make
	the list seem more responsive, but will eat more CPU. Defaults to
	500ms.

**Line-Format Options:**
* -f, --format=
	* Using the -f or --format= option, you can control the output of
	each line with a date-like format string
	* These options refer to line-format sequences below:
* --app_s=
* --no_app_s=
* --body_s=
* --no_body_s=
	* These are the current line-format sequences:
		* %a
			* the name of the application sending the notification
		* %s
			* the summary of the notification
		* %b
			* the body of the notification
		* %i
			* the id of the notification
		* %A	
			* if the notification had an APP field, this cooresponds
			to --app_s, otherwise --no_app_s
		* %B	
			* if the notification had a BODY field, this cooresponds
			to --body_s, otherwise --no_body_s

**Status-Format Options:**
* -s, --status=
	* Using the -f or --status= option, you can make an extra line
	containing general information about the current notifications
	either above or below the notification list:
* --S, --status_top
	* Will move the status bar from the bottom (default) to the top.
	* These options refer to status-format sequences below:
* --new_s=
* --no_new_s=
* --pending_s=
* --no_pending_s=
	* These are the current status-format sequences:
		* %c
			* The current count of active notifications
		* %N
			* if there was a new notification on this update,
			this cooresponds to --new_s, otherwise --no_new_s
		* %P
			* if there were pending notification on this update,
			this cooresponds to --pending_s, otherwise
			--no_pending_s
	* Note, if --status= is empty or unset, no status bar will be displayed

License
-------
This software is release under an MIT license.

Copyright (c) 2013 Tyler Thomas Hart

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

