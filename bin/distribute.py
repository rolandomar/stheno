#!/usr/bin/env python
#import os
import os
import sys
from subprocess import *
import string
#import ssh
#import paramiko
import subprocess
import socket
import fcntl
import struct
def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

print "DAEM: distribute script v0.1"

ssh_command="ssh"
ip=get_ip_address('eth0')

#print "LLL=",os.getcwd()

if len(sys.argv) != 3:
    print "error: syntax: distribute hosts_file base_dir"
    sys.exit(-1)


hosts = open(sys.argv[1],"r")
base_dir = sys.argv[2]

lines = hosts.readlines()
lines.pop()
print lines
user_ = "root"
pass_ = "Rl202703"

#if base_dir == '.':
#    base_dir = os.getcwd()
build_dir=base_dir+"/build"
bin_dir=base_dir+"/bin"

files = [
build_dir+"/euryale/Debug/"+"medusad", 
build_dir+"/euryale/Debug/"+"load",
build_dir+"/euryale/Debug/"+"libeuryale.so",
build_dir+"/stheno/Debug/"+"libstheno.so", 
build_dir+"/p3/Debug/"+"leaf",
build_dir+"/p3/Debug/"+"libleaf_plugin.so",
build_dir+"/p3/Debug/"+"libmonitor_plugin.so" ,
build_dir+"/p3/Debug/"+"libp3.so", 
build_dir+"/p3/Debug/"+"libsensor_plugin.so", 
build_dir+"/p3/Debug/"+"libsuperpeer_plugin.so", 
build_dir+"/p3/Debug/"+"monitor", 
build_dir+"/p3/Debug/"+"superpeer",
base_dir+"/p3/"+"p3.properties",
bin_dir+"/"+"superpeer.sh",
bin_dir+"/"+"monitor.sh",
bin_dir+"/"+"leaf.sh",
bin_dir+"/"+"load.sh",
bin_dir+"/"+"medusad.sh",
bin_dir+"/"+"medusad_cleanup.sh",
bin_dir+"/"+"network_tweak.sh"
]

import threading #, paramiko



class ParallelExecution(threading.Thread):                                                                                                                                                                                                       
    def __init__(self , host,remove_command,add_command):
        threading.Thread.__init__(self)                                                                                                                                                                                                     
        self.host = host
        self.add_command = add_command
        self.remove_command = remove_command

    def run(self):      
        print "Distributing to ",self.host
        #os.system(self.remove_command)
        os.system(self.add_command)
        print "Finished host ",self.host


base_dir="~/stheno"
remove_command="cd "+base_dir+";"+"rm ";
for file in files:
    remove_command = remove_command +" "+file

add_command_files=""
for file in files:
    add_command_files = add_command_files + " "+file

T = []

for line in lines:
    line = string.rstrip(line,'\n')
    if line != "":
#        if line != ip:
        print"ssh " + line + " '" + remove_command+"'"
        full_remove_command = "ssh " + line + " '" + remove_command+"'"
        #os.system(full_remove_command)
        add_command = "scp "+add_command_files + " " + user_ + "@" + line + ":" + base_dir
        #os.system(add_command)
        th = ParallelExecution(line,full_remove_command,add_command)
        T = T + [th]
        th.start()


for t in T:
    t.join()















