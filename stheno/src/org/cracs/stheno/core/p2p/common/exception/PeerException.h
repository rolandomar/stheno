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
 * File:   PeerException.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 22, 2010, 2:36 PM
 */

#ifndef PEEREXCEPTION_H
#define	PEEREXCEPTION_H

#include <euryale/common/exception/Exception.h>

class PeerException : public Exception {
public:
    PeerException(const char* exception);
    PeerException(const PeerException&);
    virtual ~PeerException();
   
    virtual const String& message();

    virtual const char* objectID() const;

    virtual const String& toString();

private:
    String m_messageString;
};


#endif	/* PEEREXCEPTION_H */

