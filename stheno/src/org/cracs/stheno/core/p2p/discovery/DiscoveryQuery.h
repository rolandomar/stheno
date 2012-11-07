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
 * File:   DiscoveryQuery.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 11, 2010, 5:53 PM
 */

#ifndef DISCOVERYQUERY_H
#define	DISCOVERYQUERY_H

#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>
#include <ace/Synch.h>
#include <ace/Bound_Ptr.h>

class DiscoveryQuery : public Serializable {
public:

    DiscoveryQuery(UInt provider, UInt type);
    DiscoveryQuery(const DiscoveryQuery& orig);
    DiscoveryQuery(DiscoveryQuery* query);
    DiscoveryQuery(InputStream& inputStream);
    virtual ~DiscoveryQuery();

    UInt getProvider();
    UInt getType();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

    ACE_Message_Block* getQuerySpecializationBlob();

    ACE_Message_Block* deattachQuerySpecializationBlob();

    void getQuerySpecializationBlob(ACE_Message_Block* querySpecializationBlob);

protected:
    UInt m_provider;
    UInt m_type;
    ACE_Message_Block* m_querySpecializationBlob;

    virtual void serializeMetadata(OutputStream& outputStream) throw (SerializationException&);

    virtual void deserializeMetadata(InputStream& inputStream) throw (SerializationException&);

    virtual UInt getSerializationSizeQuerySpecialization() throw (SerializationException&);
    
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);

};
typedef ACE_Strong_Bound_Ptr<DiscoveryQuery, ACE_Recursive_Thread_Mutex> DiscoveryQueryPtr;

#endif	/* DISCOVERYQUERY_H */

