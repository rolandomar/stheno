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
 * File:   CloseDatagramListener.h
 * Author: rmartins
 *
 * Created on September 28, 2010, 6:30 PM
 */

#ifndef CLOSEDATAGRAMLISTENER_H
#define	CLOSEDATAGRAMLISTENER_H

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
class AbstractDatagramChannel;

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
class CloseDatagramListener {
public:
    typedef AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE> CHANNEL_TYPE;
    virtual void onClose(CHANNEL_TYPE* channel) = 0;
};


#endif	/* CLOSEDATAGRAMLISTENER_H */

