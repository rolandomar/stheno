#!/bin/bash

#echo 183886080 > /proc/sys/net/core/rmem_default
#echo 183886080 > /proc/sys/net/core/rmem_max
#echo 183886080 > /proc/sys/net/core/wmem_default
#echo 183886080 > /proc/sys/net/core/wmem_max

#echo 315621376 > /proc/sys/net/core/rmem_default
#echo 315621376 > /proc/sys/net/core/rmem_max
#echo 315621376 > /proc/sys/net/core/wmem_default
#echo 315621376 > /proc/sys/net/core/wmem_max
     
#default value=124928
#/proc/sys/net/core/rmem_default 124928
#/proc/sys/net/core/rmem_max 131071
#/proc/sys/net/core/wmem_default 124928
#/proc/sys/net/core/wmem_max 131071

#/proc/sys/net/ipv4/tcp_low_latency 0

#echo 1310720 > /proc/sys/net/core/rmem_default
#echo 1310720 > /proc/sys/net/core/rmem_max
#echo 1310720 > /proc/sys/net/core/wmem_default
#echo 1310720 > /proc/sys/net/core/wmem_max


#Dont mess with the buffers for now
#echo 131072 > /proc/sys/net/core/rmem_default
#echo 131072 > /proc/sys/net/core/rmem_max
#echo 131072 > /proc/sys/net/core/wmem_default
#echo 131072 > /proc/sys/net/core/wmem_max
echo 1 > /proc/sys/net/ipv4/tcp_low_latency

/etc/init.d/gdm stop
#/etc/init.d/pulseaudio stop
/etc/init.d/cups stop
##leave the irq balance alone for now
/etc/init.d/irqbalance stop
#/etc/init.d/avahi-daemon stop
#/etc/init.d/plymouth stop
/etc/init.d/ondemand stop

#modprobe -r snd_hda_intel
#modprobe -r snd_hda_codec_nvhdmi snd_hda_codec_via snd_hda_intel
#modprobe -r snd_hda_codec_realtek snd_hda_codec snd_seq_midi snd_seq_midi_event snd_pcm snd_seq
#modprobe -r snd_hda_codec_nvhdmi  snd_hda_codec_via snd_hda_intel

#PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "eth0" | awk '{ print $1; }' | xargs echo)
#echo $PIDs
#for i in $PIDs
#do
    #echo "Changing pid $i"
#    ret=$(chrt -f -p 99 $i)
#done

#PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "irq" | awk '{ print $1; }' | xargs echo)
##echo $PIDs
#for i in $PIDs
#do
    #echo "Changing pid $i"
#    ret=$(chrt -f -p 99 $i)
#done

#PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "sky2" | awk '{ print $1; }' | xargs echo)
#echo $PIDs
#for i in $PIDs
#do
    #echo "Changing pid $i"
#    ret=$(chrt -f -p 99 $i)
#done


#PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "cpuset" | awk '{ print $1; }' | xargs echo)
##echo $PIDs
#for i in $PIDs
#do
    #echo "Changing pid $i"
#   ret=$(chrt -f -p 99 $i)
#done


#PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "events/" | awk '{ print $1; }' | xargs echo)
##echo $PIDs
#for i in $PIDs
#do
    #echo "Changing pid $i"
#    ret=$(chrt -f -p 99 $i)
#done


PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "\[" | awk '{ print $1; }' | xargs echo)
for i in $PIDs
do
    ret=$(chrt -f -p 99 $i)
done


#PIDs=$(ps -eLo pid,cls,rtprio,pri,nice,cmd | grep -i "ssh" | awk '{ print $1; }' | xargs e#cho)
#for i in $PIDs
#do
#    ret=$(chrt -f -p 99 $i)
#done
