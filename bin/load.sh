#!/bin/bash

cd /root/stheno
export LD_LIBRARY_PATH=.
ulimit -c unlimited
#./isolate_irq.sh
echo $$ > /cgroup/cpu/BE/tasks
#echo $$ > /cgroup/cpu_sched/BE/tasks

./load $@


