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
 * File:   LogStatistics.cpp
 * Author: rmartins
 * 
 * Created on January 7, 2011, 12:45 PM
 */
#include "ace/Log_Msg.h"
#include "ace/streams.h"

#include "LogStatistics.h"

LogStatistics::LogStatistics() :
m_closed(false), m_started(false), m_outputStream(0) {
}

LogStatistics::~LogStatistics() {
    close();
}

void LogStatistics::close() {
    if (m_closed) {
        return;
    }
    m_closed = true;
    if (m_outputStream != 0) {
        delete m_outputStream;
        m_outputStream = 0;
    }
}

void LogStatistics::open(String& filename) {
    if (m_started) {
        return;
    }
    m_closed = false;
    m_started = true;
    m_filename = filename;
    m_outputStream = new std::ofstream(m_filename.c_str());
}
void LogStatistics::open(const char* filename) {
    if (m_started) {
        return;
    }
    m_closed = false;
    m_started = true;
    m_filename = filename;
    m_outputStream = new std::ofstream(filename);
}

ACE_OSTREAM_TYPE* LogStatistics::getOutputStream() {
    return m_outputStream;
}

void LogStatistics::logLine(String& line) {
    if(m_outputStream==0){        
        return;
    }   
    (*m_outputStream) << line;
    m_outputStream->flush();
}