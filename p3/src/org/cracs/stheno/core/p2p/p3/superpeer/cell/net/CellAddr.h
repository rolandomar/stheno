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
 * File:   CellAddr.h
 * Author: rmartins
 *
 * Created on August 24, 2010, 3:05 PM
 */

#ifndef CELLADDR_H
#define	CELLADDR_H
#include <ace/Hash_Map_Manager.h>
#include <ace/Time_Value.h>
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>
#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>


#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>

class CellAddr: public Serializable {
public:
    CellAddr(InputStream& inputStream);
    CellAddr(CellIDPtr& cellID,String itf,String ip,Short port);
    CellAddr(const CellAddr& orig);
    virtual ~CellAddr();

    CellIDPtr& getCellID(){
        return m_cellID;
    }

    String& getIP(){
        return m_ip;
    }

    String getITF(){
        return m_itf;
    }

    Short getPort(){
        return m_port;
    }

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&) {
        UUID* cellID = m_cellID.get();
        cellID->serialize(outputStream);
        outputStream.write_string(m_itf);
        outputStream.write_string(m_ip);
        outputStream.write_short(m_port);
    }

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        CellID* cellID = new CellID(inputStream);
        m_cellID.reset(cellID);
        inputStream.read_string(m_itf);
        inputStream.read_string(m_ip);
        inputStream.read_short(m_port);
    }

protected:    
    CellIDPtr m_cellID;
    String m_itf;
    String m_ip;
    Short m_port;
};

typedef ACE_Strong_Bound_Ptr<CellAddr, ACE_Recursive_Thread_Mutex> CellAddrPtr;

#endif	/* CELLADDR_H */

