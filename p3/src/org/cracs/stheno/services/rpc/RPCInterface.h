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
 * File:   RPCInterface.h
 * Author: rmartins
 *
 * Created on January 21, 2011, 3:51 PM
 */

#ifndef RPCINTERFACE_H
#define	RPCINTERFACE_H

#include <ace/Message_Block.h>


class RPCInterface{
public:
    virtual ACE_Message_Block* twoWayInvocation(ACE_Message_Block* arg) = 0;
    virtual void oneWayInvocation(ACE_Message_Block* arg) = 0;
};

#endif	/* RPCINTERFACE_H */

