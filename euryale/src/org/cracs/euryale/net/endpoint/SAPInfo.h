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
 * File:   SAP.h
 * Author: rmartins
 *
 * Created on August 18, 2010, 3:46 PM
 */

#ifndef SAPINFO_H
#define	SAPINFO_H

#include <ace/Hash_Map_Manager.h>
#include <ace/Time_Value.h>
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>

#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/endpoint/exception/EndpointException.h>

class SAPInfo : public Serializable{
public:
    SAPInfo();
    SAPInfo(InputStream& inputStream);
    SAPInfo(const SAPInfo& orig);
    virtual ~SAPInfo();

    virtual bool addEndpoint(Endpoint& endpoint);
    virtual bool addEndpointList(list<Endpoint>& endpointList);
    virtual bool addEndpointList(list<EndpointPtr>& endpointList);
    virtual bool removeEndpoint(Endpoint& endpoint);
    virtual void getEndpointList(list<Endpoint>& el);
    virtual void getEndpoint(const QoSEndpoint& qos,Endpoint& endpoint) throw(EndpointException&);
    virtual void getFirstEndpoint(Endpoint& endpoint) const throw(EndpointException&) ;
    //Serialization interface
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

protected:
    list<Endpoint> m_endpointList;
};


typedef ACE_Strong_Bound_Ptr<SAPInfo,ACE_SYNCH_MUTEX> SAPInfoPtr;
#endif	/* SAPINFO_H */

