#!/bin/sh
echo "show text debug data with grep"
tail -f /tmp/kst_log.txt | grep $1
#EOF
