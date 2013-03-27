#! /bin/sh

tmp_file=/tmp/sindconky

sind -s "FLUSH%c Notifications: %N%P" -S \
--new_s='[new] ' --pending_s='' \
| while read; do
    if [[ "$REPLY" == "FLUSH"* ]]; then
        echo "${REPLY:5}" > $tmp_file
    else
        echo "$REPLY" >> $tmp_file
    fi
done
