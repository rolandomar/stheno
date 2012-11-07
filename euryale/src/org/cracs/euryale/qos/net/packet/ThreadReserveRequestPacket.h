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
 * File:   CPUReservePacket.h
 * Author: rmartins
 *
 * Created on December 17, 2009, 12:49 PM
 */

#ifndef _CPURESERVEPACKET_H
#define	_CPURESERVEPACKET_H

#include <euryale/qos/net/packet/MedusaPacket.h>
#include <euryale/Types.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>

class ThreadReserveRequestPacket : public MedusaPacket {
public:
    //static const UInt THREAD_RESERVE_REQUEST_PACKET_ID = 0x000001;
    ThreadReserveRequestPacket(UInt requestID,UInt task,ACE_CString& cpusetGroup,
        ACE_CString& cpuSchedGroup,CPUQoS* qos);
    ThreadReserveRequestPacket():m_qos(0){}
    ThreadReserveRequestPacket(MedusaHeader* packetHeader) :
    MedusaPacket(packetHeader),m_qos(0){
        
    }
    virtual ~ThreadReserveRequestPacket();
    
    void setCgroup(ACE_CString& cgroup)
    {
        this->m_cgroup = cgroup;
    }
    ACE_CString& getCgroup()
    {
        return m_cgroup;
    }

    ACE_CString& getScheGroup(){
        return m_scheGroup;
    }

    CPUQoS* getCPUQoS(){
        return m_qos;
    }

    UInt getTask(){
        return m_task;
    }

    virtual void
    serialize(OutputStream& outputStream) THROW(SerializationException&){
        UInt bodySize = getBodySerializationSize();
        ((MedusaHeader*)m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        serializeBody(outputStream);
    }
    
protected:
    ACE_CString m_cgroup;
    ACE_CString m_scheGroup;
    CPUQoS* m_qos;
    UInt m_task;

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&){
        //ACE_DEBUG((LM_DEBUG,
                //("(%T) ThreadReserveRequestPacket::serializeBody()!\n")));
        outputStream.write_ulong(m_task);
        outputStream.write_string(m_cgroup);
        outputStream.write_string(m_scheGroup);
        m_qos->serialize(outputStream);
        //ACE_DEBUG((LM_DEBUG,
                //ACE_TEXT("(%T) ThreadReserveRequestPacket::serializeBody(): end!\n")));
    }
    
    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&){
         //ACE_DEBUG((LM_DEBUG,
                //ACE_TEXT("(%T) ThreadReserveRequestPacket::deserializeBody()!\n")));
         inputStream.read_ulong(m_task);
        inputStream.read_string(m_cgroup);
        inputStream.read_string(m_scheGroup);
        if(m_qos != 0){
            delete m_qos;
        }
        m_qos = CPUQoSFactory::createCPUQoS(inputStream);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)HERE=%s qos=%d\n"),m_cgroup.c_str(),m_qos->getPolicy()));
    }
};

#endif	/* _CPURESERVEPACKET_H */

