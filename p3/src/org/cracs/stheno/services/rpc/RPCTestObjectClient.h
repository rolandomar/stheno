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
 * File:   RPCTestObjectClient.h
 * Author: rmartins
 *
 * Created on January 22, 2011, 8:20 PM
 */

#ifndef RPCTESTOBJECTCLIENT_H
#define	RPCTESTOBJECTCLIENT_H

#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/rpc/RPCServiceClient.h>
#include <stheno/services/rpc/RPCTestObject.h>

class RPCTestObjectClient {
public:
    RPCTestObjectClient(RPCServiceClient* rpcRPCServiceClient);
    virtual ~RPCTestObjectClient();

    void ping() {
        size_t size = 10024;
        ByteOutputStream os(size);
        os.write_ushort(RPCTestObject::PING_OP);
        ACE_Message_Block* payload = os.begin()->duplicate();
        try {
            m_rpcRPCServiceClient->oneWayInvocation(m_oid, payload);
        } catch (RPCException& ex) {
            printf("Exception! in RPC - 1way\n");
        }

    }

    Long sum(Long num) throw(RPCException&){


        size_t size = 10024;
        ByteOutputStream os(size);
        os.write_ushort(RPCTestObject::SUM_OP);
        os.write_long(num);
        ACE_Message_Block* payload = os.begin()->duplicate();
        ACE_Message_Block* reply = 0;
        //ACE_Time_Value start = ACE_OS::gettimeofday();
        try {
            reply = m_rpcRPCServiceClient->twoWayInvocation(m_oid, payload);
        } catch (RPCException& ex) {
            //printf("Exception! in RPC\n");
            throw ex;
        }
        if(reply == 0){
            ACE_Message_Block::release(reply);
            throw RPCException(RPCException::QOS_CLIENT_INITIALIZATION_ERROR);           
        }
        
        ByteInputStream is(reply);        
        ACE_Message_Block::release(reply);
        Long ret = 0;
        is.read_long(ret);
        /*if (m_counter % 250 == 0) {
            ACE_Time_Value end = ACE_OS::gettimeofday();
            end -= start;
            ULongLong deltausec;
            end.to_usec(deltausec);
            ACE_DEBUG((LM_DEBUG,
                    ACE_TEXT("(%t|%T)INFO: sum lat(%Q)usec \n"),
                    deltausec
                    ));
        }
        m_counter++;*/
        return ret;
    }

protected:
    RPCServiceClient* m_rpcRPCServiceClient;
    UUIDPtr m_oid;
    int m_counter;
};

#endif	/* RPCTESTOBJECTCLIENT_H */

