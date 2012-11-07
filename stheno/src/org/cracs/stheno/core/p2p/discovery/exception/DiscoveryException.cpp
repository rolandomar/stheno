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
 * File:   DiscoveryException.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 14, 2010, 4:00 PM
 */

#include "DiscoveryException.h"

const char* DiscoveryException::INVALID_OVERLAY = "Overlay not found, discovery failure!";
const char* DiscoveryException::NO_REPLY_AVAILABLE = "No reply was available!";


DiscoveryException::DiscoveryException(const char* msg) :
m_message(msg) {
}

DiscoveryException::DiscoveryException(String& message) :
m_message(message) {
}

DiscoveryException::DiscoveryException(InputStream& inputStream) throw (SerializationException&) {
    this->deserialize(inputStream);
}

DiscoveryException::DiscoveryException(const DiscoveryException& orig) :
m_message(orig.m_message) {
}

DiscoveryException::~DiscoveryException() {
}

void DiscoveryException::operator=(const DiscoveryException& orig) {
    m_message = orig.m_message;
}

const String& DiscoveryException::message() {
    return m_message;
}

const char* DiscoveryException::objectID() const {
    return "6A3D73859FC70AC20F11A89E4C039C59";
}

const String& DiscoveryException::toString() {
    return m_message;
}

void DiscoveryException::serialize(OutputStream& outputStream) throw (SerializationException&) {
    String strObjectID(objectID());
    outputStream.write_string(strObjectID);
}

void DiscoveryException::deserialize(InputStream& inputStream) throw (SerializationException&) {
    String strObjectID;
    //todo: check if id is corrects
    inputStream.read_string(strObjectID);
}
