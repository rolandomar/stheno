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
 * File:   DiscoveryProviderInfo.h
 * Author: rmartins
 *
 * Created on January 11, 2011, 10:39 AM
 */

#ifndef DISCOVERYPROVIDERINFO_H
#define	DISCOVERYPROVIDERINFO_H

class DiscoveryProvider;

class DiscoveryProviderInfo{
public:
    DiscoveryProviderInfo(DiscoveryProvider* provider,int providerID);
    DiscoveryProviderInfo(const DiscoveryProviderInfo& x);    
    virtual ~DiscoveryProviderInfo();

    DiscoveryProvider* getProvider();    
    int getProviderID();    
    bool isEqual(DiscoveryProvider* provider,int providerID);
    
protected:
    DiscoveryProvider* m_provider;
    int m_providerID;
};
#endif	/* DISCOVERYPROVIDERINFO_H */

