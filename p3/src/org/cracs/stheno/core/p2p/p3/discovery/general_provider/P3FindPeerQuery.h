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
 * File:   P3FindPeerQuery.h
 * Author: rmartins
 *
 * Created on November 18, 2010, 7:09 PM
 */

#ifndef P3FindPeerQuery_H
#define	P3FindPeerQuery_H

#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/common/io/ByteInputStream.h>

class P3FindPeerQuery : public DiscoveryQuery {
public:

    static const int P3FINDPEER_QUERY = 0x192134;    
    
    P3FindPeerQuery(int treeLevel);
    P3FindPeerQuery(DiscoveryQuery* query);
    P3FindPeerQuery(const P3FindPeerQuery& orig);
    virtual ~P3FindPeerQuery();

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
    
    int getTreeLevel();
protected:
    int m_treeLevel;
    
    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);    
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);
};



#endif	/* P3FindPeerQuery_H */

