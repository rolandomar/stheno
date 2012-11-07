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
 * File:   MedusaBaseHandler.h
 * Author: rmartins
 *
 * Created on February 13, 2010, 6:35 PM
 */

#ifndef _HADESBASEHANDLER_H
#define	_HADESBASEHANDLER_H

#include <ace/Synch_Traits.h>
#include <ace/Null_Condition.h>
#include <ace/Null_Mutex.h>

#include <ace/Reactor.h>
#include <ace/INET_Addr.h>
#include <ace/LSOCK_Stream.h>
#include <ace/LSOCK_Connector.h>
#include <ace/Connector.h>
#include <ace/Svc_Handler.h>
#include <ace/Reactor_Notification_Strategy.h>

#include <euryale/qos/net/packet/MedusaPacket.h>

class MedusaBaseHandler :
public ACE_Svc_Handler<ACE_LSOCK_STREAM, ACE_NULL_SYNCH> {
    typedef ACE_Svc_Handler<ACE_LSOCK_STREAM, ACE_NULL_SYNCH> super;

public:

    MedusaBaseHandler () : notifier_ (0, this, ACE_Event_Handler::WRITE_MASK)
        {}

    virtual int send(MedusaPacket* packet);
    virtual int open(void * = 0);

    // Called when input is available from the client.
    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    // Called when output is possible.
    virtual int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    // Called when a timer expires.
    virtual int handle_timeout(const ACE_Time_Value &current_time,
        const void *act = 0);
protected:
    virtual int handlePacket(MedusaPacket* packet){
        return 0;
    }

private:

    enum {
        ITERATIONS = 5
    };
    int iterations_;
    ACE_Reactor_Notification_Strategy notifier_;
    ACE_Message_Block* m_begin;
    ACE_Message_Block* m_current;
    MedusaHeader* m_header;
    ACE_Message_Block* m_partialMessage;
};

#endif	/* _HADESBASEHANDLER_H */

