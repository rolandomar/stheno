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
 * File:   FTControlSAP.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 9:32 AM
 */

#ifndef FTCONTROLSAP_H
#define	FTCONTROLSAP_H

#include <euryale/net/endpoint/SAP.h>
#include <stheno/core/p2p/p3/ft/net/control/FTControlAcceptor.h>

//class ReplicationGroup;
class P3FaultTolerance;
class FTControlAcceptor;

class FTControlSAP: public SAP<FTControlAcceptor> {
public:
    FTControlSAP(P3FaultTolerance* rg);    
    virtual ~FTControlSAP();
    
protected:
    P3FaultTolerance* m_ft;        
    FTControlAcceptor* createAcceptor();
};

#endif	/* FTCONTROLSAP_H */

