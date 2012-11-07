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
 * File:   MedusadAcceptor.cpp
 * Author: rmartins
 * 
 * Created on December 16, 2009, 3:27 PM
 */

#include "MedusadAcceptor.h"

#include <euryale/qos/medusa/net/MedusaServiceHandler.h>
#include <euryale/qos/medusa/MedusadCore.h>
#include <euryale/qos/MedusaConf.h>


MedusadAcceptor::MedusadAcceptor(MedusadCore* core) :
m_addr(MedusaConf::MedusadPath),m_core(core)
{
}

MedusadAcceptor::~MedusadAcceptor()
{
    this->handle_close(ACE_INVALID_HANDLE, 0);
}
// Listing 5

// Listing 2 code/ch07

int
//MedusadAcceptor::open (const ACE_INET_Addr &listen_addr)
MedusadAcceptor::open()
{
    ACE_stat stat;
    if (ACE_OS::lstat(MedusaConf::MedusadPath, &stat) == 0) {
        remove(MedusaConf::MedusadPath);
    }
    if (this->acceptor_.open(m_addr, 1) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
        ACE_TEXT("(%T)%@\n"),
        ACE_TEXT("(%T)MedusaAcceptor:: acceptor.open: failed")),
        -1);
    
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusadAcceptor:: opened in %s\n"),m_addr.get_path_name()));
    return this->reactor()->register_handler
        (this, ACE_Event_Handler::ACCEPT_MASK);
}
// Listing 2

// Listing 3 code/ch07

int
MedusadAcceptor::handle_input(ACE_HANDLE)
{
    MedusaServiceHandler *client;
    ACE_NEW_RETURN(client, MedusaServiceHandler(m_core), -1);
    auto_ptr<MedusaServiceHandler> p(client);

    if (this->acceptor_.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
        ACE_TEXT("(%T) %@\n"),
        ACE_TEXT("(%T)Failed to accept ")
        ACE_TEXT("(%T)client connection")),
        -1);
    p.release();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusadAcceptor::handle_input - new connection!\n")));
    client->reactor(this->reactor());
    if (client->open() == -1) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Close client down!\n")));
        client->handle_close(ACE_INVALID_HANDLE, 0);
        return -1;
    }
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Conn ok!\n")));
    return 0;
}
// Listing 3

// Listing 4 code/ch07

int
MedusadAcceptor::handle_close(ACE_HANDLE, ACE_Reactor_Mask)
{
    if (this->acceptor_.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
            ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->acceptor_.close();
    }
    return 0;
}
