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
 * File:   MembershipException.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 12, 2010, 11:33 AM
 */

#include "MeshException.h"

MeshException::MeshException(String& message) :
m_message(message) {
}

MeshException::MeshException(InputStream& inputStream) throw (SerializationException&) {
    this->deserialize(inputStream);
}

MeshException::MeshException(const MeshException& orig) :
m_message(orig.m_message) {
}

MeshException::~MeshException() {
}

void MeshException::operator=(const MeshException& orig) {
    m_message = orig.m_message;
}

const String& MeshException::message() {
    return m_message;
}

const char* MeshException::objectID() const {
    return "888D73859FC70AC20F11A89E4C039C59";
}

const String& MeshException::toString() {
    return m_message;
}

void MeshException::serialize(OutputStream& outputStream) throw (SerializationException&) {
    String strObjectID(objectID());
    outputStream.write_string(strObjectID);
    outputStream.write_string(m_message);
}

void MeshException::deserialize(InputStream& inputStream) throw (SerializationException&) {
    String strObjectID;
    //todo: check if id is corrects
    inputStream.read_string(strObjectID);
    inputStream.read_string(m_message);
}

