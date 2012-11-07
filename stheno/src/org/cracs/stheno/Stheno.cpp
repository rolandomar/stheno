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
 * File:   Stheno.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 29, 2010, 2:54 PM
 */

#include "Stheno.h"
#include <euryale/common/Backtrace.h>
#include <stheno/core/p2p/Overlay.h>
#include <stheno/service/UserService.h>
#include <stheno/service/ClientParams.h>

#include <stheno/core/SthenoCore.h>
#include <stheno/core/SthenoCoreInterface.h>
#include <stheno/core/p2p/OverlayInterface.h>
//#include <stheno/services/audio/AudioClient.h>

//#include <stheno/core/p2p/p3/superpeer/SuperPeer.h>

//hardcoded must be changed

Stheno* Stheno::createRuntime(UUIDPtr& rid) {
    Stheno* runtime = new Stheno(rid);
    return runtime;
    //return new SthenoPtr(runtime);
}

Stheno* Stheno::createRuntime() {
    Stheno* runtime = new Stheno();
    return runtime;
    //return new SthenoPtr(runtime);
}

Stheno* Stheno::duplicate() {
    Stheno* s = new Stheno(m_runtime);
    return s;
}

Stheno::Stheno() :
m_overlayInterface(0) {
    SthenoCore::createRuntime(m_runtime);
    //m_overlayInterface = m_runtime->getOverlayInterface();
    //SthenoCorePtr* runtimePtr = SthenoCore::createRuntime();
    //m_runtime = *runtimePtr;
    //delete runtimePtr;
    //:m_rid(UUID::generateUUID()){
    //m_runtime = new SthenoCore();
}

Stheno::Stheno(UUIDPtr& rid) :
m_overlayInterface(0) {
    //SthenoCorePtr* runtimePtr = SthenoCore::createRuntime(rid);
    SthenoCore::createRuntime(rid, m_runtime);
    //m_overlayInterface = m_runtime->getOverlayInterface();

    //m_runtime = *runtimePtr;
    //delete runtimePtr;
    //:m_rid(rid){    
}

Stheno::Stheno(const SthenoCorePtr& runtime) :
m_overlayInterface(0), m_runtime(runtime) {
    //m_overlayInterface = m_runtime->getOverlayInterface();

}

Stheno::~Stheno() {
    printf("Stheno::~Stheno\n");
    //Backtrace::backstrace();
    if (m_overlayInterface != 0) {
        printf("Stheno::~Stheno OI=%p\n",m_overlayInterface);
        delete m_overlayInterface;
        m_overlayInterface = 0;
        
    }
}

SthenoCorePtr& Stheno::getRuntime() throw (RuntimeException&) {
    return m_runtime;
}

void Stheno::attachOverlay(Overlay* overlay) throw (RuntimeException&) {
    //overlay->setRuntime(getRuntime().get());        
    overlay->setRuntime(new SthenoCoreInterface(getRuntime().get()));
    getRuntime()->attachOverlay(overlay);
}

ServiceFactory& Stheno::getServiceFactory() {
    //ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    return getRuntime()->getServiceFactory();
}

void Stheno::open(ACE_ARGV& args) throw (RuntimeException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    getRuntime()->open(args);

}

void Stheno::close() throw (RuntimeException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    getRuntime()->close();

}

void Stheno::startService(const UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid,UUIDPtr& deployUUIDD) throw (ServiceException&) {
    getRuntime()->startService(sid, params, iid,deployUUIDD);
    return;
}

void Stheno::startService(UUIDPtr& sid, UUIDPtr& uuid, ServiceParamsPtr& params, UUIDPtr& iid) throw (ServiceException&) {
    getRuntime()->startService(sid, uuid, params, iid);
    return;
}

void Stheno::stopService(UUIDPtr& sid, UUIDPtr& iid) {
    getRuntime()->stopService(sid, iid);
    return;
}

UserServicePtr* Stheno::getService(UUIDPtr& sid) {
    return 0;
}

//must delete

void Stheno::getUUID(UUIDPtr& uuid) throw (RuntimeException&) {
    getRuntime()->getUUID(uuid);
}

list<UUIDPtr>* Stheno::getListOfRuntimes(UUIDPtr& sid) throw (ServiceException&) {
    return getRuntime()->getInstancesOfServiceUUIDs(sid);
}

void Stheno::addService(UserService* service) {
}

void Stheno::onServiceClose(UserService* service) {

}

ServiceClient* Stheno::getClient(const UUIDPtr& sid,ClientParamsPtr& clientParams) {
    return getRuntime()->getClient(sid,clientParams);
}

ServiceClient* Stheno::getClient(const UUIDPtr& sid, const UUIDPtr& iid,ClientParamsPtr& clientParams) throw (RuntimeException&, ServiceException&){
    return getRuntime()->getClient(sid, iid,clientParams);
}

OverlayInterface* Stheno::getOverlayInterface() {
    if (m_overlayInterface == 0) {
        m_overlayInterface = m_runtime->getOverlayInterface();
    }
    return m_overlayInterface;
}