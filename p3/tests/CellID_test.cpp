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
 * File:   Runtime.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 10, 2010, 1:00 PM
 */

#include "CellID_test.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CellID_test);

#include <stheno/Stheno.h>
#include <stdio.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <ace/String_Base.h>

#include <stheno/core/p2p/p3/superpeer/cell/CellTree.h>

CellID_test::CellID_test() {
}

CellID_test::~CellID_test() {
}

void CellID_test::setUp() {

}

void CellID_test::tearDown() {

}

void CellID_test::testMethod() {
    //Stheno runtime;
    
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)\nruntimeTest\n")));
    //CellID* root = new CellID(CellID::ROOT_CELL_ID_STR);
    CellID* root = new CellID(CellID::INVALID_ID_STR);
    //CellIDPtr rootCellID(root);
    UUIDPtr rootCellID(root);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)\nruntimeTest2\n")));

    //CellTree tree(CellID::ROOT_CELL_ID);
    CellTree tree(CellID::INVALID_CELL_ID);

    CellTreeNode* temp = 0;
    CellTreeNode* node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    temp = node;
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    node = tree.createCell();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%@)\n"), node));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)node(%s)\n"), node->getCellID()->toString().c_str()));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)\n\n\n")));


    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Tree:\n%s"), tree.toStringPerLevel().c_str()));
    CellTreeNode* removeNode = tree.removeCell(temp->getCellID());
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)oldNode=%s\n"), removeNode->toString().c_str()));
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)removed=%@\n"), removeNode));
    delete removeNode;
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Tree:\n%s"), tree.toStringPerLevel().c_str()));



    UUIDPtr pid1(UUID::generateUUID());
    UUIDPtr fid(UUID::generateUUID());
    EndpointPtr cellEndpoint;
    SAPInfoPtr meshSAP;
    SAPInfoPtr discoverySAP;
    SAPInfoPtr ftSAP;
    P3PeerInfo *pee1 = new P3PeerInfo(P3PeerInfo::SUPERPEER, pid1, fid, cellEndpoint,meshSAP, discoverySAP, ftSAP);
    UUIDPtr pid2(UUID::generateUUID());
    P3PeerInfo *pee2 = new P3PeerInfo(P3PeerInfo::SUPERPEER, pid2, fid, cellEndpoint,meshSAP, discoverySAP, ftSAP);
    UUIDPtr pid3(UUID::generateUUID());
    P3PeerInfo *pee3 = new P3PeerInfo(P3PeerInfo::SUPERPEER, pid3, fid, cellEndpoint,meshSAP, discoverySAP, ftSAP);
    /*CellTreeNode *addNode = 0;
    addNode = tree.addPeer(pee1);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)addNOde=%s\n", addNode->getCellID()->toString().c_str());
    addNode = tree.addPeer(pee2);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)addNOde=%s\n", addNode->getCellID()->toString().c_str());
    addNode = tree.addPeer(pee3);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)addNOde=%s\n", addNode->getCellID()->toString().c_str());*/

    /*UUID uuid("00000000000000000000000000000000");
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)->%s\n",uuid.toString().c_str());
    CellID id("01000000000000000000000000000000");
    CellID id2("02000000000000000000000000000000");
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)DEPTH=%d char_at_depth=%d\n",id.getCellDepth(),id.getCellSpan(id.getCellDepth()));
    vector<int> lvls;
    id.getCellLevels(lvls);
    //for(int i=0; i < lvls.size(); i++){
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Lvl[%d]=%d\n",i,lvls[i]);
    //}
    
    CellID* next = id.getNextGroupID_2();
    CellID* next2 = next->getNextGroupID_2();
    CellID* next3 = next2->getNextGroupID_2();
    CellID* next4 = next3->getNextGroupID_2();
    CellID* next5 = next4->getNextGroupID_2();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Next 0=%s 1=%s 2=%s 3=%s 4=%s 5=%s\n",
            id.toString().c_str(),next->toString().c_str(),next2->toString().c_str(),next3->toString().c_str(),
            next4->toString().c_str(),next5->toString().c_str());
    
    CellID* parent = id.getParentGroupID();
    bool isDescent = id.belongsToCellTree(&id2);
    if(parent != 0){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ID=%s parent=%s isDescent=%d\n",id.toString().c_str(),parent->toString().c_str(),isDescent);
    }else{
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ID=%s parent=0 isDescent=%d\n",id.toString().c_str(),isDescent);
    }
    
    
    delete next;
    delete next2;
    delete next3;
    delete next4;
    delete next5;
    if(parent != 0)
        delete parent;
    UUID uuid2("094c57041a707dc479c86059be42ac34");
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)->%s\n",uuid2.toString().c_str());
     */




    CPPUNIT_ASSERT(true);
}
