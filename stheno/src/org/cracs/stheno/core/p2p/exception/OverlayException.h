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
 * File:   OverlayException.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 12, 2010, 11:38 AM
 */

#ifndef OVERLAYEXCEPTION_H
#define	OVERLAYEXCEPTION_H

#include <euryale/common/exception/Exception.h>

class OverlayException : public Exception {
public:
    
    static const char* INVALID_OVERLAY;
    static const char* PRIMITIVE_NOT_IMPLEMENTED;
    static const char* BOOTSTRAP_FAILED;
        
    OverlayException(const char* exception);
    OverlayException(const String& message);
    OverlayException(const OverlayException&);
    virtual ~OverlayException();
    
    virtual const String& message();
    virtual const char* objectID() const;
    virtual const String& toString();

private:
    String m_messageString;
};


#endif	/* OVERLAYEXCEPTION_H */

