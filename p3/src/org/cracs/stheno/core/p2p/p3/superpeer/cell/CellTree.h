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
 * File:   CellTree.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 15, 2010, 3:49 PM
 */

#ifndef CELLTREE_H
#define	CELLTREE_H
#include <list>
using std::list;
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <euryale/common/ListHelper.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/p3/common/P3PeerInfo.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellTreeNode.h>

class CellTree : public Serializable {
public:
    static const int MAX_DEPTH = 3;
    CellTree(CellIDPtr& rootCellID);
    CellTree(InputStream& inputStream);
    virtual ~CellTree();

    CellTreeNode* removeCell(CellIDPtr& cellID);

    CellTreeNode* getAvailableNode(Byte type);

    CellTreeNode* createCell();
    CellTreeNode* createCell(CellIDPtr& cellID);
    CellTreeNode* getParentAvailableLocation();

    void getCoordinators(list<P3PeerInfoPtr>& l);
    void getSensors(list<P3PeerInfoPtr>& l);
    void getPoL(P3PeerInfoPtr& l,list<UUIDPtr>& filter);   
    

    int addSubtree(CellTree* subTree,CellIDPtr& parentCellID);
    int addPeer(P3PeerInfo* peer);
    int addPeer(P3PeerInfo* peer,CellIDPtr& parentCellID);
    int getPeer(const UUIDPtr& pid,P3PeerInfoPtr& info);
    int getPeerByLevel(int treeLevel, P3PeerInfoPtr& info);
    int getRandomPeer(P3PeerInfoPtr& info);
    int getPeerByIID(const UUIDPtr& sid,const UUIDPtr& iid,SAPInfoPtr& info);
    

    int removeSubtree(const CellIDPtr& fid);
    int removeSubtree();

    void pruneSubtrees(list<UUIDPtr>& l);


    CellTreeNode* getCellTreeNode(const UUIDPtr& fid);
    CellTreeNode* getCellTreeNode(const CellIDPtr& fid);
    CellTreeNode* getCellTreeNode_i(const CellIDPtr& fid,CellTreeNode* node);

    CellTreeNode* getRootNode();

    CellIDPtr& getRootCellID();

    String& toString();

    String& toStringPerLevel();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    CellTreeNode* dettach();
    
protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    bool m_debugCellTree;
    CellTreeNode* m_rootNode;
    String m_str;
    String m_strPerLevel;

    
    int getPeer_i(CellTreeNode* startNode,const UUIDPtr& pid,P3PeerInfoPtr& info);
    int getPeerByLevel_i(CellTreeNode* startNode,int treeLevel,P3PeerInfoPtr& info);
    int getRandomPeer_i(CellTreeNode* startNode,list<P3PeerInfoPtr>& l);
    int getPeerByIID_i(CellTreeNode* node, const UUIDPtr& sid,const UUIDPtr& iid, SAPInfoPtr& info);
    
    list<CellTreeNode*>* getNodesPerDepth(int depth);
    
    void getSensors_i(CellTreeNode* startNode, list<P3PeerInfoPtr>& l);

    void getNodesPerDepth_i(list<CellTreeNode*>* l,
            CellTreeNode* node, int depth);


    void getCoordinators_i(CellTreeNode* startNode,list<P3PeerInfoPtr>& l);
    //depth-first leftmost node

    CellTreeNode* findBottomNode(CellTreeNode* parent, CellTreeNode* node);

    CellTreeNode* getParentTreeNode(CellIDPtr& cellID);

    CellTreeNode* getParentTreeNode(CellID* cellID);

    CellTreeNode* getTreeNode(CellIDPtr& cellID);

    CellTreeNode* getTreeNode(CellID* cellID);

    CellTreeNode* getParentTreeNode_i(CellTreeNode* parent,
            CellTreeNode* node, CellID* cellID);

    CellTreeNode* getTreeNode_i(CellTreeNode* node, CellID* cellID);

    CellTreeNode* createCell_i(CellTreeNode* node);    

    CellTreeNode* getAvailableNodeForSP_i(CellTreeNode* node, Byte type);

    CellTreeNode* getAvailableNodeForSensor_i(CellTreeNode* node, Byte type);

    int removeSubtree_i(CellTreeNode* node);
private:
    bool isUUIDInList(const UUIDPtr& uuid,list<UUIDPtr>& filter);
    
};

#endif	/* CELLTREE_H */

