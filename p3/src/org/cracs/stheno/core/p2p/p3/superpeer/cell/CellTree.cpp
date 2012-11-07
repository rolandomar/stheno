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
 * File:   CellTree.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 15, 2010, 3:49 PM
 */

#include "CellTree.h"
#include <euryale/runtime/RuntimeConfiguration.h>
#include <stheno/core/p2p/p3/P3Config.h>

CellTree::CellTree(CellIDPtr& rootCellID): m_rootNode(0) {
    m_debugCellTree = RuntimeConfiguration::isClassDebugged("CellTree");
    int treeSpan, maxSps, maxSensors, maxMonitors;
    String path = "TREE_SPAN_0";
    P3Configuration::instance()->lookupValue(path.c_str(), treeSpan, 2);
    path = "CELL_SPS_0";
    P3Configuration::instance()->lookupValue(path.c_str(), maxSps, 2);
    path = "CELL_SENSORS_0";
    P3Configuration::instance()->lookupValue(path.c_str(), maxSensors, 80);
    path = "CELL_MONITORS_0";
    P3Configuration::instance()->lookupValue(path.c_str(), maxMonitors, 2);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MAX_SPS=%d\n",maxSps);
    m_rootNode = new CellTreeNode(rootCellID, 0, 0, 0, treeSpan, maxSps, maxSensors, maxMonitors);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ROOT NODE=%@\n",m_rootNode);
}

CellTree::CellTree(InputStream& inputStream) : m_rootNode(0) {
    m_debugCellTree = RuntimeConfiguration::isClassDebugged("CellTree");
    this->deserialize(inputStream);
}

//CellTree::CellTree(const CellTree& orig) {
//}

CellTree::~CellTree() {
    if (m_rootNode != 0) {
        delete m_rootNode;
        m_rootNode = 0;
    }
}

CellTreeNode* CellTree::getRootNode() {
    return m_rootNode;
}

list<CellTreeNode*>* CellTree::getNodesPerDepth(int depth) {
    list<CellTreeNode*> *l = new list<CellTreeNode*>();
    getNodesPerDepth_i(l, m_rootNode, depth);
    return l;
}

void CellTree::getNodesPerDepth_i(list<CellTreeNode*>* l,
        CellTreeNode* node, int depth) {
    if (node == 0) {
        return;
    }
    if (node->getDepth() == depth) {
        l->push_back(node);
        return;
    }

    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            getNodesPerDepth_i(l, node->getSibling(i), depth);
        }
    }
}

void CellTree::getCoordinators_i(CellTreeNode* startNode, list<P3PeerInfoPtr>& l) {
    if (startNode == 0) {
        return;
    }
    P3PeerInfoPtr coord;
    startNode->getCoordinator(coord);
    l.push_back(coord);
    for (int i = 0; i < startNode->getTreeSpan(); i++) {
        if (startNode->getSibling(i) != 0) {
            getCoordinators_i(startNode->getSibling(i), l);
        }
    }
}

void CellTree::getCoordinators(list<P3PeerInfoPtr>& l) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    getCoordinators_i(m_rootNode, l);
}

void CellTree::getSensors_i(CellTreeNode* startNode, list<P3PeerInfoPtr>& l) {
    if (startNode == 0) {
        return;
    }
    startNode->getPeersByType(l, P3PeerInfo::SENSOR);

    for (int i = 0; i < startNode->getTreeSpan(); i++) {
        if (startNode->getSibling(i) != 0) {
            getSensors_i(startNode->getSibling(i), l);
        }
    }
}

void CellTree::getSensors(list<P3PeerInfoPtr>& l) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    getSensors_i(m_rootNode, l);
}

bool CellTree::isUUIDInList(const UUIDPtr& uuid, list<UUIDPtr>& filter) {
    list<UUIDPtr>::iterator iter = filter.begin();
    while (iter != filter.end()) {
        if ((*iter)->isEqual(uuid)) {
            return true;
        }
        iter++;
    }
    return false;
}

