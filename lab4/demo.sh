#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer Jonathan &
./reader 172.20.10.5 70 /dev/mydev
