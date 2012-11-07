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
 * File:   LeafPeer.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 30, 2010, 12:27 PM
 */

#ifndef LEAFPEER_H
#define	LEAFPEER_H

#include <stheno/core/p2p/p3/P3.h>


class P3LeafMesh;
class LeafDiscovery;
class StreamServer;
class ActuatorServer;
class LeafPeer : public P3 {
public:
    LeafPeer();
    LeafPeer(int type);
    LeafPeer(SthenoCoreInterface* runtime);
    LeafPeer(int type,SthenoCoreInterface* runtime);
    virtual ~LeafPeer();
    
    void open() throw (OverlayException&);
    int getType();
    virtual void getPID(UUIDPtr& uuid);
        
    virtual void getDiscovery(DiscoveryPtr& discoveryPtr) throw(OverlayException&);
    virtual void getMesh(MeshPtr& meshPtr) throw (OverlayException&);
    virtual void getFaultTolerance(FaultTolerancePtr& ftPTr) throw (OverlayException&);
      
    LeafDiscovery* getP3Discovery();
    P3LeafMesh* getP3MeshService();
    
protected:
    int m_type;
    MeshPtr m_meshPtr;
    P3LeafMesh* m_meshService;
    LeafDiscovery* m_discoveryService;
    StreamServer* m_streamServer;
    ActuatorServer* m_actuatorServer;
    
    DiscoveryPtr m_discoveryPtr;
    
    virtual void close_i() throw (OverlayException&);
    virtual void open_i() throw (OverlayException&);

};

#endif	/* LEAFPEER_H */

