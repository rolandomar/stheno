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
 * File:   CellException.h
 * Author: rmartins
 *
 * Created on July 26, 2010, 4:30 PM
 */

#ifndef CELLEXCEPTION_H
#define	CELLEXCEPTION_H

#include <euryale/common/exception/Exception.h>

class CellException : public Exception {
public:
    
    static const char* CELL_MERGE_DETECTED;
    static const char* MISSING_VALUE_FROM_PPROPERTIES;
    static const char* BIND_FAILED;
    static const char* FAILED_TO_LAUNCH_THREAD;
    static const char* FAILED_TO_BIND_ADDR;
    
        
    CellException(const char* exception);
    CellException(const CellException&);
    virtual ~CellException();
    
    virtual const String& message();
    virtual const char* objectID() const;
    virtual const String& toString();
    
private:
    String m_messageString;
};


#endif	/* CELLEXCEPTION_H */

