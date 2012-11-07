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
 * File:   Runtime.cpp
 * Author: rmartins
 * 
 * Created on October 27, 2009, 3:31 PM
 */

#include "RuntimeConfiguration.h"

RuntimeConfiguration::RuntimeConfiguration() {
    m_hrtInterval = new Interval(RuntimeConfiguration::HRTMinPriority, RuntimeConfiguration::HRTMaxPriority);
    m_srtInterval = new Interval(RuntimeConfiguration::SRTMinPriority, RuntimeConfiguration::SRTMaxPriority);
    m_beInterval = new Interval(RuntimeConfiguration::BEPriority, RuntimeConfiguration::BEPriority);
}

RuntimeConfiguration::~RuntimeConfiguration() {
    delete m_hrtInterval;
    delete m_srtInterval;
    delete m_beInterval;
}

bool RuntimeConfiguration::isClassDebugged(const char* className) {    
    //return true;
    String classNameStr(className);        
    /*if(classNameStr == "LeafClientHandler"){
        return true;
    }
    
    if(classNameStr == "FTDataClientHandler"){
        return true;
    }
    
    if(classNameStr == "FTControlClientHandler"){
        return true;
    }
    if(classNameStr == "FTControlSvcHandler"){
        return true;
    }
    if(classNameStr == "FTDataSvcHandler"){
        return true;
    }
    if(classNameStr == "P3ReplicationGroup"){
        return true;
    }
    if(classNameStr == "RequestGroup"){
        return true;
    }*/
    /*if(classNameStr == "EventFuture"){
        return true;
    }*/
    
    
    if(classNameStr == "Cell"){
        return true;
    }
    
    //if(classNameStr == "CellDiscovery"){
    //    return true;
    //}
    /*if(classNameStr == "Synchronizable"){
        return true;
    }*/
    
    
    /*
    if(classNameStr == "RequestGroup"){
        return true;
    }*/
    /*if(classNameStr == "Cell"){
        return true;
    }
    
    
    if(classNameStr == "Task"){
        return true;
    }
     if(classNameStr == "CellSvcHandler"){
        return true;
    }     
    
    
    if(classNameStr == "SuperPeerDiscovery"){
        return true;
    }*/
    
    /*if(classNameStr == "FTDataSvcHandler"){
        return true;
    }
    
    if(classNameStr == "FTDataClientHandler"){
        return true;
    }*/
    /*if(classNameStr == "ChannelManager"){
        return true;
    }
    
    if(classNameStr == "CellDiscoveryChannel"){
        return true;
    }*/
    return false;
}
