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
 * File:   RPCTestObject.cpp
 * Author: rmartins
 * 
 * Created on January 22, 2011, 8:14 PM
 */

#include "RPCTestObject.h"
#include <stheno/services/rpc/RPCService.h>

RPCTestObject::RPCTestObject(RPCService* rpcService) :
RPCObject(rpcService), m_oid(new UUID("BFDD340C31ECDD49CD7E694AF4AFF426")), m_sum(0) {
  /*pthread_mutexattr_t attr;
  if(pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT) != 0){
    printf("Error! on mutex setprotocol\n");
  }
  if(pthread_mutex_init (&m_lock, &attr) != 0){
    printf("Error! on mutex setattr\n");
    }*/
    
    
}

RPCTestObject::~RPCTestObject() {
}

ULong RPCTestObject::ping() {
    //printf("<<<<<<<<<<<<<<<<<<<<<<PING>>>>>>>>>>>>>>>>>>>>>\n");
    return 0;
}


LongLong total = 23213;

Long RPCTestObject::sum(Long num) {
  ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,0);    
  //if(pthread_mutex_lock(&m_lock) != 0){
  //  printf("Error! on mutex\n");
  //}
  ACE_Time_Value start = ACE_OS::gettimeofday();
  ACE_Time_Value delta (0,10000);
  ACE_Time_Value end = start + delta;
  
  for(LongLong i=0; i < 5000000 && ACE_OS::gettimeofday() < end; i++){
    int kk = 32832;
    total = i*(total+i)*i*kk;
    total = total/2  + i;
    total *= total*total*total*i/kk;
    }
  m_sum += num;
  //Long temp = num;
  //pthread_mutex_unlock(&m_lock);
  //return temp;
  return m_sum;
}

ACE_Message_Block* RPCTestObject::handleTwoWayInvocation(ACE_Message_Block* arg) {
    ByteInputStream is(arg);
    ACE_Message_Block::release(arg);
    UShort op;
    is.read_ushort(op);
    if (op == SUM_OP) {
        Long num;
        is.read_long(num);
        Long ret = sum(num);
        size_t size = 10024;
        ByteOutputStream os(size);
        os.write_long(ret);
        ACE_Message_Block* payload = os.begin()->duplicate();
        //printf("<<<<<<<<<<< is this FT=%d>>>>>>>>>>>>>>\n", getRPCService()->isFT());
        if (getRPCService()->isFT()) {
            size_t size = 10024;
            ByteOutputStream os(size);           
            os.write_long(ret);
            getRPCService()->replicate(os.begin()->duplicate());
        }


        return payload;
    }
    return 0;
}

void RPCTestObject::onReplication(ByteInputStream& is) throw (ServiceException&) {    
    is.read_long(m_sum);
    //printf("Replica: sum is now %d\n",m_sum);
}
