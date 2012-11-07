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
 * File:   ServiceException.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 22, 2010, 10:54 AM
 */

#ifndef SERVICEEXCEPTION_H
#define	SERVICEEXCEPTION_H

#include <euryale/common/exception/Exception.h>
#include <euryale/common/Integer.h>

class ServiceException : public Exception {
public:
    static const char* invalidStateMsg(int oldState,int newState){
        String msg = "Invalid state transition (";
        msg += Integer::toUnsignedString(oldState,0);
        msg += ",";
        msg += Integer::toUnsignedString(newState,0);
        return msg.c_str();        
    }
    
    static const char* INVALID_STATE;
    static const char* INVALID_FT_PARAMS;
    static const char* NON_FT_SERVICE;
    static const char* INSUFFICIENT_RESOURCES;
    static const char* SERVICE_NOT_KNOWN;
    static const char* SERVICE_WITHOUT_IMPL;
    static const char* REGISTRATION_ERROR;
    static const char* INVALID_ARGUMENT;
    static const char* LOW_LVL_P2P_UNAVAILABLE;

    
    ServiceException(const char* exception);
    ServiceException(const ServiceException&);
    virtual ~ServiceException();

    virtual void raise(void) {
        throw *this;
    }

    virtual const String& message() {
        return m_messageString;
    }

    virtual const char* objectID() const {
        return "b7814bb30fad0657573b1687d34b5129";
    }

    virtual const String& toString() {
        return message();
    }
    

private:
    String m_messageString;
};

#endif	/* SERVICEEXCEPTION_H */

