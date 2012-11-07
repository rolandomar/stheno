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
 * File:   MeshUpdateInfo.h
 * Author: rmartins
 *
 * Created on September 29, 2010, 12:54 PM
 */

#ifndef MESHUPDATEINFO_H
#define	MESHUPDATEINFO_H

#include <stheno/core/p2p/p3/mesh/InfoUpdate.h>
#include <stheno/core/p2p/common/ServiceInstanceInfo.h>

class MeshUpdateInfo: public InfoUpdate {
public:
    static const UInt MESH_UPDATEINFO_TYPE = 0x902322;
    
    MeshUpdateInfo(UInt operation);
    MeshUpdateInfo(const MeshUpdateInfo& orig);
    virtual ~MeshUpdateInfo();
};
#endif	/* MESHUPDATEINFO_H */

