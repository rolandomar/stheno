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
 * File:   CellTreeNode.cpp
 * Author: rmartins
 * 
 * Created on June 30, 2010, 3:58 PM
 */

#include "CellTreeNode.h"
#include <euryale/runtime/RuntimeConfiguration.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellTree.h>
#include <stheno/core/p2p/p3/P3Config.h>

CellTreeNode::CellTreeNode(CellIDPtr& cellID, CellTreeNode* parent, int spanNo,
        int depth, int treeSpan, int maxSP, int maxSensors, int maxMonitor) : m_cellID(cellID) {
    m_debugCellTreeNode = RuntimeConfiguration::isClassDebugged("CellTreeNode");
    printf("==========================>CellTreeNode(%p)\n",this);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode(%s)\n",cellID->toString().c_str());
    m_depth = depth; //m_cellID->getCellDepth();
    m_spanNo = spanNo;
    m_parent = parent;
    //m_span = m_cellID->getCellSpan(m_depth);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode %s depth=%d span=%d\n",cellID->toString().c_str(),m_depth,m_span);
    m_treeSpan = treeSpan;
    m_maxSPs = maxSP;
    m_maxSensors = maxSensors;
    m_maxMonitor = maxMonitor;
    m_siblings = new CellTreeNode*[m_treeSpan];
    for (int i = 0; i < treeSpan; i++) {
        m_siblings[i] = 0;
    }
}

CellTreeNode::CellTreeNode(CellTreeNode* parent, InputStream& inputStream) : m_parent(parent) {
    m_debugCellTreeNode = RuntimeConfiguration::isClassDebugged("CellTreeNode");
    printf("==========================>CellTreeNode(%p)\n",this);
    this->deserialize(inputStream);
}

CellTreeNode::~CellTreeNode() {
  printf("==========================>~CellTreeNode(%p)\n",this);
    /*for (int i = 0; i < m_treeSpan; i++) {
        delete m_siblings[i];
    }*/
    delete [] m_siblings;
    //ListHelper<P3PeerInfo*>::clearList(m_sps);
    //ListHelper<P3PeerInfo*>::clearList(m_sensors);
    //ListHelper<P3PeerInfo*>::clearList(m_monitors);
    m_sps.clear();
    m_sensors.clear();
    m_monitors.clear();
}

void CellTreeNode::setSpanNo(int spanNo) {
    m_spanNo = spanNo;
}

int CellTreeNode::getSpanNo() {
    return m_spanNo;
}

void CellTreeNode::setParent(CellTreeNode* parent) {
    m_parent = parent;
}

void CellTreeNode::getCoordinator(P3PeerInfoPtr& info) {
    if (m_sps.size() == 0) {
        return;
    }
    info = m_sps.front();
}

CellTreeNode* CellTreeNode::getParent() {
    return m_parent;
}

void CellTreeNode::setDepth(int depth) {
    m_depth = depth;
}

void CellTreeNode::setPort(Short port) {
    m_port = port;
}


//->01 -->011 -->012

void CellTreeNode::createSibling(int spanNo, CellIDPtr& cellIDPtr) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::createSibling(no) %s\n",this->m_cellID->toString().c_str());
    if (m_siblings[spanNo] == 0) {
        int treeSpan, maxSps, maxSensors, maxMonitors;
        String path = "TREE_SPAN_" + Integer::toUnsignedString(m_depth + 1);
        P3Configuration::instance()->lookupValue(path.c_str(), treeSpan, 2);
        path = "CELL_SPS_" + Integer::toUnsignedString(m_depth + 1);
        P3Configuration::instance()->lookupValue(path.c_str(), maxSps, 2);
        path = "CELL_SENSORS_" + Integer::toUnsignedString(m_depth + 1);
        P3Configuration::instance()->lookupValue(path.c_str(), maxSensors, 80);
        path = "CELL_MONITORS_" + Integer::toUnsignedString(m_depth + 1);
        P3Configuration::instance()->lookupValue(path.c_str(), maxMonitors, 2);
        m_siblings[spanNo] = new CellTreeNode(cellIDPtr, this, spanNo, m_depth + 1,
                treeSpan, maxSps, maxSensors, maxMonitors);
    }
}

bool CellTreeNode::sliblingExists(int spanNo) {
    return (m_siblings[spanNo] != 0);
}

void CellTreeNode::removeSibling(int spanNo) {
    if (m_siblings[spanNo] != 0) {
        delete m_siblings[spanNo];
        m_siblings[spanNo] = 0;
    }
}

