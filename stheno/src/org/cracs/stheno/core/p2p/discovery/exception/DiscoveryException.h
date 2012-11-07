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
 * File:   DiscoveryException.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 14, 2010, 4:00 PM
 */

#ifndef DISCOVERYEXCEPTION_H
#define	DISCOVERYEXCEPTION_H

#include <euryale/common/exception/Exception.h>
#include <euryale/serialization/Serializable.h>

class DiscoveryException: public Exception,public Serializable {
public:
    static const char* INVALID_OVERLAY;
    static const char* NO_REPLY_AVAILABLE;
    
    
    DiscoveryException(const char* msg);
    DiscoveryException(String& message);
    DiscoveryException(InputStream& inputStream) throw (SerializationException&);
    DiscoveryException(const DiscoveryException& orig);
    virtual ~DiscoveryException();
    
    void operator=(const DiscoveryException& orig);
    
    virtual const String& message() ;
    virtual const char* objectID() const;
    
    virtual const String& toString();
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    
private:
    String m_message;
};

#endif	/* DISCOVERYEXCEPTION_H */

