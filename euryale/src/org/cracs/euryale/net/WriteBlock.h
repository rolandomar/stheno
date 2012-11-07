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
 * File:   WriteBlock.h
 * Author: rmartins
 *
 * Created on September 28, 2010, 6:26 PM
 */

#ifndef WRITEBLOCK_H
#define	WRITEBLOCK_H

#include <ace/Message_Block.h>
#include <ace/Time_Value.h>
#include <euryale/event/Event.h>
#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>


class WriteBlock : public Event {
public:

    WriteBlock();
    WriteBlock(ACE_Time_Value* timeout);
    WriteBlock(ACE_Message_Block* mb, ACE_Time_Value* timeout);
    ~WriteBlock();

    ACE_Message_Block* getMessageBlock();

    int wait(ACE_Time_Value* timeout);
    int signal(void);

protected:
    bool m_status;
    ACE_Message_Block* m_mb;
};

typedef ACE_Strong_Bound_Ptr<WriteBlock, ACE_Recursive_Thread_Mutex> WriteBlockPtr;

#endif	/* WRITEBLOCK_H */