CellTreeNode* CellTreeNode::getSibling(int spanNo) {
    if (m_siblings[spanNo] != 0) {
        return m_siblings[spanNo];
    }
    return 0;
}

CellTreeNode* CellTreeNode::setSibbling(int spanNo, CellTreeNode* newChild) {
    CellTreeNode* temp = m_siblings[spanNo];
    m_siblings[spanNo] = newChild;
    if (newChild != 0) {
        m_siblings[spanNo]->setParent(this);
    }
    return temp;
}

void CellTreeNode::replaceSibling(CellTreeNode* node, CellTreeNode* newNode) {
    for (int i = 0; i < m_treeSpan; i++) {
        /*if (m_siblings[i] != 0) {
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)R=%s S=%s\n", node->getCellID()->toString().c_str(), m_siblings[i]->getCellID()->toString().c_str());
        }*/
        if (m_siblings[i] == node) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)replaceSibling newnode=%@!\n", newNode);
            m_siblings[i] = newNode;
            if (newNode != 0) {
                newNode->setDepth(node->getDepth());
                newNode->setSpanNo(node->getSpanNo());
                newNode->setParent(node->getParent());
                for (int i = 0; i < m_treeSpan; i++) {
                    newNode->setSibbling(i, node->getSibling(i));
                    if (newNode->getSibling(i) != 0) {
                        newNode->getSibling(i)->setParent(newNode);
                    }
                }
            }
            return;
        }
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)no replaceSibling!\n");
}

bool CellTreeNode::hasAllSiblings() {
    for (int i = 0; i < m_treeSpan; i++) {
        if (m_siblings[i] == 0) {
            return false;
        }
    }
    return true;
}

int CellTreeNode::countSiblings() {
    int count = 0;
    for (int i = 0; i < m_treeSpan; i++) {
        if (m_siblings[i] != 0) {
            count++;
        }
    }
    return count;
}

CellTreeNode* CellTreeNode::createSibling(CellIDPtr& cellIDPtr) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::createSibling() %s\n",this->m_cellID->toString().c_str());
    if (!hasAllSiblings()) {
        for (int i = 0; i < m_treeSpan; i++) {
            if (m_siblings[i] == 0) {
                createSibling(i, cellIDPtr);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)****CellTreeNode::createSibling() %s Span=%d empty\n",
                //m_siblings[i]->m_cellID->toString().c_str(),i);
                return m_siblings[i];
            }
        }
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::createSibling() %s is Full\n",this->m_cellID->toString().c_str());
    return 0;
}

CellTreeNode* CellTreeNode::createSibling() {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::createSibling() %s\n",this->m_cellID->toString().c_str());
    if (!hasAllSiblings()) {
        for (int i = 0; i < m_treeSpan; i++) {
            if (m_siblings[i] == 0) {
                UUID* generateUUID = CellID::generateUUID();
                CellID* sibling = new CellID(*generateUUID);
                delete generateUUID;
                CellIDPtr cellIDPtr(sibling);
                createSibling(i, cellIDPtr);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)****CellTreeNode::createSibling() %s Span=%d empty\n",
                //m_siblings[i]->m_cellID->toString().c_str(),i);
                return m_siblings[i];
            }
        }
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::createSibling() %s is Full\n",this->m_cellID->toString().c_str());
    return 0;
}

CellIDPtr& CellTreeNode::getCellID() {
    return m_cellID;
}

int CellTreeNode::getDepth() {
    return m_depth;
}

void CellTreeNode::setLocation(int depth, int spanNo) {
    m_depth = depth;
    m_spanNo = spanNo;
    for (int i = 0; i < m_treeSpan; i++) {
        if (m_siblings[i] != 0) {
            m_siblings[i]->setLocation(depth + 1, i);
        }
    }
}

/*int getSpan() {
    return m_span;
}*/

int CellTreeNode::getTreeSpan() {
    return m_treeSpan;
}

bool CellTreeNode::hasAvailableSlotsForPeer(Byte type) {
    list<P3PeerInfoPtr>* list = 0;
    int max = 0;
    switch (type) {
        case P3PeerInfo::SUPERPEER:
        case P3PeerInfo::SUPERPEER_COORD:
        {
            list = &m_sps;
            max = m_maxSPs;
            break;
        }
        case P3PeerInfo::SENSOR:
        {
            list = &m_sensors;
            max = m_maxSensors;
            break;
        }
        case P3PeerInfo::MONITOR:
        {
            list = &m_monitors;
            max = m_maxMonitor;
            break;
        }
        default:
            return -1;
    }

    if (list->size() >= max) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)hasAvailableSlotsForPeer::add() - type(%d) is full! size(%lu)\n"), type, list->size()));
        return false;
    }
    return true;
}

