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
 * File:   CellSvcHandler.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 9, 2010, 12:59 PM
 */

#include "CellSvcHandler.h"

CellSvcHandler::~CellSvcHandler() {
    /*if (m_requests != 0) {
        delete m_requests;
        m_requests = 0;
    }*/
    close();
}

