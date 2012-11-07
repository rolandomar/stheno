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
 * File:   StreamServiceChannelManager.cpp
 * Author: rmartins
 * 
 * Created on November 24, 2010, 2:55 PM
 */

#include "StreamServiceChannelManager.h"

StreamServiceChannelManager::StreamServiceChannelManager(ListenerType* listener):
ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>(listener)
{
}


StreamServiceChannelManager::~StreamServiceChannelManager() {
}

