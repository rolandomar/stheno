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
 * File:   PeerMap.h
 * Author: rmartins
 *
 * Created on June 18, 2010, 11:45 AM
 */

#ifndef PEERMAP_H
#define	PEERMAP_H

#include <stheno/core/p2p/common/PeerMapBase.h>
#include <stheno/core/p2p/common/PeerInfo.h>

//User interface
class PeerMap: public PeerMapBase<PeerInfo>{
public:
    PeerMap();
    virtual ~PeerMap();
};

typedef ACE_Strong_Bound_Ptr<PeerMap,ACE_SYNCH_MUTEX> PeerMapPtr;

#endif	/* PEERMAP_H */

