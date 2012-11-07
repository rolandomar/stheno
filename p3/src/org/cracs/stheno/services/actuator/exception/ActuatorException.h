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
 * File:   ActuatorException.h
 * Author: rmartins
 *
 * Created on January 22, 2011, 7:40 PM
 */

#ifndef ActuatorException_H
#define	ActuatorException_H

#include <euryale/common/exception/Exception.h>

class ActuatorException : public Exception {
public:
    
    static const char* QOS_CLIENT_INITIALIZATION_ERROR;
    static const char* INSUFFICIENT_RESOURCES;
    static const char* INVALID_OVERLAY;
    static const char* INVALID_RUNTIME;
        
    ActuatorException(const char* exception);
    ActuatorException(const ActuatorException&);
    virtual ~ActuatorException();
    
    virtual const String& message();
    virtual const char* objectID() const;
    virtual const String& toString();
    
private:
    String m_messageString;
};
#endif	/* ActuatorException_H */

