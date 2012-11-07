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


#include <euryale/qos/medusa/MedusadCore.h>
#include <malloc.h>

#include <euryale/common/Backtrace.h>

int main(int argc, char** argv) {
    try {
        bool remove = false;
        if (argc > 1) {
            if (strcmp(argv[1], "-r") == 0) {
                remove = true;
            }else{
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("Error starting medusad, wrong argument! ('medusad -r' for removal, or 'medusad' for server startup)\n")));
                exit(-1);
            }
                
        }
        MedusadCore hadesd;
        hadesd.init(remove);
        if (!remove) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("Starting medusad...\n")));
            hadesd.start();
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("Removing cgroup files...\n")));
            hadesd.remove();
        }
    } catch (CgroupException& ex) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Ex=%s\n"), ex.toString().c_str()));
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)End\n")));
        Backtrace::backstrace();
    }
    return 0;
}
