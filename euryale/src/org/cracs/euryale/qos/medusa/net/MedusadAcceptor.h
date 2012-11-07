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
 * File:   MedusadAcceptor.h
 * Author: rmartins
 *
 * Created on December 16, 2009, 3:27 PM
 */

#ifndef _HADESDACCEPTOR_H
#define	_HADESDACCEPTOR_H

#include "ace/Auto_Ptr.h"
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include "ace/LSOCK_Acceptor.h"
#include "ace/Reactor.h"
#include <ace/UNIX_Addr.h>

class MedusadCore;
class MedusadAcceptor : public ACE_Event_Handler
{
public:
  
  MedusadAcceptor(MedusadCore *core);
  virtual ~MedusadAcceptor ();

  //FUZZ: disable check_for_lack_ACE_OS
  //int open (const ACE_INET_Addr &listen_addr);
  int open ();
  //FUZZ: enable check_for_lack_ACE_OS

  // Get this handler's I/O handle.
  virtual ACE_HANDLE get_handle (void) const
    { return this->acceptor_.get_handle (); }

  // Called when a connection is ready to accept.
  virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);

  // Called when this handler is removed from the ACE_Reactor.
  virtual int handle_close (ACE_HANDLE handle,
                            ACE_Reactor_Mask close_mask);

protected:
  MedusadCore *m_core;
  ACE_LSOCK_Acceptor acceptor_;
  ACE_UNIX_Addr m_addr;
};


#endif	/* _HADESDACCEPTOR_H */

