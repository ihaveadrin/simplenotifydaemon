#! /bin/sh

contents=""
pid=""

sind -s "FLUSH
" \
--new_s='[new] ' --pending_s='' \
| while read; do
    if [[ "$REPLY" == "FLUSH"* ]]; then
        kill $pid &> /dev/null
        echo "${contents:1}" \
            | osd_cat \
            -f '-*-*-bold-*-*-*-22-120-*-*-*-*-*-*' \
            -p bottom -A right -c white  \
            -O 1 -u black -d 10 &
        pid=$!
        contents=""
    else
        contents="$contents
$REPLY"
    fi
done

kill $pid
