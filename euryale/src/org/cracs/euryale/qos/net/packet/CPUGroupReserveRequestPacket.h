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
 * File:   CPUGroupReserveRequest.h
 * Author: rmartins
 *
 * Created on December 18, 2009, 3:31 PM
 */

#ifndef _CPUGROUPRESERVEREQUEST_H
#define	_CPUGROUPRESERVEREQUEST_H

#include <euryale/qos/net/packet/MedusaPacket.h>
#include <euryale/Types.h>
#include <euryale/qos/cpu_qos/CPUGroupQoS.h>

class CPUGroupReserveRequestPacket : public MedusaPacket {
public:
    //static const UInt CPUGROUP_RESERVE_REQUEST_PACKET_ID = 0x000003;
    CPUGroupReserveRequestPacket(ACE_CString& cpuGroup,UInt requestID,CPUGroupQoS& qos);
    CPUGroupReserveRequestPacket(){}
    CPUGroupReserveRequestPacket(MedusaHeader* packetHeader) :
    MedusaPacket(packetHeader){

    }
    virtual ~CPUGroupReserveRequestPacket();

    void setCgroup(ACE_CString& cgroup)
    {
        this->m_cgroup = cgroup;
    }
    ACE_CString& getCgroup()
    {
        return m_cgroup;
    }

    CPUGroupQoS& getCPUGroupQoS(){
        return m_qos;
    }

protected:
    ACE_CString m_cgroup;
    CPUGroupQoS m_qos;

    ACE_INLINE virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&){
        outputStream.write_string(m_cgroup);
        m_qos.serialize(outputStream);
    }

    ACE_INLINE virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&){
        inputStream.read_string(m_cgroup);
        m_qos.deserialize(inputStream);
    }
};

#endif	/* _CPUGROUPRESERVEREQUEST_H */

