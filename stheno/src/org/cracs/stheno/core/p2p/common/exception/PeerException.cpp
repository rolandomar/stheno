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
 * File:   PeerException.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 22, 2010, 2:36 PM
 */

#include "PeerException.h"

PeerException::PeerException(const char* exception) : m_messageString(exception) {


}

PeerException::PeerException(const PeerException& c) : m_messageString(c.m_messageString) {
}

PeerException::~PeerException() {
}

const String& PeerException::message() {
    return m_messageString;
}

const char* PeerException::objectID() const {
    return "b7814bb30fad0657573b1687d34b5129";
}

const String& PeerException::toString() {
    return message();
}