void CellTree::getPoL(P3PeerInfoPtr& peer, list<UUIDPtr>& filter) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);    
    vector<int> stack;
    int min = INT_MAX;
    CellTreeNode* node = m_rootNode;
    int index = 0;
    while (node != 0) {
        //printf("Watch: CellTree::getPoL - loop node=%p index=%d\n", node, index);
        if (index == 0) { //only evaluate one time
            list<P3PeerInfoPtr> l;
            node->getPeersByType(l, P3PeerInfo::SUPERPEER_COORD);
            list<P3PeerInfoPtr>::iterator iter = l.begin();
            while (iter != l.end()) {
                if (!isUUIDInList((*iter)->getPID(), filter)) {
                    int load = (*iter)->getTotalServicesRunning();
                    if (load != -1 && load <= min) {
                        min = load;
                        peer = (*iter);
                        //printf("Watch: CellTree::getPoL - min depth=%d uuid=%s\n", node->getDepth(), peer->getPID()->toString().c_str());
                    }
                }
                iter++;
            }
        }
        bool flag = false;
        for (int i = index; i < node->getTreeSpan(); i++) {
            if (node->getSibling(i) != 0) {
                flag = true;
                index = 0;
                stack.push_back(i);
                node = node->getSibling(i);
                break;
            }
        }
        if (!flag) {//no more
            node = node->getParent();
            if (node != 0) {
                index = stack.back();
                index++;
                stack.pop_back();
            }
        }


    }
}

CellTreeNode* CellTree::getAvailableNode(Byte type) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    CellTreeNode* node = m_rootNode;
    switch (type) {
        case P3PeerInfo::SUPERPEER:
        case P3PeerInfo::SUPERPEER_COORD:
        {
            node = getAvailableNodeForSP_i(node, type);
            return node;
        }
        case P3PeerInfo::SENSOR:
        {
            return getAvailableNodeForSensor_i(node, type);
        }
        case P3PeerInfo::MONITOR:
        {
            return m_rootNode;
        }
        default:
            return 0;
    }
}

CellTreeNode* CellTree::getParentAvailableLocation() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    //CellID* root = new CellID(CellID::ROOT_CELL_ID_STR);
    CellTreeNode* temp = createCell_i(m_rootNode);
    return temp;
}

CellTreeNode* CellTree::createCell() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    //CellID* root = new CellID(CellID::ROOT_CELL_ID_STR);
    CellTreeNode* temp = createCell_i(m_rootNode);
    return temp->createSibling();
}

CellTreeNode* CellTree::createCell(CellIDPtr& cellID) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    //CellID* root = new CellID(CellID::ROOT_CELL_ID_STR);
    CellTreeNode* temp = createCell_i(m_rootNode);
    return temp->createSibling(cellID);
}

int CellTree::removeSubtree(const CellIDPtr& fid) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    CellTreeNode* fidRoot = this->getCellTreeNode(fid);
    if (fidRoot == 0) {
        return -1;
    }
    CellTreeNode* parent = fidRoot->getParent();
    if (parent != 0) {
        for (int i = 0; i < parent->getTreeSpan(); i++) {
            if (parent->getSibling(i) == fidRoot) {
                parent->setSibbling(i, 0);
                break;
            }
        }
    }
    return removeSubtree_i(fidRoot);
}

int CellTree::removeSubtree() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    CellTreeNode* fidRoot = this->m_rootNode;
    if (fidRoot == 0) {
        return -1;
    }
    for (int i = 0; i < fidRoot->getTreeSpan(); i++) {
        if (fidRoot->getSibling(i) != 0) {
            removeSubtree(fidRoot->getSibling(i)->getCellID());
        }
    }
    return 0;
}

int CellTree::removeSubtree_i(CellTreeNode * node) {
    if (node == 0) {
        return 0;
    }
    /*if (!node->hasAllSiblings()) {
        //TODO:
        delete node;
        return 0;
    }*/
    for (int i = 0; i < node->getTreeSpan(); i++) {
        removeSubtree_i(node->getSibling(i));
    }
    //TODO:
    delete node;
    return 0;
}

CellTreeNode * CellTree::getCellTreeNode(const UUIDPtr & fid) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    if (fid.null()) {
        return 0;
    }
    CellID* cellID_ = new CellID(*fid.get());
    CellIDPtr cellID(cellID_);
    return getCellTreeNode(cellID);
}

