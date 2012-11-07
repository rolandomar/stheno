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
 * File:   Cell.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 30, 2010, 12:31 PM
 */

#ifndef CELL_H
#define	CELL_H

#include <euryale/threading/Task.h>
#include <euryale/common/sleep/SleepHelper.h>

#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellListener.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCellPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCellReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/exception/CellException.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/CellNotifyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinCellPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinCellReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinMeshPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinMeshReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/LeaveMeshPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/LeaveMeshReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/RebindMeshPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/RebindMeshReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/UpdateInfoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/UpdateInfoReplyPacket.h>

#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCoordinatorPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCoordinatorReply.h>



#include <stheno/core/p2p/p3/superpeer/cell/CellReply.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellTree.h>
#include <euryale/net/StreamChannelGroup.h>
#include "net/CellAcceptor.h"
#include <euryale/net/StreamChannelGroup.h>
#include <stheno/core/p2p/p3/mesh/InfoUpdate.h>

#include <stheno/core/p2p/discovery/general_provider/SensorInfo.h>

class CellDiscovery;
class CellSvcHandler;
class CellFollowerSvcHandler;
class CellAcceptor;
class CellFollowerAcceptor;
class CellClientHandler;
typedef ACE_Strong_Bound_Ptr<CellClientHandler, ACE_Recursive_Thread_Mutex> CellClientHandlerPtr;
class ParentCellClientHandler;
typedef ACE_Strong_Bound_Ptr<ParentCellClientHandler, ACE_Recursive_Thread_Mutex> ParentCellClientHandlerPtr;

class CellGroup;
class NotifyThread;

class P3Mesh;
class QoSManagerInterface;

