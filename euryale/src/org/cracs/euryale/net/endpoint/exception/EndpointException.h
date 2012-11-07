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
 * File:   EndpointException.h
 * Author: rmartins
 *
 * Created on August 18, 2010, 4:06 PM
 */

#ifndef ENDPOINTEXCEPTION_H
#define	ENDPOINTEXCEPTION_H

#include <euryale/common/exception/Exception.h>

class EndpointException : public Exception {
public:
    static const char* NULL_ENDPOINTPTR;
    
    EndpointException(const char* exception);
    EndpointException(const EndpointException&);
    virtual ~EndpointException();

    virtual const String& message() {
        return m_messageString;
    }

    virtual const char* objectID() const {
        return "C0D55E844A105428308FE698B7CD0D45";
    }

    virtual const String& toString() {
        return this->message();
    }

private:
    String m_messageString;
};
#endif	/* ENDPOINTEXCEPTION_H */

