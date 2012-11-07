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
build_dir+"/euryale/Release/"+"medusad", 
build_dir+"/euryale/Release/"+"load",
build_dir+"/euryale/Release/"+"libeuryale.so",
build_dir+"/stheno/Release/"+"libstheno.so", 
build_dir+"/p3/Release/"+"leaf",
build_dir+"/p3/Release/"+"libleaf_plugin.so",
build_dir+"/p3/Release/"+"libmonitor_plugin.so" ,
build_dir+"/p3/Release/"+"libp3.so", 
build_dir+"/p3/Release/"+"libsensor_plugin.so", 
build_dir+"/p3/Release/"+"libsuperpeer_plugin.so", 
build_dir+"/p3/Release/"+"monitor", 
build_dir+"/p3/Release/"+"superpeer",
base_dir+"/p3/"+"p3.properties",
bin_dir+"/"+"superpeer.sh",
bin_dir+"/"+"monitor.sh",
bin_dir+"/"+"leaf.sh",
bin_dir+"/"+"load.sh",
bin_dir+"/"+"medusad.sh",
bin_dir+"/"+"medusad_cleanup.sh",
bin_dir+"/"+"kill.sh",
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
        print "COMMMAND=",self.add_command
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















