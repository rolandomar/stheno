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
 * File:   P3.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 30, 2010, 11:50 AM
 */

#include "P3.h"
#include <stheno/Stheno.h>
#include <stheno/core/p2p/p3/mesh/P3Mesh.h>
#include <stheno/core/p2p/p3/discovery/P3Discovery.h>
#include <stheno/core/p2p/p3/superpeer/discovery/SuperPeerDiscovery.h>

P3::P3(SthenoCoreInterface* runtime):Overlay(runtime) {
        
}

P3::~P3() {
}



/*SuperPeerDiscovery* P3::getP3Discovery(){
    return m_discoveryService;
}

 
 P3Mesh* P3::getP3MeshService(){
     return m_meshService;
 }*/