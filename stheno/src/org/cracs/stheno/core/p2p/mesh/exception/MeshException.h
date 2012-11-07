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
 * File:   MembershipException.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 12, 2010, 11:33 AM
 */

#ifndef MESHEXCEPTION_H
#define	MESHEXCEPTION_H

#include <euryale/common/exception/Exception.h>
#include <euryale/serialization/Serializable.h>

class MeshException: public Exception,public Serializable {
public:
    MeshException(String& message);
    MeshException(InputStream& inputStream) throw (SerializationException&);
    MeshException(const MeshException& orig);
    virtual ~MeshException();
    
    void operator=(const MeshException& orig);
    
    virtual const String& message();
    virtual const char* objectID() const;    
    virtual const String& toString();
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    
protected:
    String m_message;
};


#endif	/* MESHEXCEPTION_H */

