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
 * File:   ActuatorService.h
 * Author: rmartins
 *
 * Created on October 26, 2010, 11:29 AM
 */

#ifndef ActuatorSERVICE_H
#define	ActuatorSERVICE_H

#include <stheno/service/UserService.h>
#include <stheno/services/actuator/ServiceActuatorParams.h>
#include <stheno/core/p2p/OverlayInterface.h>
#include <stheno/services/actuator/net/ActuatorServiceSAP.h>
#include <stheno/services/actuator/net/ActuatorServiceChannelManager.h>

//#include <stheno/services/actuator/net/ActuatorFrameListener.h>

class ActuatorClientHandler;
class ActuatorService : public UserService{
public:
    ActuatorService();
    ActuatorService(Stheno* runtime);
    ActuatorService(Stheno* runtime,UUIDPtr& iid);    
    ActuatorService(Stheno* runtime,const char* id);
    ActuatorService(Stheno* runtime,const char* id,UUIDPtr& iid);    
    virtual ~ActuatorService();

    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params) {return 0;}
    virtual ServiceAbstract* duplicate() throw (ServiceException&);
    virtual const char* getName();
    virtual const char* id();
    //virtual list<EndpointPtr>& getEndpoints() throw (ServiceException&);
    virtual void getSAP(SAPInfoPtr& sap);
    
    
    //type(params,ServiceActuatorParams)
    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&);
    
    virtual void close() {}

    //get a client to this service => local bind
    //virtual ServiceClient* getClient() = 0;
    //remote binds
    virtual ServiceClient* getClient(const UUIDPtr& sid,ClientParamsPtr& cp) {return 0;}
    virtual ServiceClient* getClient(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp);
    
    virtual bool changeToPrimaryRole();
    
    bool handleActionPacket(ActionPacket* ap);

       
    void sendAction(ActionPacket* ap);
    
protected:
    ServiceActuatorParams* m_actuatorParams;
    //ActuatorClientHandler* m_sensorClient;
    ActuatorServiceChannelManager m_sensorClients;
    OverlayInterface* m_overlayInterface;
    //ActuatorServiceAcceptor m_acceptor;
    ActuatorServiceSAP* m_sap;        

};

#endif	/* ActuatorSERVICE_H */

