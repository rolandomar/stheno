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
 * File:   PacketException.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 27, 2010, 5:00 PM
 */

#include "PacketException.h"


PacketException::PacketException(const char* exception){//:m_messageString(exception) {
    m_messageString = new String(exception);
	// TODO Auto-generated constructor stub

}

PacketException::~PacketException() {
	// TODO Auto-generated destructor stub
    if(m_messageString!=0){
        delete m_messageString;
    }
}
