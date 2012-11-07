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
 * File:   ChannelException.h
 * Author: rmartins
 *
 * Created on December 4, 2009, 1:25 PM
 */

#ifndef _CHANNELEXCEPTION_H
#define	_CHANNELEXCEPTION_H

#include <euryale/common/exception/Exception.h>

class ChannelException : public Exception {
public:
    ChannelException(const char* exception);
    ChannelException(const ChannelException&);
    virtual ~ChannelException();

    virtual void raise(void) {
        throw *this;
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
    String* m_messageString;
};



#endif	/* _CHANNELEXCEPTION_H */

