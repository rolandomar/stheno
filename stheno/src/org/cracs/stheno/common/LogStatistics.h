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
 * File:   LogStatistics.h
 * Author: rmartins
 *
 * Created on January 7, 2011, 12:45 PM
 */

#ifndef LOGSTATISTICS_H
#define	LOGSTATISTICS_H

#include <euryale/Types.h>
#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>

class LogStatistics {
public:
    LogStatistics();    
    virtual ~LogStatistics();
    
    ACE_OSTREAM_TYPE* getOutputStream();
    void logLine(String& line);
    void open(String& filename);
    void open(const char* filename);
    void close();
protected:
    String m_filename;
    bool m_started;
    bool m_closed;
    ACE_OSTREAM_TYPE *m_outputStream;

};

typedef ACE_Singleton<LogStatistics, ACE_Thread_Mutex>  LogStatisticsSingleton;

#endif	/* LOGSTATISTICS_H */

