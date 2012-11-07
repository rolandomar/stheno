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
 * File:   DiscoveryQueryReply.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 12, 2010, 10:57 AM
 */

#ifndef DISCOVERYQUERYREPLY_H
#define	DISCOVERYQUERYREPLY_H

#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>
#include <stheno/core/p2p/discovery/exception/DiscoveryException.h>
#include <ace/Synch.h>
#include <ace/Bound_Ptr.h>

class DiscoveryQueryReply : public Serializable {
public:
    DiscoveryQueryReply(UInt provider, UInt type);
    DiscoveryQueryReply(InputStream& inputStream);
    //DiscoveryQueryReply(DiscoveryException* ex);
    DiscoveryQueryReply(UInt provider, UInt type, DiscoveryException* ex);
    DiscoveryQueryReply(DiscoveryQueryReply* query);
    DiscoveryQueryReply(const DiscoveryQueryReply& orig);
    virtual ~DiscoveryQueryReply();

    UInt getType();
    UInt getProvider();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

    ACE_Message_Block* getQuerySpecializationBlob();

    ACE_Message_Block* deattachQuerySpecializationBlob();

    void getQuerySpecializationBlob(ACE_Message_Block* querySpecializationBlob);

    DiscoveryException* deattachException();

    bool isException();

protected:
    UInt m_provider;
    UInt m_type;
    ACE_Message_Block* m_querySpecializationBlob;
    DiscoveryException* m_ex;

    virtual void serializeMetadata(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeMetadata(InputStream& inputStream) throw (SerializationException&);
    virtual UInt getSerializationSizeQuerySpecialization() throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
};
typedef ACE_Strong_Bound_Ptr<DiscoveryQueryReply, ACE_Recursive_Thread_Mutex> DiscoveryQueryReplyPtr;
#endif	/* DISCOVERYQUERYREPLY_H */