int CellTreeNode::remove(UUIDPtr& uuid) {
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO CellTreeNode::remove(%s)\n"),uuid->toString().c_str()));
    list<P3PeerInfoPtr>* l = 0;
    l = &m_sps;
    list<P3PeerInfoPtr>::iterator iter = l->begin();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)remove\n")));
    while (iter != l->end()) {
        P3PeerInfo* temp = (*iter).get();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)remove inside %s %s\n"),temp->getPID()->toString().c_str(),uuid->toString().c_str()));
        if (temp->getPID()->isEqual(uuid)) {
            l->erase(iter);
            return 0;
        }
        iter++;
    }
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO CellTreeNode::remove(%s) checked sps, now sensors\n"),uuid->toString().c_str()));
    iter = m_sensors.begin();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)remove\n")));
    while (iter != m_sensors.end()) {
        P3PeerInfo* temp = (*iter).get();
        if (m_debugCellTreeNode) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)remove inside Leaf %s %s\n"), temp->getPID()->toString().c_str(), uuid->toString().c_str()));
        }
        if (temp->getPID()->isEqual(uuid)) {
            m_sensors.erase(iter);
            return 0;
        }
        iter++;
    }
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO CellTreeNode::remove(%s) checked sensors, now monitors\n"),uuid->toString().c_str()));
    iter = m_monitors.begin();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)remove\n")));
    while (iter != m_monitors.end()) {
        P3PeerInfo* temp = (*iter).get();
        if (m_debugCellTreeNode) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)remove inside Monitor %s %s\n"), temp->getPID()->toString().c_str(), uuid->toString().c_str()));
        }
        if (temp->getPID()->isEqual(uuid)) {
            m_monitors.erase(iter);
            return 0;
        }
        iter++;
    }

    return -1;
}

int CellTreeNode::add(P3PeerInfo* peer) {
    list<P3PeerInfoPtr>* l = 0;
    int max = 0;
    switch (peer->getType()) {
        case P3PeerInfo::SUPERPEER:
        case P3PeerInfo::SUPERPEER_COORD:
        {
            l = &m_sps;
            max = m_maxSPs;
            break;
        }
        case P3PeerInfo::SENSOR:
        {
            l = &m_sensors;
            max = m_maxSensors;
            break;
        }
        case P3PeerInfo::MONITOR:
        {
            l = &m_monitors;
            max = m_maxMonitor;
            break;
        }
        default:
            return -1;
    }
    list<P3PeerInfoPtr>::iterator iter = l->begin();
    while (iter != l->end()) {
        P3PeerInfo* temp = (*iter).get();
        if (m_debugCellTreeNode) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellTreeNode::add() - id(%s) id2(%s)\n"),
                    temp->getPID()->toString().c_str(), peer->getPID()->toString().c_str()));
        }
        if (temp->getPID()->isEqual(peer->getPID())) {
            if (m_debugCellTreeNode) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellTreeNode::add() - id(%s) type(%d) already present\n"),
                        peer->getPID()->toString().c_str(), peer->getType()));
            }
            return -1;
        }
        iter++;
    }

    if (l->size() >= max) {
        if (m_debugCellTreeNode) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellTreeNode::add() - type(%d) is full! size(%lu)\n"), peer->getType(), l->size()));
        }
        return -1;
    }

    P3PeerInfoPtr peerPtr(peer);
    l->push_back(peerPtr);
    if (m_debugCellTreeNode) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellTreeNode::add() - id(%s) type(%d) OK\n"), peer->getPID()->toString().c_str(), peer->getType()));
    }
    return 0;
}

String& CellTreeNode::toString_() {
    m_str = "\0";
    for (int i = 0; i < m_depth; i++) {
        m_str += "+";
    }
    m_str += "(" + Integer::toUnsignedString(m_depth, 10) + "," +
            Integer::toUnsignedString(m_spanNo, 10) + "," +
            m_cellID->toString() + ")";
    for (int i = 0; i < getTreeSpan(); i++) {
        if (getSibling(i) != 0) {
            m_str += "\n" + getSibling(i)->toString_();
        }
    }
    return m_str;
}

