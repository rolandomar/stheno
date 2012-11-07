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
 * File:   CellTreeNode.h
 * Author: rmartins
 *
 * Created on June 30, 2010, 3:58 PM
 */

#ifndef CELLTREENODE_H
#define	CELLTREENODE_H

#include <list>
using std::list;
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <euryale/common/ListHelper.h>
#include <euryale/net/endpoint/Endpoint.h>

#include <stheno/core/p2p/common/PeerInfo.h>
#include <euryale/common/uuid/PeerLocation.h>

class P3PeerInfo;
typedef ACE_Strong_Bound_Ptr<P3PeerInfo, ACE_SYNCH_MUTEX> P3PeerInfoPtr;

class CellTreeNode : public Serializable {
public:

    CellTreeNode(CellIDPtr& cellID, CellTreeNode* parent, int spanNo, int depth,
            int treeSpan, int maxSP, int maxSensors, int maxMonitor);

    CellTreeNode(CellTreeNode* parent,InputStream& inputStream);

    virtual ~CellTreeNode();

    void setSpanNo(int spanNo);

    int getSpanNo();

    void setParent(CellTreeNode* parent);

    CellTreeNode* getParent();
    
    void setDepth(int depth);

    void createSibling(int spanNo,CellIDPtr& cellIDPtr);

    bool sliblingExists(int spanNo);

    void removeSibling(int spanNo);

    CellTreeNode* getSibling(int spanNo);

    CellTreeNode* setSibbling(int spanNo, CellTreeNode* newChild);

    void replaceSibling(CellTreeNode* node, CellTreeNode* newNode);

    bool hasAllSiblings();

    int countSiblings();    

    CellTreeNode* createSibling();
    CellTreeNode* createSibling(CellIDPtr& cellIDPtr);

    CellIDPtr& getCellID();

    int getDepth();

    void setLocation(int depth,int spanNo);

    int getTreeSpan();

    bool hasAvailableSlotsForPeer(Byte type);

    void setPort(Short port);

    int getPeerCount();
    void getCoordinator(P3PeerInfoPtr& info);
    
    void getPeers(list<PeerLocation>& l);
    
    void getPeersByType(list<P3PeerInfoPtr>& l,int type);    
    int getPeer(const UUIDPtr& pid,P3PeerInfoPtr& info);
    int getPeerByIID(const UUIDPtr& sid,const UUIDPtr& iid,SAPInfoPtr& info);
    

    int add(P3PeerInfo* peer);
    int remove(UUIDPtr& uuid);
    String& toString_();

    String& toString();
    String& getSpanString();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

    String m_str;
    String m_spanStr;
protected:
    bool m_debugCellTreeNode;
    Short m_port;
    CellTreeNode* m_parent;
    CellIDPtr m_cellID;
    /*list<P3PeerInfo*> m_sps;
    list<P3PeerInfo*> m_sensors;
    list<P3PeerInfo*> m_monitors;*/
    list<P3PeerInfoPtr> m_sps;
    list<P3PeerInfoPtr> m_sensors;
    list<P3PeerInfoPtr> m_monitors;
    int m_depth;    
    int m_spanNo;    
    int m_treeSpan;
    int m_maxSPs;
    int m_maxSensors;
    int m_maxMonitor;
    CellTreeNode** m_siblings;
    
};

#endif	/* CELLTREENODE_H */

