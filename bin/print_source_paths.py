#!/usr/bin/env python                                                                                                                                                            
# a bar plot with errorbars                                                                                                                                                      
import numpy as np
import os
import sys
import re
from subprocess import *
import math
import string

def search( path ):
    if path.endswith('.svn'):
        return
    if path.endswith('.cvs'):
        return
    
    #print path
    files = os.listdir(path)
    packages = []
    #print files
    for file in files:
        sub_path = path+'/'+file
        if os.path.isdir(sub_path):
            if sub_path.endswith('.svn'):
                continue
            if sub_path.endswith('.cvs'):
                continue
            print sub_path
            #print "AUX_SOURCE_DIRECTORY("+sub_path+"\t\t src_"+sub_path+")"            
            search(sub_path)
            
        
print "Processing files from path ",sys.argv[1]
if not os.path.isdir(sys.argv[1]):
    print "invalid directory"
    sys.exit(-1)

search(sys.argv[1])


    
