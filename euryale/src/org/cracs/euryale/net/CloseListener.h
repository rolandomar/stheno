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
 * File:   CloseListener.h
 * Author: rmartins
 *
 * Created on September 28, 2010, 6:53 PM
 */

#ifndef CLOSELISTENER_H
#define	CLOSELISTENER_H

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
class AbstractStreamChannel;

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
class CloseListener {
public:    
    virtual void onClose(AbstractStreamChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* channel) = 0;
};

#endif	/* CLOSELISTENER_H */

