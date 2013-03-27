#! /bin/sh

# This script sticks a little, bitty, button on
# your screen. When you get notificationsm it
# pops down a dzen slave window with the pending
# notifications.
#
# You can dismiss notifications just by hovering
# over the window.
#
# If you want to see your notifications after youve
# dismissed them, hover over the button to pop the
# list back down. If you have pending notifications
# that you've dismissed, the button will show a '!'
# by default, otherwise it will display nothing.



### Dzen-Specific Settings #################

# Font for dzen
font="-*-fixed-medium-*-*-*-12-*-*-*-*-*-*-*"

# The bg of the button when you have
#   no pending notifications, as well
#   as the border of the slave window
dzenbg=black

# The color of slave window text
dzenfg=white

# The bg of the slave window text, as
#   well as the bg of the button when
#   you have pending notifications
activebgcolor=darkred

# What the notification icon looks like
pending="^bg($activebgcolor) ! ^bg()"

# The position of the button, relative to The
#   top-left corner of it
x=2540
y=48

# The size of the main button
buttonwidth=20

# The width of the slave window
windowwidth=400

# Play around with this, its unpredicatable
#   when the slave window is larger than the
#   main window
windowalignment=l

# The size of each line of dzen
line_height=16



### SiND-Specific Settings ##################

# How often to fetch for notifications
updateinterval=500

# When notifications, by default, expire (in ms)
timeout=10000


### General Settings
# How many lines are in the slave window
lines=3

# The format of each notification (see sind -h)
userformat='%i : %a%A%s%B%b'
app_s=' : '
body_s=' : '

userstatus="%P
%N"
new_s='^uncollapse()'"$border"
pending_s="^tw()$pending"
no_pending_s="^tw()
^collapse()"

### Generate some vars ######################
format=\
'^fg('"$activebgcolor"')^p(+2)^r('\
"$(( $windowwidth - 2 ))x$(( $line_height - 8 ))"')'\
'^p(_LEFT)'\
'^p(+4;)^ib(1)^p()^fg()'"$userformat"

pending='^bg('"$activebgcolor"')^fg('"$notify_color"') '\
"$notify_icon"' 
^fg()^bg()'



### Execute! ################################
sind -l $(( $lines + 1 )) \
--app_s="$app_s" --body_s="$body_s" --new_s="$new_s" \
--pending_s="$pending_s" --no_pending_s="$no_pending_s" \
-d -f "$format" -s "$userstatus" \
-u "$updateinterval" -t "$timeout" \
| dzen2 \
-l "$lines" -h "$line_height" \
-x "$x" -y "$y" -sa "$windowalignment" \
-tw "$buttonwidth" -w "$windowwidth" \
-bg "$dzenbg" -fg "$dzenfg" -fn "$font"
