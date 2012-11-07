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
 * File:   OverlayInterface.h
 * Author: rmartins
 *
 * Created on October 25, 2010, 7:26 PM
 */

#ifndef OVERLAYINTERFACE_H
#define	OVERLAYINTERFACE_H

#include <stheno/core/p2p/discovery/DiscoveryInterface.h>
#include <stheno/core/p2p/mesh/MeshInterface.h>
//#include <stheno/core/SthenoCore.h>
#include <stheno/core/p2p/Overlay.h>
class SthenoCore;
class OverlayInterface {
public:
    friend class SthenoCore;
    
    virtual ~OverlayInterface();
    
    DiscoveryInterfacePtr& getDiscovery();
    MeshInterfacePtr& getMesh();
    
    
private:
    OverlayInterface(SthenoCore* core,DiscoveryInterfacePtr& di,MeshInterfacePtr& mi);
    
    OverlayPtr m_overlayPtr;
    DiscoveryInterfacePtr m_discoveryPtr;
    MeshInterfacePtr m_meshPtr;
    
    SthenoCore* m_core;

};

#endif	/* OVERLAYINTERFACE_H */

