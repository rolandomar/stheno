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
 * File:   PeerMapBase.h
 * Author: rmartins
 *
 * Created on August 19, 2010, 2:02 PM
 */

#ifndef PEERMAPBASE_H
#define	PEERMAPBASE_H

#include <ace/Hash_Map_Manager.h>
#include <ace/Time_Value.h>
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>
#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <euryale/Types.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/common/ServiceInfo.h>

#include <stheno/core/p2p/common/exception/PeerException.h>
#include <stheno/core/p2p/common/PeerFactory.h>

template <class PEER_INFO>
class PeerMapBase : public Serializable {
public:
    typedef PEER_INFO PeerInfoType;
    typedef ACE_Strong_Bound_Ptr<PeerInfoType, ACE_SYNCH_MUTEX> PeerInfoTypePtr;
    typedef ACE_Strong_Bound_Ptr<PeerMapBase<PeerInfoType>, ACE_SYNCH_MUTEX> PeerMapPtr;

    PeerMapBase();
    virtual ~PeerMapBase();

    bool exists(const UUIDPtr& uuid);
    bool addPeer(PeerInfoTypePtr& peer);
    bool removePeer(UUIDPtr& uuid);
    void getPeerInfo(const UUIDPtr& uuid, PeerInfoTypePtr& peerInfoPtr) throw (PeerException&);

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

protected:
    ACE_Hash_Map_Manager<UUIDPtr, PeerInfoTypePtr, ACE_SYNCH_RW_MUTEX> m_map;
    PeerFactory<PeerInfoType>* m_factory;
};

//Implementation
template <class PEER_INFO>
PeerMapBase<PEER_INFO>::PeerMapBase() {
}

template <class PEER_INFO>
PeerMapBase<PEER_INFO>::~PeerMapBase() {
}

template <class PEER_INFO>
bool PeerMapBase<PEER_INFO>::exists(const UUIDPtr& uuid) {
    if (m_map.find(uuid) == 0) {
        return true;
    }
    return false;
}

template <class PEER_INFO>
bool PeerMapBase<PEER_INFO>::addPeer(PeerInfoTypePtr& peer) {
    if (!exists(peer->getPID())) {
        if (m_map.bind(peer->getPID(), peer) == 0) {
            return true;
        }
    }
    return false;
}

template <class PEER_INFO>
bool PeerMapBase<PEER_INFO>::removePeer(UUIDPtr& uuid) {
    if (m_map.unbind(uuid) == 0) {
        return true;
    }
    return false;
}

template <class PEER_INFO>
void PeerMapBase<PEER_INFO>::getPeerInfo(const UUIDPtr& uuid, PeerInfoTypePtr& peerInfoPtr) throw (PeerException&) {
    //PeerInfoPtr info;
    if (m_map.find(uuid, peerInfoPtr) == 0) {
        //return new PeerInfoPtr(info);
        return;
    }
    throw (PeerException("Peer not found!"));
}

template <class PEER_INFO>
void PeerMapBase<PEER_INFO>::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt count = m_map.current_size();
    outputStream.write_ulong(count);
    typename ACE_Hash_Map_Manager<UUIDPtr, PeerInfoTypePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();
    while (iter != m_map.end()) {
        iter->item()->serialize(outputStream);
        iter++;
    }


}

template <class PEER_INFO>
void PeerMapBase<PEER_INFO>::deserialize(InputStream& inputStream) throw (SerializationException&) {
    m_map.unbind_all();
    UInt count = 0;
    inputStream.read_ulong(count);
    for (int i = 0; i < count; i++) {
        PeerInfoType* pi = m_factory->createPeer(inputStream);
        PeerInfoTypePtr iPtr(pi);
        m_map.bind(pi->getPID(), iPtr);
    }
}



#endif	/* PEERMAPBASE_H */

