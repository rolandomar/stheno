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
 * File:   ServiceParams.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 25, 2010, 3:15 PM
 */

#ifndef SERVICEPARAMS_H
#define	SERVICEPARAMS_H

#include <ace/Synch.h>
#include <ace/Refcounted_Auto_Ptr.h>

#include <euryale/serialization/Serializable.h>
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>

#include <stheno/service/FTServiceParams.h>

/*
 * Params to be passed for service creation 
 */

class ServiceParams : public Serializable {
public:
    ServiceParams(const UUIDPtr& sid);
    ServiceParams(const UUIDPtr& sid, FTServiceParamsPtr& ftParams);
    ServiceParams(ServiceParams* params);
    ServiceParams(InputStream& inputStream);
    virtual ~ServiceParams();
    
    ServiceParams* duplicate();

    FTServiceParamsPtr& getFTParams();
    UUIDPtr& getSID();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

    ACE_Message_Block* getQuerySpecializationBlob();
    ACE_Message_Block* deattachQuerySpecializationBlob();
    void getQuerySpecializationBlob(ACE_Message_Block* querySpecializationBlob);

protected:
    UUIDPtr m_sid;
    FTServiceParamsPtr m_ftParams;
    ACE_Message_Block* m_querySpecializationBlob;    
        
    ServiceParams(const UUIDPtr& sid, FTServiceParamsPtr& ftParams,ACE_Message_Block* querySpecializationBlob);

    virtual void serializeMetadata(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeMetadata(InputStream& inputStream) throw (SerializationException&);
    virtual UInt getSerializationSizeQuerySpecialization() throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
};

typedef ACE_Strong_Bound_Ptr<ServiceParams, ACE_SYNCH_MUTEX> ServiceParamsPtr;
#endif	/* SERVICEPARAMS_H */

