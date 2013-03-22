#! /bin/sh

font="-*-fixed-medium-*-*-*-12-*-*-*-*-*-*-*"
dzenbg=black
dzenfg=white

x=2540
y=48

seperator=' === '
pending='^fg(red)!
^fg'"$dzenfg"
buttonwidth=20
windowwidth=1280
windowalignment=l

updateinterval=500
timeout=10000

line_height=16
lines=2

./sind -l $(( $lines + 1 )) -n '^uncollapse()' -p "^tw()$pending" \
-d -s "$seperator" \
-r "^tw()
^collapse()" \
-u "$updateinterval" -t "$timeout" \
| dzen2 \
-l "$lines" -h "$line_height" \
-x "$x" -y "$y" -sa "$windowalignment" \
-tw "$buttonwidth" -w "$windowwidth" \
-bg "$dzenbg" -fg "$dzenfg" -fn "$font"
