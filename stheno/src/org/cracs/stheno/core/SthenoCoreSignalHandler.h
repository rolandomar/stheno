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
 * File:   SthenoCoreSignalHandler.h
 * Author: rmartins
 *
 * Created on February 19, 2011, 1:29 PM
 */

#ifndef STHENOCORESIGNALHANDLER_H
#define	STHENOCORESIGNALHANDLER_H
#include <ace/Event_Handler.h>
#include <ace/Log_Msg.h>
#include <ace/Sig_Handler.h>
class SthenoCoreSignalHandler : public ACE_Event_Handler {
public:
    SthenoCoreSignalHandler();
    virtual ~SthenoCoreSignalHandler();

    int open() {        
        ACE_Sig_Handler sh;
        if(sh.register_handler(SIGTERM, this)!=0){
            perror("Error on signal registration:");            
            return -1;
        } 
         if(sh.register_handler(SIGSEGV, this)!=0){
            perror("Error on signal registration:");            
            return -1;
        } 
        m_open = true;
        return 0;
    }

    virtual int handle_signal(int signo,
            siginfo_t * = 0,
            ucontext_t * = 0) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) ERROR: SIGNAL RECEIVED(%d)\n"), signo));
        if(signo == SIGTERM || signo == SIGKILL || signo == SIGSEGV){
            exit(-1);
        }        
        return 0;
    }
protected:
    bool m_open;

};

#endif	/* STHENOCORESIGNALHANDLER_H */

