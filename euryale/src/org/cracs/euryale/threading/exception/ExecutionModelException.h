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
 * File:   ExecutionModelException.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 15, 2010, 10:44 AM
 */

#ifndef EXECUTIONMODELEXCEPTION_H
#define	EXECUTIONMODELEXCEPTION_H

#include <euryale/common/exception/Exception.h>

class ExecutionModelException : public Exception {
public:

    /*static const char* LOCK_ERROR;
    static const char* INITIALIZATION_ERROR;
    static const char* BAD_BINDING;
    static const char* TIMEOUT;
    static const char* JOIN_FAILED;*/
    static const char* msgArray[];

    enum {
        CLOSED_ERROR = 0, LOCK_ERROR, INITIALIZATION_ERROR, BAD_BINDING, TIMEOUT,
        JOIN_FAILED, ERROR_ELECTING_LEADER, ERROR_BECOMING_LEADER,NO_HANDLER
    };

    //ExecutionModelException(const char* exception);
    ExecutionModelException(int type);
    ExecutionModelException(int type, const char* exception);
    ExecutionModelException(const ExecutionModelException&);
    virtual ~ExecutionModelException();

    virtual void raise(void) {
        throw *this;
    }

    int getType() {
        return m_type;
    }

    virtual const String& message() {
        return *m_messageString;
    }

    virtual const char* objectID() const {
        return "d2214bb30fadf657573b1687d34b5129";
    }

    virtual const String& toString() {
        return this->message();
    }

private:
    int m_type;
    String* m_messageString;
};

#endif	/* EXECUTIONMODELEXCEPTION_H */

