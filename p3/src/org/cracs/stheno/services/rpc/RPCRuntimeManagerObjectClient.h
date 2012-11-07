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
 * File:   RPCRuntimeManagerObjectClient.h
 * Author: rmartins
 *
 * Created on February 8, 2011, 11:00 AM
 */

#ifndef RPCRUNTIMEMANAGEROBJECTCLIENT_H
#define	RPCRUNTIMEMANAGEROBJECTCLIENT_H

#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/rpc/RPCServiceClient.h>
#include <stheno/services/rpc/RPCTestObject.h>

class RPCRuntimeManagerObjectClient {
public:
    RPCRuntimeManagerObjectClient(RPCServiceClient* rpcRPCServiceClient);
    virtual ~RPCRuntimeManagerObjectClient();

    void hardFault() {
        size_t size = 10024;
        ByteOutputStream os(size);
        os.write_ushort(RPCRuntimeManagerObject::HARD_FAULT_OP);
        ACE_Message_Block* payload = os.begin()->duplicate();
        try {
            m_rpcRPCServiceClient->oneWayInvocation(m_oid, payload);
        } catch (RPCException& ex) {
            printf("Exception! in RPC - 1way\n");
        }

    }

    bool temporaryFault(ACE_Time_Value& duration) {
        size_t size = 10024;
        ByteOutputStream os(size);
        os.write_ushort(RPCRuntimeManagerObject::TEMP_FAULT_OP);
        os.write_longlong(duration.sec());
        os.write_ulonglong(duration.usec());
        ACE_Message_Block* payload = os.begin()->duplicate();
        ACE_Message_Block* reply = 0;
        try {
            reply = m_rpcRPCServiceClient->twoWayInvocation(m_oid, payload);
        } catch (RPCException& ex) {
            printf("Exception! in RPC\n");
            return 0;
        }
        if (reply == 0) {
            ACE_Message_Block::release(reply);
            return 0;
        }

        ByteInputStream is(reply);
        ACE_Message_Block::release(reply);
        bool ret = 0;
        is.read_boolean(ret);
        return ret;
    }

    void close() {
        if (m_rpcRPCServiceClient != 0) {
            m_rpcRPCServiceClient->close();
            delete m_rpcRPCServiceClient;
            m_rpcRPCServiceClient = 0;
        }

    }

protected:
    RPCServiceClient* m_rpcRPCServiceClient;
    UUIDPtr m_oid;
};


#endif	/* RPCRUNTIMEMANAGEROBJECTCLIENT_H */

