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
 * File:   RPCServiceSAP.h
 * Author: rmartins
 *
 * Created on January 21, 2011, 4:24 PM
 */

#ifndef RPCSERVICESAP_H
#define	RPCSERVICESAP_H

#include <euryale/net/endpoint/SAP.h>
#include <stheno/services/rpc/net/RPCServiceAcceptor.h>

class RPCService;
class RPCServiceSAP: public SAP<RPCServiceAcceptor> {
public:
    RPCServiceSAP(RPCService* discovery);    
    virtual ~RPCServiceSAP();
    
protected:
    RPCService* m_discovery;    
    //virtual abstract method impl
    RPCServiceAcceptor* createAcceptor();

};


#endif	/* RPCSERVICESAP_H */

