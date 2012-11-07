PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "irq" | awk '{ print $1; }' | xargs echo)
echo $PIDs
for i in $PIDs
do
    echo "Changing pid $i"
    ret=$(chrt -f -p 99 $i)
done

PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "cpuset" | awk '{ print $1; }' | xargs echo)
echo $PIDs
for i in $PIDs
do
    echo "Changing pid $i"
    ret=$(chrt -f -p 99 $i)
done