String& CellTreeNode::toString() {
    //m_str = "";
    stringstream ss;
    for (int i = 0; i < m_depth; i++) {
        //m_str += "+";
        ss << "-";
    }
    /*m_str += "(" + Integer::toUnsignedString(m_depth, 10) + "," +
            getSpanString() + "," +
            m_cellID->toString() + ")\n";*/
    ss << "(" + Integer::toUnsignedString(m_depth, 10) << "," <<
            getSpanString() << "," <<
            m_cellID->toString() << ")\n";
    //m_str += "Superpeers:\n";
    ss << "Superpeers:\n";
    list<P3PeerInfoPtr>::iterator iter = m_sps.begin();
    while (iter != m_sps.end()) {
        for (int i = 0; i < m_depth; i++) {
            //m_str += " ";
            ss << " ";
        }
        //m_str += (*iter)->toString() + "\n";
        ss << (*iter)->toString() << "\n";
        iter++;
    }
    //m_str += "Leafs:\n";
    ss << "Leafs:\n";
    iter = m_sensors.begin();
    while (iter != m_sensors.end()) {
        for (int i = 0; i < m_depth; i++) {
            //m_str += " ";
            ss << " ";
        }
        //m_str += (*iter)->toString() + "\n";
        ss << (*iter)->toString() << "\n";
        iter++;
    }
    //m_str += "Monitors:\n";
    ss << "Monitors:\n";
    iter = m_monitors.begin();
    while (iter != m_monitors.end()) {
        for (int i = 0; i < m_depth; i++) {
            //m_str += " ";
            ss << " ";
        }
        //m_str += (*iter)->toString() + "\n";
        ss << (*iter)->toString() << "\n";
        iter++;
    }
    m_str = ss.str().c_str();
    return m_str;
}

String& CellTreeNode::getSpanString() {
    m_spanStr = "\0";
    if (getParent() != 0) {
        m_spanStr += getParent()->getSpanString();
    }
    m_spanStr += Integer::toUnsignedString(m_spanNo, 10);
    return m_spanStr;
}

int CellTreeNode::getPeerCount() {
    return m_sps.size();
}

void CellTreeNode::getPeers(list<PeerLocation>& l) {
    list<P3PeerInfoPtr>::iterator iter = m_sps.begin();
    while (iter != m_sps.end()) {
        P3PeerInfo* info = (*iter).get();
        PeerLocation loc(info->getPID(), info->getFID());
        l.push_back(loc);
        iter++;
    }

    iter = m_sensors.begin();
    while (iter != m_sensors.end()) {
        P3PeerInfo* info = (*iter).get();
        PeerLocation loc(info->getPID(), info->getFID());
        l.push_back(loc);
        iter++;
    }

    iter = m_monitors.begin();
    while (iter != m_monitors.end()) {
        P3PeerInfo* info = (*iter).get();
        PeerLocation loc(info->getPID(), info->getFID());
        l.push_back(loc);
        iter++;
    }
}

void CellTreeNode::getPeersByType(list<P3PeerInfoPtr>& l, int type) {
    switch (type) {
        case P3PeerInfo::SUPERPEER:
        case P3PeerInfo::SUPERPEER_COORD:
        {
            list<P3PeerInfoPtr>::iterator iter = m_sps.begin();
            while (iter != m_sps.end()) {

                l.push_back(*iter);
                iter++;
            }
            return;
        }
        case P3PeerInfo::SENSOR:
        {
            list<P3PeerInfoPtr>::iterator iter = m_sensors.begin();
            while (iter != m_sensors.end()) {
                l.push_back(*iter);
                iter++;
            }
            return;
        }
        case P3PeerInfo::MONITOR:
        {
            list<P3PeerInfoPtr>::iterator iter = m_monitors.begin();
            while (iter != m_monitors.end()) {
                l.push_back(*iter);
                iter++;
            }
            return;
        }
    }
}