CellTreeNode * CellTree::getCellTreeNode(const CellIDPtr & fid) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    return getCellTreeNode_i(fid, m_rootNode);
}

CellTreeNode * CellTree::getCellTreeNode_i(const CellIDPtr& fid, CellTreeNode * node) {
    if (node->getCellID()->isEqual(*fid)) {
        return node;
    }
    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            CellTreeNode* temp = getCellTreeNode_i(fid, node->getSibling(i));
            if (temp != 0) {
                return temp;
            }
        }
    }
    return 0;
}

void CellTree::pruneSubtrees(list<UUIDPtr>& l) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: pruneSubtrees()\n")));
    CellTreeNode* node = m_rootNode;
    for (int i = 0; i < node->getTreeSpan(); i++) {
        CellTreeNode* child = node->getSibling(i);
        if (child != 0) {
            P3PeerInfoPtr coordPtr;
            child->getCoordinator(coordPtr);
            UUIDPtr coord = coordPtr->getPID();
            list<UUIDPtr>::iterator iter = l.begin();
            bool flag = false;
            while (iter != l.end()) {
                if ((*iter)->isEqual(coord)) {
                    flag = true;
                    break;
                }
                iter++;
            }
            if (!flag) {
                if (m_debugCellTree) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: pruneSubtrees() - remove chidCellID(%s)\n"), child->getCellID()->toString().c_str()));
                }
                m_rootNode->setSibbling(i, 0);
                delete child;
            }

        }
    }
}

int CellTree::addSubtree(CellTree* subTree, CellIDPtr& parentCellID) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    CellTreeNode* node = 0;
    node = this->getCellTreeNode(subTree->getRootNode()->getCellID());
    if (node != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addSubtree() - already exists\n")));
        //already exists
        delete subTree;
        return 0;
    }

    if (parentCellID.null()) {
        if (m_debugCellTree) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addSubtree() - root Node\n")));
        }
        node = this->m_rootNode;
    } else {
        if (m_debugCellTree) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addSubtree() - non-root parent(%s) cell(%s)\n"),
                    parentCellID->toString().c_str(),
                    subTree->getRootCellID()->toString().c_str()));
        }
        node = this->getCellTreeNode(parentCellID);
    }
    if (node == 0) {
        if (m_debugCellTree) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addSubtree() - no node!\n")));
        }
        delete subTree;
        return -1;
    }
    for (int i = 0; i < node->getTreeSpan(); i++) {
        CellTreeNode* child = node->getSibling(i);
        if (child == 0) {
            CellTreeNode* rootSubTreeNode = subTree->dettach();
            rootSubTreeNode->setLocation(node->getDepth() + 1, i);
            if (m_debugCellTree) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addSubtree() - subTree@(%s)\n"), rootSubTreeNode->toString().c_str()));
            }
            node->setSibbling(i, rootSubTreeNode);
            if (m_debugCellTree) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addSubtree() - setSibling cellID(%s) parent(%s)\n"),
                        rootSubTreeNode->toString().c_str(),
                        node->getCellID()->toString().c_str()));

                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addSubtree() - CHECK cellID(%s) parent(%s)\n"),
                        node->getSibling(i)->toString().c_str(),
                        node->getCellID()->toString().c_str()));
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addSubtree() - tree:\n%s\n"), toString().c_str()));
            }
            return 0;
        }
    }
    delete subTree;
    return -1;

}

int CellTree::addPeer(P3PeerInfo* peer, CellIDPtr & parentCellID) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    CellTreeNode* node = this->getCellTreeNode(peer->getFID());
    if (node == 0) {
        node = this->getCellTreeNode(parentCellID);
        if (node == 0) {
            if (m_debugCellTree) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: addPeer(P3PeerInfo* peer, CellIDPtr & parentCellID) - Failed to add Peer!!!\n")));
            }
            return -1;
        }
        CellID* cellID = new CellID(*(peer->getFID()));
        CellIDPtr cellIDPtr(cellID);
        node = node->createSibling(cellIDPtr);
        if (node == 0) {
            return -1;
        }
    }
    return node->add(peer);
}

