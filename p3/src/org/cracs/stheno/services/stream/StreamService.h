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
 * File:   StreamService.h
 * Author: rmartins
 *
 * Created on October 26, 2010, 11:29 AM
 */

#ifndef STREAMSERVICE_H
#define	STREAMSERVICE_H

#include <stheno/service/UserService.h>
#include <stheno/services/stream/ServiceStreamParams.h>
#include <stheno/core/p2p/OverlayInterface.h>
#include <stheno/services/stream/net/StreamServiceSAP.h>

#include <stheno/services/stream/net/StreamFrameListener.h>

class StreamClientHandler;
class StreamService : public UserService,public StreamFrameListener {
public:
    StreamService(Stheno* runtime,const char* id);
    StreamService(Stheno* runtime,const char* id,UUIDPtr& iid);    
    virtual ~StreamService();

    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params) = 0;
    virtual ServiceAbstract* duplicate() throw (ServiceException&) = 0;
    virtual const char* getName() = 0;
    //virtual list<EndpointPtr>& getEndpoints() throw (ServiceException&);
    virtual void getSAP(SAPInfoPtr& sap);
    virtual const char* id() = 0;
    
    //type(params,ServiceStreamParams)
    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&);
    
    virtual void close() {}

    //get a client to this service => local bind
    //virtual ServiceClient* getClient() = 0;
    //remote binds
    virtual ServiceClient* getClient(const UUIDPtr& sid,ClientParamsPtr& cp) {return 0;}
    virtual ServiceClient* getClient(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp) {return 0;}
    
    virtual void onFramePacket(FramePacket* packet) ;
    
    virtual bool changeToPrimaryRole();

    
    //Get frames from [frameNo,present_frame]
    int getRetrieveStorage(UInt frameNo,list<StreamFramePacket*>& l);
    
protected:
    ACE_Recursive_Thread_Mutex m_lock;
    ServiceStreamParams* m_streamParams;
    StreamClientHandler* m_sensorClient;
    OverlayInterface* m_overlayInterface;
    //StreamServiceAcceptor m_acceptor;
    StreamServiceSAP* m_sap;
    
    //Storage handling
    vector<FramePacket*> m_storage;
    UInt m_storageMaxLimit;
    UInt m_storageIndex;
    
    void addToStorage(FramePacket* packet);
    

};

#endif	/* STREAMSERVICE_H */

