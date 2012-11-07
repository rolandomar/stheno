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
 * File:   CellAddr.cpp
 * Author: rmartins
 * 
 * Created on August 24, 2010, 3:05 PM
 */

#include "CellAddr.h"
CellAddr::CellAddr(InputStream& inputStream){
    this->deserialize(inputStream);
}
CellAddr::CellAddr(CellIDPtr& cellID,String itf,String ip,Short port):
m_cellID(cellID),m_itf(itf),m_ip(ip),m_port(port){
}

CellAddr::CellAddr(const CellAddr& orig):
m_cellID(orig.m_cellID),m_itf(orig.m_itf),m_ip(orig.m_ip),m_port(orig.m_port)
{
}

CellAddr::~CellAddr() {
}

