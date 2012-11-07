#!/usr/bin/env python
import os
import sys
from subprocess import *
import string
import subprocess
import fnmatch
print "DAEM: distribute script v0.1"

ssh_command="ssh"

#if len(sys.argv) != 2:
#    print "error: syntax: distribute hosts_file"
#    sys.exit(-1)


user_ = "root"
pass_ = "Rl202703"


import threading 


class ParallelExecution(threading.Thread):                                                                                                                                                                                                       
    def __init__(self ,add_command):
        threading.Thread.__init__(self)                                                                                                                                                                                                     
        self.add_command = add_command

    def run(self):      
        os.system(self.add_command)


base_dir="/root/stheno"
#add_command_files=""
#for file in files:
#    add_command_files = add_command_files + " "+base_dir+"/"+file


#stat_file_pattern = "*.log"
#files = []
#for filename in os.listdir(base_dir):
#    if fnmatch.fnmatch(filename, stat_file_pattern):
#        files = files + [filename]


#for f in files:
#    add_command_files = add_command_files + " "+base_dir+"/"+f

cmd = "scp /root/stheno/*.log rmartins@172.18.50.100:~/stheno_controller/"
os.system(cmd)

#cmd = "scp /root/stheno/*core* rmartins@172.18.50.100:~/stheno_controller/"
#os.system(cmd)
#T = []

#def group(s, n): 
#    return [s[i:i+n] for i in xrange(0, len(s), n)]
#def group(s, n): 
#    if len(s) == 0:
#        return []
#    num=len(s)/n
#    if num <=0:
#        num = 1
#    for index in range(0,num):
#        return [s[i:i+num] for i in xrange(0, len(s), num)]

#file_group = group(files,2)
#for item in file_group:
#    #print "item=",item
#    add_command_files=""    
#    for f in item:
#        add_command_files = add_command_files + " "+base_dir+"/"+f
#    cmd = "scp "+add_command_files + " " + "rmartins@172.18.50.100:~/stheno_controller/"
#    th = ParallelExecution(cmd)
#    T = T + [th]
#    th.start()

#for t in T:
#    t.join()















