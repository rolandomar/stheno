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
 * File:   WriteDatagramBlock.h
 * Author: rmartins
 *
 * Created on September 28, 2010, 5:59 PM
 */

#ifndef WRITEDATAGRAMBLOCK_H
#define	WRITEDATAGRAMBLOCK_H

#include <ace/Message_Block.h>
#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <euryale/event/Event.h>
#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>


class WriteDatagramBlock : public Event {
public:
    WriteDatagramBlock(ACE_Time_Value* timeout);
    WriteDatagramBlock(ACE_Message_Block* mb, ACE_INET_Addr* addr, ACE_Time_Value* timeout);
    virtual ~WriteDatagramBlock();
    //
    ACE_Message_Block* getMessageBlock();
    //
    ACE_INET_Addr* getAddr();

    int wait(ACE_Time_Value* timeout);
    int signal(void);

protected:
    bool m_status;
    ACE_Message_Block* m_mb;
    ACE_INET_Addr* m_addr;
};

typedef ACE_Strong_Bound_Ptr<WriteDatagramBlock, ACE_Recursive_Thread_Mutex> WriteDatagramBlockPtr;

#endif	/* WRITEDATAGRAMBLOCK_H */

