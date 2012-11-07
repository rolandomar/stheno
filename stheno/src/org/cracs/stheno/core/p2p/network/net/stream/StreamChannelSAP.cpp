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
 * File:   StreamChannelSAP.cpp
 * Author: rmartins
 * 
 * Created on October 27, 2010, 11:44 AM
 */

#include "StreamChannelSAP.h"

StreamChannelSAP::StreamChannelSAP(IncomingStreamClientHandler* clientHandler):
m_clientHandler(clientHandler)
//:m_mesh(mesh) {
{
}

StreamChannelSAP::~StreamChannelSAP() {
    close();
}

StreamChannelAcceptor* StreamChannelSAP::createAcceptor() {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO--------------------------: P3MeshAcceptor::createAcceptor() \n")));
    //P3MeshAcceptor* acceptor = new P3MeshAcceptor(m_mesh);
    StreamChannelAcceptor* acceptor = new StreamChannelAcceptor(m_clientHandler);
    //printf("ACCEPTOR=%p %d\n",acceptor,acceptor->get_handle());
    return acceptor;
}
