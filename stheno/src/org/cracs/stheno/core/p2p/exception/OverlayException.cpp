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
 * File:   OverlayException.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 12, 2010, 11:38 AM
 */

#include "OverlayException.h"

const char* OverlayException::INVALID_OVERLAY = "Overlay not found!";
const char* OverlayException::PRIMITIVE_NOT_IMPLEMENTED = "Overlay does not implement primitive!";
const char* OverlayException::BOOTSTRAP_FAILED = "Overlay bootstrap failed!";


OverlayException::OverlayException(const char* exception) : m_messageString(exception) {
}

OverlayException::OverlayException(const String& message) : m_messageString(message) {
}

OverlayException::OverlayException(const OverlayException& c) : m_messageString(c.m_messageString) {
}

OverlayException::~OverlayException() {
}

const String& OverlayException::message() {
    return m_messageString;
}

const char* OverlayException::objectID() const {
    return "fff14bb30fad0657573b1687d34b5129";
}

const String& OverlayException::toString() {
    return message();
}
