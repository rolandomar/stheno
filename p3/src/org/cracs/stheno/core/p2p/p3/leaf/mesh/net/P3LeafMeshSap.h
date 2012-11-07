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
 * File:   P3LeafMeshSap.h
 * Author: rmartins
 *
 * Created on October 11, 2010, 2:17 PM
 */

#ifndef P3LEAFMESHSAP_H
#define	P3LEAFMESHSAP_H

#include <euryale/net/endpoint/SAP.h>
#include <stheno/core/p2p/p3/leaf/mesh/net/LeafAcceptor.h>

class P3LeafMesh;
class P3LeafMeshSap: public SAP<LeafAcceptor> {
public:
    P3LeafMeshSap(P3LeafMesh* mesh);    
    virtual ~P3LeafMeshSap();        
    
protected:
    P3LeafMesh* m_mesh;    
    //virtual abstract method impl
    LeafAcceptor* createAcceptor();        
};

#endif	/* P3LEAFMESHSAP_H */

