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
 * File:   MedusaServiceHandler.h
 * Author: rmartins
 *
 * Created on December 16, 2009, 3:28 PM
 */

#ifndef _HADESSERVICEHANDLER_H
#define	_HADESSERVICEHANDLER_H

#include "ace/Message_Block.h"
#include "ace/Message_Queue.h"
#include "ace/LSOCK_Stream.h"
#include "ace/Synch.h"

#include <euryale/net/EventHandler.h>
#include <euryale/qos/net/packet/MedusaPacket.h>
#include "ace/Reactor_Notification_Strategy.h"
//#include <euryale/qos/medusa/net/packet/MedusaPacketStateMachine.h>

class MedusadCore;
class MedusaServiceHandler : public EventHandler
{
public:
    MedusaServiceHandler(MedusadCore* core):
    notifier_ (0, this, ACE_Event_Handler::WRITE_MASK),
    m_core(core){
        
    }

    virtual ~MedusaServiceHandler(){
        if(m_begin!=0){
            delete m_begin;
        }
    }


  ACE_LSOCK_Stream &peer (void) { return this->sock_; }
  static const int MAX_BUF_SIZE=102400;

  //FUZZ: disable check_for_lack_ACE_OS
  int open (void* arg=0);
  //FUZZ: enable check_for_lack_ACE_OS

  int send(MedusaPacket* packet);

  // Get this handler's I/O handle.
  virtual ACE_HANDLE get_handle (void) const
    { return this->sock_.get_handle (); }

  // Called when input is available from the client.
  virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);

  // Called when output is possible.
  virtual int handle_output (ACE_HANDLE fd = ACE_INVALID_HANDLE);

  // Called when this handler is removed from the ACE_Reactor.
  virtual int handle_close (ACE_HANDLE handle,
                            ACE_Reactor_Mask close_mask);
  
  UUIDPtr& getUUID();

protected:
  UUIDPtr m_uuid;
  ACE_LSOCK_Stream sock_;
  ACE_Message_Queue<ACE_NULL_SYNCH> output_queue_;
  ACE_Message_Block* m_begin;
  ACE_Message_Block* m_current;
  MedusaHeader* m_header;
  ACE_Message_Block* m_partialMessage;
  MedusadCore* m_core;
  ACE_Atomic_Op<ACE_Thread_Mutex, UInt> m_sequenceID;

  
  void processPacket(MedusaPacket* packet);


  ACE_Reactor_Notification_Strategy notifier_;
};

#endif	/* _HADESSERVICEHANDLER_H */