int CellTree::addPeer(P3PeerInfo * peer) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    //printf("KKKKKKKKKKKKKKKKKKKKKK Add peer discovery=%p\n", peer->getDiscoverySAP().get());
    CellTreeNode* node = this->getCellTreeNode(peer->getFID());
    if (node == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: addPeer(P3PeerInfo * peer) - Failed to add Peer!!!\n")));
        return -1;
    }
    return node->add(peer);
    /*CellTreeNode* node = getAvailableNode(peer->getType());
    if (node != 0) {
        if (node->add(peer) == 0) {
            return node;
        }
    } else {
        CellTreeNode* addNode = this->createCell();
        if (addNode->add(peer) == 0) {
            return addNode;
        } else {
            return 0;
        }
    }
    return 0;*/
}

int CellTree::getPeerByLevel(int treeLevel, P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    return getPeerByLevel_i(m_rootNode, treeLevel, info);
}

int CellTree::getRandomPeer(P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    list<P3PeerInfoPtr> l;
    getRandomPeer_i(m_rootNode, l);
    int size = l.size();
    if (size == 0) {
        return -1;
    }
    int pos = ACE_OS::rand() % size;
    int i = 0;
    list<P3PeerInfoPtr>::iterator iter = l.begin();
    while ((iter != l.end())) {
        if (i == pos) {
            info = *iter;
            return 0;
        }
        iter++;
        i++;
    }
    return 0;
}

int CellTree::getPeer(const UUIDPtr& pid, P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    return getPeer_i(m_rootNode, pid, info);
}

int CellTree::getPeerByIID(const UUIDPtr& sid, const UUIDPtr& iid, SAPInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    return getPeerByIID_i(m_rootNode, sid, iid, info);
}

int CellTree::getPeerByIID_i(CellTreeNode* node, const UUIDPtr& sid, const UUIDPtr& iid, SAPInfoPtr& info) {
    printf("getPeerByIID_i()\n");
    if (node == 0) {
        printf("getPeerByIID_i() - ==0, ret -1\n");
        return -1;
    }

    if (node->getPeerByIID(sid, iid, info) == 0) {
        printf("getPeerByIID_i() - found!\n");
        return 0;
    }

    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            int ret = getPeerByIID_i(node->getSibling(i), sid, iid, info);
            if (ret == 0) {
                printf("getPeerByIID_i()  child has it\n");
                return 0;
            }
        }
    }
    printf("getPeerByIID_i() - no child had it , ret -1\n");
    return -1;
}

int CellTree::getPeerByLevel_i(CellTreeNode* node, int treeLevel, P3PeerInfoPtr& info) {
    if (node == 0) {
        return -1;
    }    
    
    if (node->getDepth() == treeLevel) {
        //ok it's this level
        list<P3PeerInfoPtr> l;
        node->getPeersByType(l, P3PeerInfo::SUPERPEER);
        int size = l.size();
        if (size == 0) {
            return 0;
        }
        int pos = ACE_OS::rand() % size;
        int i = 0;
        list<P3PeerInfoPtr>::iterator iter = l.begin();
        while ((iter != l.end())) {
            if (i == pos) {
                info = *iter;
                //printf("\n\n\n\n\n\n\nsize=%d pos=%d depth=%d level=%d %s %s\n",size,pos,node->getDepth(),treeLevel,info->getPID()->toString().c_str(),info->getFID()->toString().c_str());
                printf("%s\n",this->toString().c_str());
                return 0;                
            }
            iter++;
            i++;
        }
        //printf("\n\n\n\n\n\n\n FAILLLLLLLLLLLLLLLLLL\n");
        return -1;
    }

    list<P3PeerInfoPtr> l;
    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            P3PeerInfoPtr subInfo;
            int ret = getPeerByLevel_i(node->getSibling(i), treeLevel, subInfo);
            if (ret == 0) {
                l.push_back(subInfo);
            }
        }
    }
    int size = l.size();
    if (size == 0) {
        return 0;
    }
    int pos = ACE_OS::rand() % size;
    int i = 0;
    list<P3PeerInfoPtr>::iterator iter = l.begin();
    while ((iter != l.end()))
    {
        if (i == pos) {
            info = *iter;
            return 0;
        }
        iter++;
        i++;
    }
    return -1;
}

