#!/bin/sh

(
flock -x 3
sleep 10
)3>/tmp/file_lock
