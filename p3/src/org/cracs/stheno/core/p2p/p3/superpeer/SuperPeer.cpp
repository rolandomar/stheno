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
 * File:   SuperPeer.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 30, 2010, 12:04 PM
 */

#include "SuperPeer.h"
#include <euryale/common/Backtrace.h>
#include <stheno/core/p2p/p3/mesh/P3Mesh.h>
#include <stheno/core/p2p/p3/ft/P3FaultTolerance.h>
//#include <stheno/core/p2p/p3/discovery/P3Discovery.h>
#include <stheno/core/p2p/p3/superpeer/discovery/SuperPeerDiscovery.h>
#include <euryale/qos/cpu_qos/CPUGroupQoS.h>

SuperPeer::SuperPeer(SthenoCoreInterface* runtime) :
P3(runtime), m_meshService(0), m_discoveryService(0) {
}

SuperPeer::SuperPeer() :
P3(), m_meshService(0), m_discoveryService(0) {
}

SuperPeer::~SuperPeer() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)~SuperPeer\n")));
}

void SuperPeer::getDiscovery(DiscoveryPtr& discoveryPtr) throw (OverlayException&) {
    discoveryPtr = m_discoveryPtr;
}

void SuperPeer::getMesh(MeshPtr& meshPtr) throw (OverlayException&) {
    //return 0;
    meshPtr = m_meshPtr;
}

void SuperPeer::getFaultTolerance(FaultTolerancePtr& ftPtr) throw (OverlayException&) {
    ftPtr = m_ftPtr;
}

void SuperPeer::open() throw (OverlayException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Superpeer::open(%d)\n"), m_status));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toStarting();
    open_i();
    //toStart();
}

void SuperPeer::open_i() throw (OverlayException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Superpeer::open_i(%d)\n"), m_status));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (isStarting() /*|| isResuming()*/) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Superpeer::open_i starting(%d)\n"), m_status));
        
         UInt runtimeQoS = m_runtime->getRuntimeQoS();
         UInt runtimeQoSPeriod = m_runtime->getRuntimeQoSPeriod();
         
         //UInt p2pQoS = 500;//5000;         
         
         //UInt discoveryQoS = 50000;//;140000;         
         //UInt meshQoS = 50000;//140000;
         //UInt ftQoS = 50000;//50000;         
         
         //total = 36K
         //nt=150K
    
        m_meshService = new P3Mesh(this);
        m_discoveryService = new SuperPeerDiscovery(this);
        m_ftService = new P3FaultTolerance(this);
        //bootstrap
        try {
            //discovery boot
            ServiceParamsPtr nullParams;
            QoSManagerInterface* discoveryQoSManager = 0;
            /*if (getQoSManager() != 0) {
                discoveryQoSManager = getQoSManager()->createSubDomainQoSManager("discovery", discoveryQoS, runtimeQoSPeriod);
            }*/
            discoveryQoSManager = getQoSManager();
            m_discoveryService->open(nullParams, ServiceAbstract::FT_NOT_SETTED, discoveryQoSManager);
            m_discoveryPtr.reset(m_discoveryService);
            //ft boot
            QoSManagerInterface* ftQoSManager = 0;
            /*if (getQoSManager() != 0) {
                ftQoSManager = getQoSManager()->createSubDomainQoSManager("ft", ftQoS, runtimeQoSPeriod);
            }*/
            ftQoSManager = getQoSManager();
            m_ftService->open(nullParams, ServiceAbstract::FT_NOT_SETTED, ftQoSManager);
            m_ftPtr.reset(m_ftService);
            //mesh boot            
            QoSManagerInterface* meshQoSManager = 0;
            /*if (getQoSManager() != 0) {
                meshQoSManager = getQoSManager()->createSubDomainQoSManager("mesh", meshQoS, runtimeQoSPeriod);
            }*/
            meshQoSManager = getQoSManager();
            m_meshService->open(nullParams, ServiceAbstract::FT_NOT_SETTED, meshQoSManager);
            m_meshPtr.reset(m_meshService);


        } catch (ServiceException& ex) {
            Backtrace::backstrace();
            throw OverlayException(ex.message());
        }
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Superpeer::open(): error\n")));
        throw OverlayException(OverlayException::INVALID_OVERLAY);
    }
}

void SuperPeer::close_i() throw (OverlayException&) {
    //P3::close_i();
    printf("SuperPeer::close_i() - 1\n");
    if (m_meshService != 0) {
        m_meshService->close();
        m_meshService = 0;
    }
    printf("SuperPeer::close_i() - 2\n");
    if (m_discoveryService != 0) {
        m_discoveryService->close();
        m_discoveryService = 0;
    }
    printf("SuperPeer::close_i() - 3\n");
}

SuperPeerDiscovery* SuperPeer::getP3Discovery() {
    return m_discoveryService;
}

P3Mesh* SuperPeer::getP3MeshService() {
    return m_meshService;
}

void SuperPeer::getPID(UUIDPtr& uuid) {
    getUUID(uuid);
}




#ifdef LL

void *
SuperPeer::operator new (size_t bytes) {
    return ::new char[bytes];
}
#if defined (ACE_HAS_NEW_NOTHROW)

void *
SuperPeer::operator new (size_t bytes, const ACE_nothrow_t&) {
    return ::new (ACE_nothrow) char[bytes];
}
#if !defined (ACE_LACKS_PLACEMENT_OPERATOR_DELETE)

void
SuperPeer::operator delete (void *p, const ACE_nothrow_t&) throw () {
    delete [] static_cast<char *> (p);
}
#endif /* ACE_LACKS_PLACEMENT_OPERATOR_DELETE */
#endif

void
SuperPeer::operator delete (void *ptr) {
    delete [] static_cast<char *> (ptr);
}
#endif









