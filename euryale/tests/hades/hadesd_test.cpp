/*
 *  Copyright 2012 Rolando Martins, CRACS & INESC-TEC, DCC/FCUP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *   
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * 
 */
/* 
 * File:   hadesd_test.cpp
 * Author: rmartins
 *
 * Created on December 19, 2009, 10:32 AM
 */

#include <stdio.h>

#include <euryale/qos/hadesd/HadesdCore.h>

int main(){
    try{
        HadesdCore hadesd;
        hadesd.init();
        hadesd.start();
    }catch(CgroupException* ex){
       ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Ex=%s\n",ex->toString().c_str())));
       ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)End\n")));
    }
    return 0;
}