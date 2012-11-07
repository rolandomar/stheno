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
 * File:   RPCRuntimeManagerObject.cpp
 * Author: rmartins
 * 
 * Created on February 8, 2011, 10:58 AM
 */

#include "RPCRuntimeManagerObject.h"

#include <stheno/services/rpc/RPCService.h>

RPCRuntimeManagerObject::RPCRuntimeManagerObject(RPCService* rpcService) :
RPCObject(rpcService), m_oid(new UUID("67F12BAC6A3B5C704D45BAE532416804")) {
}

RPCRuntimeManagerObject::~RPCRuntimeManagerObject() {
}

void RPCRuntimeManagerObject::hardFault() {
    printf("==============>HARD FAULT<===================\n");
    exit(-1);
    return;
}

bool RPCRuntimeManagerObject::temporaryFault(ACE_Time_Value& duration) {
    printf("==============>TEMP FAULT<===================\n");
    return true;
}

void RPCRuntimeManagerObject::handleOneWayInvocation(ACE_Message_Block* arg) {
    ByteInputStream is(arg);
    UShort op;
    is.read_ushort(op);
    if (op == HARD_FAULT_OP) {
        hardFault();
        //ENDS HERE!            
    }
    ACE_Message_Block::release(arg);
    return;

}

ACE_Message_Block* RPCRuntimeManagerObject::handleTwoWayInvocation(ACE_Message_Block* arg) {
    ByteInputStream is(arg);
    ACE_Message_Block::release(arg);
    UShort op;
    is.read_ushort(op);
    if (op == TEMP_FAULT_OP) {
        ACE_Time_Value duration;
        LongLong secs;
        ULongLong usecs;
        is.read_longlong(secs);
        is.read_ulonglong(usecs);
        duration.set(secs, usecs);
        bool ret = temporaryFault(duration);
        size_t size = 10024;
        ByteOutputStream os(size);
        os.write_boolean(ret);
        ACE_Message_Block* payload = os.begin()->duplicate();
        return payload;
    }
    return 0;
}

void RPCRuntimeManagerObject::onReplication(ByteInputStream& is) throw (ServiceException&) {
    throw ServiceException(ServiceException::NON_FT_SERVICE);
}
