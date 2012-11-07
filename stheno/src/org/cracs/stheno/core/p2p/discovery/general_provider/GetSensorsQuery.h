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
 * File:   GetSensorsQuery.h
 * Author: rmartins
 *
 * Created on October 14, 2010, 12:28 PM
 */

#ifndef GETSENSORSQUERY_H
#define	GETSENSORSQUERY_H

#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/common/io/ByteInputStream.h>

class GetSensorsQuery : public DiscoveryQuery {
public:

    static const int GETSENSORS_QUERY = 0x1213AA;    
    
    GetSensorsQuery();
    GetSensorsQuery(DiscoveryQuery* query);
    GetSensorsQuery(const GetSensorsQuery& orig);
    virtual ~GetSensorsQuery();

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
protected:
    
    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);    
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);
};



#endif	/* GETSENSORSQUERY_H */