int CellTree::getRandomPeer_i(CellTreeNode* node, list<P3PeerInfoPtr>& l) {
    if (node == 0) {
        return 0;
    }
    node->getPeersByType(l, P3PeerInfo::SUPERPEER);
    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            getRandomPeer_i(node->getSibling(i), l);
        }
    }
    return 0;
}

int CellTree::getPeer_i(CellTreeNode* node, const UUIDPtr& pid, P3PeerInfoPtr& info) {
    if (node == 0) {
        return -1;
    }

    if (node->getPeer(pid, info) == 0) {
        return 0;
    }

    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            int ret = getPeer_i(node->getSibling(i), pid, info);
            if (ret == 0) {
                return 0;
            }
        }
    }
    return -1;
}

CellIDPtr & CellTree::getRootCellID() {
    return m_rootNode->getCellID();
}

String & CellTree::toString() {
    //return m_rootNode->toString();
    return toStringPerLevel();
}

String& CellTree::toStringPerLevel() {
    //m_strPerLevel = "\0";
    stringstream ss;
    int size = 0;
    for (int i = 0; i < 10000; i++) {
        list<CellTreeNode*>* l = getNodesPerDepth(i);
        list<CellTreeNode*>::iterator iter = l->begin();
        if (l->size() == 0) {
            delete l;
            break;
        }
        while (iter != l->end()) {
            if (i < MAX_DEPTH) {
                //m_strPerLevel += (*iter)->toString() + "\n";
                ss << (*iter)->toString() << "\n";
            }
            size++;
            iter++;
        }
        delete l;
        //m_strPerLevel += "\n";
    }
    //m_strPerLevel += "\nTotalSize=";
    ss << "\nTotalSize=";
    //m_strPerLevel += Integer::toUnsignedString(size) + "\n";
    ss << Integer::toUnsignedString(size) << "\n";
    m_strPerLevel = ss.str().c_str();
    return m_strPerLevel;
}

CellTreeNode * CellTree::removeCell(CellIDPtr & cellID) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    CellTreeNode* node = getTreeNode(cellID);
    if (node != 0) {
        CellTreeNode* parent = getParentTreeNode(cellID);
        if (parent == 0) {
            //throw
            return 0;
        }
        CellTreeNode* promoted = findBottomNode(parent, node);
        if (promoted == 0 || promoted == node) {
            //no siblings
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)no promotion!\n");
            return node;
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)promoted! parent=%s\n", parent->toString().c_str());
        parent->replaceSibling(node, promoted);
        return node;
    } else {
        //throw
        return 0;
    }
}

CellTreeNode * CellTree::createCell_i(CellTreeNode * node) {
    if (node == 0) {
        return 0;
    }
    if (!node->hasAllSiblings()) {
        return node;
    }

    CellTreeNode* min = 0;
    for (int i = 0; i < node->getTreeSpan(); i++) {
        CellTreeNode* temp = createCell_i(node->getSibling(i));
        if (temp != 0) {
            if (min == 0) {
                min = temp;
            } else {
                if (min->getDepth() > temp->getDepth()) {
                    min = temp;
                }
            }
        }
    }
    /*if (min != 0) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)====> createCell_i(%s) - min\n", min->getCellID()->toString().c_str());
    } else {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)====> min=0\n");
    }*/
    return min;
}

CellTreeNode * CellTree::getAvailableNodeForSP_i(CellTreeNode* node, Byte type) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T).....getAvailableNodeForSP_i %@\n",node);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T).....getAvailableNodeForSP_i %d\n",node->getDepth());
    if (node == 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T).....getAvailableNodeForSP_i NULL\n");
        return 0;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getAvailableNodeForSP_i before checking available nodes %@\n",node);
    if (node->hasAvailableSlotsForPeer(type)) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)----getAvailableNodeForSP_i %s %d has SPACE\n"), node->getCellID()->toString().c_str(), type));
        return node;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getAvailableNodeForSP_i before sibling\n");
    for (int i = 0; i < node->getTreeSpan(); i++) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getAvailableNodeForSP_i before sibling %d\n",i);
        if (node->getSibling(i) != 0) {

            CellTreeNode* ret =
                    getAvailableNodeForSP_i(node->getSibling(i), type);
            if (ret != 0) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)getAvailableNodeForSP_i %s %d ret\n"), ret->getCellID()->toString().c_str(), type));
                return ret;
            } else {
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)+++getAvailableNodeForSP_i %s %d NULL\n",node->getCellID()->toString().c_str(),type);
            }
        } else {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)***getAvailableNodeForSP_i %s SPan=%d is null\n",node->getCellID()->toString().c_str(),i);
        }
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)----getAvailableNodeForSP_i %s %d NONE\n",node->getCellID()->toString().c_str(),type);
    return 0;
}

