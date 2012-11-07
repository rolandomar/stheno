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
 * File:   RPCRuntimeManagerObject.h
 * Author: rmartins
 *
 * Created on February 8, 2011, 10:58 AM
 */

#ifndef RPCRUNTIMEMANAGEROBJECT_H
#define	RPCRUNTIMEMANAGEROBJECT_H

#include <euryale/Types.h>
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <stheno/services/rpc/exception/RPCException.h>
#include <stheno/services/rpc/RPCObject.h>
#include <stheno/service/exception/ServiceException.h>

class RPCRuntimeManagerObject : public RPCObject {
public:
    RPCRuntimeManagerObject(RPCService* rpcService);
    virtual ~RPCRuntimeManagerObject();

    enum {
        HARD_FAULT_OP = 0x01,
        TEMP_FAULT_OP = 0x02
    };

    virtual UUIDPtr& getOID() {
        return m_oid;
    }

    void hardFault();
    bool temporaryFault(ACE_Time_Value& duration);
    
    

    virtual void handleOneWayInvocation(ACE_Message_Block* arg);
    virtual ACE_Message_Block* handleTwoWayInvocation(ACE_Message_Block* arg);
    
    void onReplication(ByteInputStream& is) throw (ServiceException&);

protected:
    UUIDPtr m_oid;    

};


#endif	/* RPCRUNTIMEMANAGEROBJECT_H */

