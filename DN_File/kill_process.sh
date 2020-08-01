#!/bin/sh
pids=$(pidof -s $1)
echo $pids
kill -9 $pids

# EOF