CellTreeNode * CellTree::getAvailableNodeForSensor_i(CellTreeNode* node, Byte type) {
    if (node == 0) {
        return 0;
    }
    CellTreeNode* cur = 0;
    int maxDepth = -1;
    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            CellTreeNode* ret = getAvailableNodeForSensor_i(node->getSibling(i), type);
            if (ret != 0) {
                if (ret->getDepth() > maxDepth) {
                    maxDepth = ret->getDepth();
                    cur = ret;
                }
            }
        }
    }

    if (cur == 0) {
        if (node->hasAvailableSlotsForPeer(type)) {
            return node;
        }
    }
    return cur;
}

CellTreeNode * CellTree::findBottomNode(CellTreeNode* parent, CellTreeNode * node) {
    if (node == 0) {
        return 0;
    }
    if (node->countSiblings() == 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)null replace\n");
        parent->replaceSibling(node, 0);
        return node;
    }
    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            return findBottomNode(node, node->getSibling(i));
        }
    }
    return 0;
}

CellTreeNode * CellTree::getParentTreeNode(CellIDPtr & cellID) {
    return getParentTreeNode(cellID.get());
}

CellTreeNode * CellTree::getParentTreeNode(CellID * cellID) {
    CellTreeNode* node = m_rootNode;
    if (*cellID == *(node->getCellID().get())) {
        return 0;
    }
    return getParentTreeNode_i(0, node, cellID);
}

CellTreeNode * CellTree::getParentTreeNode_i(CellTreeNode* parent, CellTreeNode* node, CellID * cellID) {
    if (node == 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getTreeNode_i: null!\n");
        return 0;
    }

    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getTreeNode_i: sibling %d!\n", i);
            if (*(node->getSibling(i)->getCellID().get()) == *cellID) {
                return node;
            }

            CellTreeNode* temp = getParentTreeNode_i(node, node->getSibling(i), cellID);
            if (temp != 0) {
                return temp;
            }
        }
    }
    return 0;
}

CellTreeNode * CellTree::getTreeNode(CellIDPtr & cellID) {
    return getTreeNode(cellID.get());
}

CellTreeNode * CellTree::getTreeNode(CellID * cellID) {
    /*vector<int> lvls;
    cellID->getCellLevels(lvls);
    int depth = cellID->getCellDepth();
    CellTreeNode* node = &m_rootNode;
    for (int i = 1; i < depth; i++) {
        if (node != 0) {
            node = node->getSibling(lvls[i]-1);
        } else {
            break;
        }
    }
    return node;*/
    CellTreeNode* node = m_rootNode;
    return getTreeNode_i(node, cellID);
}

CellTreeNode * CellTree::getTreeNode_i(CellTreeNode* node, CellID * cellID) {
    if (node == 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getTreeNode_i: null!\n");
        return 0;
    }
    if (*(node->getCellID().get()) == *cellID) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getTreeNode_i: == %s\n", cellID->toString().c_str());
        return node;
    }
    for (int i = 0; i < node->getTreeSpan(); i++) {
        if (node->getSibling(i) != 0) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getTreeNode_i: sibling %d!\n", i);
            CellTreeNode* temp = getTreeNode_i(node->getSibling(i), cellID);
            if (temp != 0) {
                return temp;
            }
        }
    }
    return 0;
}

void CellTree::serialize(OutputStream& outputStream) throw (SerializationException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_rootNode->serialize(outputStream);
}

void CellTree::deserialize(InputStream& inputStream) throw (SerializationException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_rootNode = new CellTreeNode(0, inputStream);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:**************CellTree %@)\n"),m_rootNode));
}

CellTreeNode* CellTree::dettach() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    CellTreeNode* node = m_rootNode;
    m_rootNode = 0;
    return node;
}