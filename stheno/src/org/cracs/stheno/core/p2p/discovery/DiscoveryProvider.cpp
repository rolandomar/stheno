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
 * File:   DiscoveryProvider.cpp
 * Author: rmartins
 * 
 * Created on July 26, 2010, 5:02 PM
 */

#include "DiscoveryProvider.h"

DiscoveryProvider::DiscoveryProvider() {
}

DiscoveryProvider::~DiscoveryProvider() {
}

bool DiscoveryProvider::isEventProvided(UInt event){
    list<UInt>& l = getProvidedEvents();
    list<UInt>::iterator iter = l.begin();
    while(iter != l.end()){
        if(*iter == event){
            return true;
        }
        iter++;
    }
    return false;    
}
