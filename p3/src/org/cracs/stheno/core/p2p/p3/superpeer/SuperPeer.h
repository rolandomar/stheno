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
 * File:   SuperPeer.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 30, 2010, 12:04 PM
 */

#ifndef SUPERPEER_H
#define	SUPERPEER_H

#include <stheno/core/p2p/p3/P3.h>

#include <euryale/common/sleep/SleepHelper.h>


class SuperPeer : public P3{
public:
    SuperPeer();
    SuperPeer(SthenoCoreInterface* runtime);
    virtual ~SuperPeer();
    
    virtual void getDiscovery(DiscoveryPtr& discoveryPtr) throw(OverlayException&);
    virtual void getMesh(MeshPtr& ) throw(OverlayException&);
    virtual void getFaultTolerance(FaultTolerancePtr&) throw(OverlayException&);    
   
    virtual void getPID(UUIDPtr& uuid);
    
    void open() throw (OverlayException&);

    virtual SuperPeerDiscovery* getP3Discovery();
    virtual P3Mesh* getP3MeshService();
    
    
protected:    
    MeshPtr m_meshPtr;    
    P3Mesh* m_meshService;
    SuperPeerDiscovery* m_discoveryService;
    P3FaultTolerance* m_ftService;
    
    DiscoveryPtr m_discoveryPtr;
    FaultTolerancePtr m_ftPtr;

    
    virtual void close_i() throw (OverlayException&);
    virtual void open_i() throw (OverlayException&);    
};
#endif	/* SUPERPEER_H */

#ifdef LL
void *operator new (size_t bytes);
#if defined (ACE_HAS_NEW_NOTHROW)
// Overloaded new operator, nothrow_t variant.
void *operator new (size_t bytes, const ACE_nothrow_t&);
#if !defined (ACE_LACKS_PLACEMENT_OPERATOR_DELETE)
void operator delete (void *p, const ACE_nothrow_t&) throw ();
#endif /* ACE_LACKS_PLACEMENT_OPERATOR_DELETE */
#endif
void operator delete (void *ptr);
#endif