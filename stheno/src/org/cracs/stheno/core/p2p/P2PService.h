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
 * File:   P2PService.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 29, 2010, 12:03 PM
 */

#ifndef P2PSERVICE_H
#define	P2PSERVICE_H

#include <ace/Bound_Ptr.h>
#include <ace/Synch_Traits.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <euryale/Types.h>
#include <stheno/service/ServiceAbstract.h>

class Overlay;
class P2PService: public ServiceAbstract {
public:
    
    P2PService(const char* id,Overlay* overlay);
    virtual ~P2PService();

    virtual void close_i() throw(ServiceException&);    
    //not applicable to P2P services
    virtual ServiceAbstract* duplicate() throw(ServiceException&);

    Overlay* getOverlay();

protected:    
    Overlay* m_overlay;    
};

typedef ACE_Strong_Bound_Ptr<P2PService, ACE_Recursive_Thread_Mutex> P2PServicePtr;


#endif	/* P2PSERVICE_H */

