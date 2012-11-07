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
 * File:   ActuatorException.cpp
 * Author: rmartins
 * 
 * Created on January 22, 2011, 7:40 PM
 */

#include "ActuatorException.h"

const char* ActuatorException::QOS_CLIENT_INITIALIZATION_ERROR = "QoS Client failed!";
const char* ActuatorException::INSUFFICIENT_RESOURCES = "Insufficient resources to bootstrap runtime!";
const char* ActuatorException::INVALID_OVERLAY = "Overlay not found!";
const char* ActuatorException::INVALID_RUNTIME = "Runtime has an invalid state!";

ActuatorException::ActuatorException(const char* exception) : m_messageString(exception) {


}

ActuatorException::ActuatorException(const ActuatorException& c) : m_messageString(c.m_messageString) {
}

ActuatorException::~ActuatorException() {
}

const String& ActuatorException::message() {
    return m_messageString;
}

const char* ActuatorException::objectID() const {
    return "f1514bb30fad0657573b1687d34b5129";
}

const String& ActuatorException::toString() {
    return message();
}