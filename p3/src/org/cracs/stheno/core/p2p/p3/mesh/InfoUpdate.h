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
 * File:   InfoUpdate.h
 * Author: rmartins
 *
 * Created on September 29, 2010, 12:09 PM
 */

#ifndef INFOUPDATE_H
#define	INFOUPDATE_H

#include <ace/Synch.h>
#include <ace/Bound_Ptr.h>


#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>

class InfoUpdate {
public:
   
    InfoUpdate(UInt type,UInt operation);
    InfoUpdate(InputStream& inputStream);
    InfoUpdate(const InfoUpdate& orig);
    virtual ~InfoUpdate();
            
    virtual UInt getType();
    virtual UInt getOperation();
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    
protected:
    UInt m_type;
    UInt m_operation;
    
    InfoUpdate();
    
};

typedef ACE_Strong_Bound_Ptr<InfoUpdate, ACE_Recursive_Thread_Mutex> InfoUpdatePtr;


#endif	/* INFOUPDATE_H */

