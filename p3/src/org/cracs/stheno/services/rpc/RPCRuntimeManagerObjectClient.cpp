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
 * File:   RPCRuntimeManagerObjectClient.cpp
 * Author: rmartins
 * 
 * Created on February 8, 2011, 11:00 AM
 */

#include "RPCRuntimeManagerObjectClient.h"


RPCRuntimeManagerObjectClient::RPCRuntimeManagerObjectClient(RPCServiceClient* rpcRPCServiceClient):
m_rpcRPCServiceClient(rpcRPCServiceClient),m_oid(new UUID("67F12BAC6A3B5C704D45BAE532416804"))
{
}

RPCRuntimeManagerObjectClient::~RPCRuntimeManagerObjectClient() {
    close();
}



