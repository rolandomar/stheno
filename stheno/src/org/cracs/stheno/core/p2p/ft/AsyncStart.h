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
 * File:   AsyncStart.h
 * Author: rmartins
 *
 * Created on August 5, 2010, 10:45 AM
 */

#ifndef ASYNCSTART_H
#define	ASYNCSTART_H

#include <euryale/event/EventFuture.h>
#include <stheno/core/p2p/ft/StartResult.h>

class AsyncStart: public EventFuture<StartResult*>{
public:
    
    //AsyncStart(ACE_Time_Value* timeout);
    AsyncStart(UInt expectedSize, ACE_Time_Value* timeout);
    virtual ~AsyncStart();    
};

#endif	/* ASYNCSTART_H */

