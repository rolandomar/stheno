#!/bin/bash

cd /root/stheno
export LD_LIBRARY_PATH=.
ulimit -c unlimited
./monitor $@