void CellTreeNode::deserialize(InputStream& inputStream) throw (SerializationException&) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::deserialize inputstream_size=%d\n",inputStream.length());
    CellID* cellID = new CellID(inputStream);
    m_cellID.reset(cellID);
    inputStream.read_short(m_port);
    inputStream.read_long(m_depth);
    inputStream.read_long(m_spanNo);
    inputStream.read_long(m_treeSpan);
    inputStream.read_long(m_maxSPs);
    inputStream.read_long(m_maxSensors);
    inputStream.read_long(m_maxMonitor);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::deserialize - %s %d %d %d %d\n",m_cellID->toString().c_str(),m_depth,m_spanNo,m_treeSpan,m_maxSPs);
    m_siblings = new CellTreeNode*[m_treeSpan];
    for (int i = 0; i < m_treeSpan; i++) {
        m_siblings[i] = 0;
    }
    int count = 0;
    inputStream.read_long(count);
    for (int i = 0; i < count; i++) {
        P3PeerInfo* pi = new P3PeerInfo(inputStream);
        P3PeerInfoPtr piPtr(pi);
        m_sps.push_back(piPtr);
    }
    inputStream.read_long(count);
    for (int i = 0; i < count; i++) {
        P3PeerInfo* pi = new P3PeerInfo(inputStream);
        P3PeerInfoPtr piPtr(pi);
        m_sensors.push_back(piPtr);
    }
    inputStream.read_long(count);
    for (int i = 0; i < count; i++) {
        P3PeerInfo* pi = new P3PeerInfo(inputStream);
        P3PeerInfoPtr piPtr(pi);
        m_monitors.push_back(piPtr);
    }
    int siblingCount = 0;
    inputStream.read_long(siblingCount);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::deserialize - siblingCount(%d)\n",siblingCount);
    for (int i = 0; i < siblingCount; i++) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::deserialize - sibling...\n");
        int order = 0;
        inputStream.read_long(order);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::deserialize - sibling(%d)\n",order);
        this->setSibbling(order, new CellTreeNode(this, inputStream));
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::deserialize end\n");

}

void CellTreeNode::serialize(OutputStream& outputStream) throw (SerializationException&) {
    m_cellID->serialize(outputStream);
    outputStream.write_short(m_port);
    outputStream.write_long(m_depth);
    outputStream.write_long(m_spanNo);
    outputStream.write_long(m_treeSpan);
    outputStream.write_long(m_maxSPs);
    outputStream.write_long(m_maxSensors);
    outputStream.write_long(m_maxMonitor);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::serialize - %s %d %d %d %d\n",m_cellID->toString().c_str(),m_depth,m_spanNo,m_treeSpan,m_maxSPs);
    int count = m_sps.size();
    outputStream.write_long(count);
    list<P3PeerInfoPtr>::iterator iter = m_sps.begin();
    while (iter != m_sps.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }
    count = m_sensors.size();
    outputStream.write_long(count);
    iter = m_sensors.begin();
    while (iter != m_sensors.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }
    count = m_monitors.size();
    outputStream.write_long(count);
    iter = m_monitors.begin();
    while (iter != m_monitors.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }
    int siblingCount = 0;
    for (int i = 0; i < getTreeSpan(); i++) {
        if (getSibling(i) != 0) {
            siblingCount++;
        }
    }
    outputStream.write_long(siblingCount);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellTreeNode::serialize - siblingCount(%d)\n",siblingCount);
    if (siblingCount > 0) {
        for (int i = 0; i < getTreeSpan(); i++) {
            if (getSibling(i) != 0) {
                outputStream.write_long(i);
                getSibling(i)->serialize(outputStream);
            }
        }
    }
}

int CellTreeNode::getPeer(const UUIDPtr& pid, P3PeerInfoPtr& info) {
    list<P3PeerInfoPtr>::iterator iter = m_sps.begin();
    while (iter != m_sps.end()) {
        if ((*iter)->getPID()->isEqual(pid)) {
            info = (*iter);
            return 0;
        }
        iter++;
    }
    iter = m_sensors.begin();
    while (iter != m_sensors.end()) {
        if ((*iter)->getPID()->isEqual(pid)) {
            info = (*iter);
            return 0;
        }
        iter++;
    }

    iter = m_monitors.begin();
    while (iter != m_monitors.end()) {
        if ((*iter)->getPID()->isEqual(pid)) {
            info = (*iter);
            return 0;
        }
        iter++;
    }
    return -1;
}

int CellTreeNode::getPeerByIID(const UUIDPtr& sid, const UUIDPtr& iid, SAPInfoPtr& info) {
    list<P3PeerInfoPtr>::iterator iter = m_sps.begin();
    while (iter != m_sps.end()) {
        if ((*iter)->isServiceInstanceRunning(sid, iid)) {
            //info = (*iter);
            ServiceInstanceInfoPtr si;
            (*iter)->getInstanceOfService(sid, iid, si);
            info = si->getSAPInfo();
            return 0;
        }
        iter++;
    }
    return -1;
}
