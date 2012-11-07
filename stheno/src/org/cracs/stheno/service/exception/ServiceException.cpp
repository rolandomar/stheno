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
 * File:   ServiceException.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 22, 2010, 10:54 AM
 */

#include "ServiceException.h"

const char* ServiceException::INVALID_STATE = "Invalid State";
const char* ServiceException::INVALID_FT_PARAMS = "Invalid FT params.";
const char* ServiceException::NON_FT_SERVICE = "Non-FT service!";
const char* ServiceException::REGISTRATION_ERROR = "Service registration failed.";
const char* ServiceException::INSUFFICIENT_RESOURCES = "Insufficient resources.";
const char* ServiceException::SERVICE_NOT_KNOWN = "Service not known.";
const char* ServiceException::SERVICE_WITHOUT_IMPL = "Service without known implementation.";
const char* ServiceException::LOW_LVL_P2P_UNAVAILABLE = "Low level P2P is unable.";
const char* ServiceException::INVALID_ARGUMENT = "Invalid argument.";


ServiceException::ServiceException(const char* exception) : m_messageString(exception) {


}

ServiceException::ServiceException(const ServiceException& c) : m_messageString(c.m_messageString) {
}

ServiceException::~ServiceException() {
}


