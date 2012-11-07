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
 * File:   RuntimeException.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 25, 2010, 4:33 PM
 */

#include "RuntimeException.h"

const char* RuntimeException::QOS_CLIENT_INITIALIZATION_ERROR = "QoS Client failed!";
const char* RuntimeException::INSUFFICIENT_RESOURCES = "Insufficient resources to bootstrap runtime!";
const char* RuntimeException::INVALID_OVERLAY = "Overlay not found!";
const char* RuntimeException::INVALID_RUNTIME = "Runtime has an invalid state!";
const char* RuntimeException::INVALID_ARGS = "Invalid args!";

RuntimeException::RuntimeException(const char* exception) : m_messageString(exception) {


}

RuntimeException::RuntimeException(const RuntimeException& c) : m_messageString(c.m_messageString) {
}

RuntimeException::~RuntimeException() {
}

const String& RuntimeException::message() {
    return m_messageString;
}

const char* RuntimeException::objectID() const {
    return "b7814bb30fad0657573b1687d34b5129";
}

const String& RuntimeException::toString() {
    return message();
}