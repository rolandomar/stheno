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
 * File:   RPCException.cpp
 * Author: rmartins
 * 
 * Created on January 22, 2011, 7:40 PM
 */

#include "RPCException.h"

const char* RPCException::QOS_CLIENT_INITIALIZATION_ERROR = "QoS Client failed!";
const char* RPCException::INSUFFICIENT_RESOURCES = "Insufficient resources to bootstrap runtime!";
const char* RPCException::INVALID_OVERLAY = "Overlay not found!";
const char* RPCException::INVALID_RUNTIME = "Runtime has an invalid state!";

RPCException::RPCException(const char* exception) : m_messageString(exception) {


}

RPCException::RPCException(const RPCException& c) : m_messageString(c.m_messageString) {
}

RPCException::~RPCException() {
}

const String& RPCException::message() {
    return m_messageString;
}

const char* RPCException::objectID() const {
    return "b7814bb30fad0657573b1687d34b5129";
}

const String& RPCException::toString() {
    return message();
}