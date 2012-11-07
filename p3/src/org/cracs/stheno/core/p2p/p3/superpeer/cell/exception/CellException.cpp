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
 * File:   CellException.cpp
 * Author: rmartins
 * 
 * Created on July 26, 2010, 4:30 PM
 */

#include "CellException.h"

const char* CellException::CELL_MERGE_DETECTED = "Merge detected!";
const char* CellException::MISSING_VALUE_FROM_PPROPERTIES = "Missing value from properties file";
const char* CellException::BIND_FAILED = "Failed to bind to mesh!";
const char* CellException::FAILED_TO_LAUNCH_THREAD = "Failed to launch thread!";
const char* CellException::FAILED_TO_BIND_ADDR = "Failed to bind addr!";

CellException::CellException(const char* exception) : m_messageString(exception) {
}

CellException::CellException(const CellException& c) : m_messageString(c.m_messageString) {
}

CellException::~CellException() {
}

const String& CellException::message() {
    return m_messageString;
}

const char* CellException::objectID() const {
    return "b7814bb30fad0657573b1687d34b5129";
}

const String& CellException::toString() {
    return message();
}