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
 * File:   P3.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 30, 2010, 11:50 AM
 */

#ifndef P3_H
#define	P3_H
#include "ace/svc_export.h"
#include "ace/Service_Config.h"
#include <stheno/core/p2p/Overlay.h>

class Stheno;

class P3Mesh;
class P3Discovery;
class P3FaultTolerance;
class SuperPeerDiscovery;
class P3 : public Overlay {
public:
    P3(){}
    P3(SthenoCoreInterface* runtime);
    virtual ~P3();

    
    //virtual SuperPeerDiscovery* getP3Discovery();
    //virtual P3Mesh* getP3MeshService();
    
protected:
    //SuperPeerDiscovery* m_discoveryService;
    //P3Mesh* m_meshService;
};


#endif	/* P3_H */

