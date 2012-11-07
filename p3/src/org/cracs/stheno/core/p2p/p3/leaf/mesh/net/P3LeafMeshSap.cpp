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
 * File:   P3LeafMeshSap.cpp
 * Author: rmartins
 * 
 * Created on October 11, 2010, 2:17 PM
 */

#include "P3LeafMeshSap.h"



#include <stheno/core/p2p/p3/leaf/mesh/P3LeafMesh.h>
#include <stheno/core/p2p/p3/leaf/mesh/net/LeafAcceptor.h>

P3LeafMeshSap::P3LeafMeshSap(P3LeafMesh* mesh) :
m_mesh(mesh) {
}

P3LeafMeshSap::~P3LeafMeshSap() {
    close();
}

LeafAcceptor* P3LeafMeshSap::createAcceptor() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO--------------------------: P3LeafMeshSap::createAcceptor() \n")));
    LeafAcceptor* acceptor = new LeafAcceptor(m_mesh);
    printf("ACCEPTOR=%p %d\n",acceptor,acceptor->get_handle());
    return acceptor;
}
