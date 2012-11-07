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
 * File:   AsyncDiscoveryQueryReply.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 12, 2010, 11:06 AM
 */

#include "AsyncDiscoveryQueryReply.h"

AsyncDiscoveryQueryReply::AsyncDiscoveryQueryReply(ACE_Time_Value* timeout):
    EventFuture<DiscoveryQueryReply*>(timeout) {
}

AsyncDiscoveryQueryReply::AsyncDiscoveryQueryReply(UInt expectedSize, 
        ACE_Time_Value* timeout):
        EventFuture<DiscoveryQueryReply*>(expectedSize,timeout) {
}


/*AsyncDiscoveryQueryReply::AsyncDiscoveryQueryReply(const AsyncDiscoveryQueryReply& orig) {
}*/

AsyncDiscoveryQueryReply::~AsyncDiscoveryQueryReply() {
}