class Cell : /*public Task,*/ public ChannelManagerListener<ACE_SOCK_Stream, ACE_MT_SYNCH>,
public CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    friend class RebindThread;
    friend class NotifyThread;
    friend class ParentCellClientHandler;
    Cell(P3Mesh* listener, QoSManagerInterface* qosManager);
    virtual ~Cell();

    static int getTreeSpan(int depth);

    void getCellID(CellIDPtr& cellID);
    void getFID(UUIDPtr& fid);

    void getParentUUID(UUIDPtr& parentUUID);
    void getParentCellID(CellIDPtr& parentCellID);
    void getPID(UUIDPtr& pid);
    int getPeer(UUIDPtr& uuid, P3PeerInfoPtr& info);
    int getPeerByLevel(int treeLevel, P3PeerInfoPtr& info);
    int getRandomPeer(P3PeerInfoPtr& info);

    void open(CellReply* info, CellDiscovery* discovery = 0);

    void close() throw (CellException&);

    //CellReply* bind() throw (ServiceException&);
    //
    void handleDiscoveryPacket(CellDiscovery* discovery, SthenoPacket* packet);
    //
    int leaveMesh(UUIDPtr& uuid, CellIDPtr& cellID);
    int handleLeaveMesh(CellSvcHandler* channel, LeaveMeshPacket* packet);
    int handleJoinMesh(CellSvcHandler* channel, JoinMeshPacket* packet);
    int handleRebindMesh(CellSvcHandler* channel, RebindMeshPacket* packet);
    int handleJoinCell(CellSvcHandler* channel, JoinCellPacket* packet);
    int handleUpdateInfo(CellSvcHandler* channel, UpdateInfoPacket* packet);
    //
    int svc_i(void*);

    void joinCoordinator(UUIDPtr& parentUUID,
            CellAddrPtr& parentCellAddr,
            EndpointPtr& parentCoordinator,
            SAPInfoPtr& parentDiscoverySAP,
            SAPInfoPtr& parentMeshSAP,
            SAPInfoPtr& parentFTSAP
            ) throw (ServiceException&);

    void joinNonCoordinator(UUIDPtr& coordinatorUUID,
            EndpointPtr& coordinator) throw (ServiceException&);

    bool connecToParentCoordinator(UUIDPtr& parentUUID,
            CellAddrPtr& parentCellAddr,
            EndpointPtr& parentCoordinator,
            SAPInfoPtr& parentDiscoverySAP,
            SAPInfoPtr& meshFTSAP,
            SAPInfoPtr& parentFTSAP
            ) throw (ServiceException&);

    void createSAP() throw (ServiceException&);

    void getSAP(EndpointPtr& e);
    void getFollowerSAP(EndpointPtr& e);

    //void joinMesh(CellIDPtr& parentCellID,EndpointPtr& coordinator, ACE_Message_Block*& mb);

    bool IsCoordinator();
    void attach(CellDiscovery* discovery) throw (ServiceException&);

    virtual void onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
    virtual void onAdd(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);


    /*follower intercace*/
    int handleCoordinatorRebindMesh(/*CellClientHandler*/CellFollowerSvcHandler* channel, RebindMeshPacket* packet);
    int handleCoordinatorJoinMesh(/*CellClientHandler*/CellFollowerSvcHandler* channel, JoinMeshPacket* packet);
    int handleCoordinatorLeaveMesh(/*CellClientHandler*/CellFollowerSvcHandler* channel, LeaveMeshPacket* packet);
    int handleCoordinatorUpdateInfo(/*CellClientHandler*/CellFollowerSvcHandler* channel, UpdateInfoPacket* packet);


    //int handleCoordinatorBind(CellFollowerSvcHandler* channel);

    int processUpdateInfo(InfoUpdatePtr& info);

    bool isClosed();
    bool isRebinding();
    bool isRoot();
    bool isStarted();

    virtual void getNeighbors(list<PeerLocation>& l);

    virtual void getDiscoverySAPInfo(UUIDPtr& uuid, SAPInfoPtr& si) throw (ServiceException&);
    virtual void getMeshSAPInfo(UUIDPtr& uuid, SAPInfoPtr& si) throw (ServiceException&);
    virtual void getFTSAPInfo(UUIDPtr& uuid, SAPInfoPtr& si) throw (ServiceException&);

    void updateState(InfoUpdate* update) throw (ServiceException&);

    void getSensors(list<P3PeerInfoPtr>& l);

    void getPoL(P3PeerInfoPtr& peer, list<UUIDPtr>& filter);

    int getPeerByIID(const UUIDPtr& sid, const UUIDPtr& iid, SAPInfoPtr& info);

    int getPeer(const UUIDPtr& pid, P3PeerInfoPtr& info);

    void serializeState(OutputStream& outputStream) throw (SerializationException&);


    void getAudioSensors(list<SensorInfoPtr>& audioL);
    void getVideoSensors(list<SensorInfoPtr>& videoL);
    void getEventSensors(list<SensorInfoPtr>& eventL);
    void getActuatorSensors(list<SensorInfoPtr>& actuatorL);

    QoSManagerInterface* getQoSManager();

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    bool m_debugCell;
    Short m_startAllocationPort;
    bool m_started;
    bool m_rebinding;
    bool m_closed;
    UUIDPtr m_parentUUID;
    CellIDPtr m_cellID;
    UUIDPtr m_fid;
    CellIDPtr m_parentCellID;
    CellDiscovery* m_discovery;
    //used in recovery mode, when we assume control of the root node,
    //we maintain the original discovery for eficiency
    CellDiscovery* m_rootDiscovery;

    P3Mesh* m_listener;
    QoSManagerInterface* m_qosManager;
    bool m_coordinator;
    //Coordinator
    CellAcceptor* m_acceptor;
    //Follower
    CellFollowerAcceptor* m_followerAcceptor;
    CellTree* m_cellTree;
    //parent INFO - should be more isolated or grouped
    //ParentCellClientHandler* m_parentChannel;
    ParentCellClientHandlerPtr m_parentChannel;


    SAPInfoPtr m_parentDiscoverySAP;
    SAPInfoPtr m_parentMeshSAP;
    SAPInfoPtr m_parentFTSAP;
    //Coordinator context
    CellGroup* m_cellGroup;
    CellGroup* m_cellFollowerGroup;
    CellGroup* m_childrenGroup;
    CellGroup* m_leafGroup;
    NotifyThread* m_notify;
    //Follower
    //CellClientHandler* m_client;
    CellClientHandlerPtr m_client;
    //sensors
    list<SensorInfoPtr> m_audioSensors;
    list<SensorInfoPtr> m_videoSensors;
    list<SensorInfoPtr> m_eventSensors;
    list<SensorInfoPtr> m_actuatorSensors;

    //TODO
    CellFollowerSvcHandler* m_coordinatorChannel;

    ACE_SYNCH_RECURSIVE_MUTEX& getLock();

    CellReply* requestCell() throw (ServiceException&);

    void handleDiscoveryPacket_i(CellDiscovery* discovery, SthenoPacket* packet);
    void handleCellRequest(CellDiscovery* discovery, RequestCellPacket* packet);
    void handleCoordinatorRequest(CellDiscovery* discovery, RequestCoordinatorPacket* packet);
    //the most dreadfull event on a cell
    void handleNetworkPartition(CellNotifyPacket* packet);

    void sendNotify();

    void fireRebind();
    bool rebindToParentCoordinator(UUIDPtr& parentUUID,
            CellAddrPtr& parentCellAddr,
            EndpointPtr& parentCoordinator) throw (ServiceException&);

    bool rebindToNewCoordinator(UUIDPtr& uuid,
            EndpointPtr& coordinatorEndpoint);

    bool rebind(UUIDPtr& parentUUID,
            CellAddrPtr& parentCellAddr,
            EndpointPtr& parentCoordinator);

    void handleCoordinatorFailure(UUIDPtr& uuid);

    void getParentHandler(ParentCellClientHandlerPtr& parentHandler);

    //TODO: can this be public?
    // NO!

    CellTree* getCellTree() {
        return m_cellTree;
    }


};
#endif	/* CELL_H */

