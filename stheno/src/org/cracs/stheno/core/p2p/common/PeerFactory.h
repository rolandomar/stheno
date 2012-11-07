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
 * File:   PeerFactory.h
 * Author: rmartins
 *
 * Created on August 3, 2010, 2:31 PM
 */

#ifndef PEERFACTORY_H
#define	PEERFACTORY_H

#include <stheno/core/p2p/common/PeerInfo.h>

template <class PEER_INFO>
class PeerFactory {
public:
    typedef PEER_INFO PeerInfoType;
    PeerFactory();    
    virtual ~PeerFactory();
    
    virtual PeerInfoType* createPeer(UInt type) = 0;
    virtual PeerInfoType* createPeer(InputStream& inputStream) = 0;
};

#endif	/* PEERFACTORY_H */

