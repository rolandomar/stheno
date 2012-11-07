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
 * File:   AsyncDiscoveryQueryReply.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 12, 2010, 11:06 AM
 */

#ifndef ASYNCDISCOVERYQUERYREPLY_H
#define	ASYNCDISCOVERYQUERYREPLY_H

#include <euryale/event/EventFuture.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>

class AsyncDiscoveryQueryReply: public EventFuture<DiscoveryQueryReply*>{
public:
    AsyncDiscoveryQueryReply(ACE_Time_Value* timeout);
    AsyncDiscoveryQueryReply(UInt expectedSize, ACE_Time_Value* timeout);
    
    //AsyncDiscoveryQueryReply(const AsyncDiscoveryQueryReply& orig);
    virtual ~AsyncDiscoveryQueryReply();
protected:
    

};

#endif	/* ASYNCDISCOVERYQUERYREPLY_H */

