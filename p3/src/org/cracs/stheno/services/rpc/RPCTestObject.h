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
 * File:   RPCTestObject.h
 * Author: rmartins
 *
 * Created on January 22, 2011, 8:14 PM
 */

#ifndef RPCTESTOBJECT_H
#define	RPCTESTOBJECT_H
#include <euryale/Types.h>
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <stheno/services/rpc/exception/RPCException.h>
#include <stheno/services/rpc/RPCObject.h>
#include <stheno/service/exception/ServiceException.h>

class RPCTestObject : public RPCObject {
public:
    RPCTestObject(RPCService* rpcService);
    virtual ~RPCTestObject();

    enum {
        PING_OP = 0x01,
        SUM_OP = 0x02,
    };

    virtual UUIDPtr& getOID() {
        return m_oid;
    }

    ULong ping();
    Long sum(Long num);

    virtual void handleOneWayInvocation(ACE_Message_Block* arg) {
        ByteInputStream is(arg);
        UShort op;
        is.read_ushort(op);
        if (op == PING_OP) {
            ping();
        }
        ACE_Message_Block::release(arg);
        
    }

    virtual ACE_Message_Block* handleTwoWayInvocation(ACE_Message_Block* arg);
    void onReplication(ByteInputStream& is) throw (ServiceException&);

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    //pthread_mutex_t m_lock;    
    //pthread_mutex_t m;
    UUIDPtr m_oid;
    Long m_sum;

};

#endif	/* RPCTESTOBJECT_H */

