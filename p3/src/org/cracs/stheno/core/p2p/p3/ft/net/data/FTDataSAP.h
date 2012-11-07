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
 * File:   FTDataSAP.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 9:32 AM
 */

#ifndef FTDataSAP_H
#define	FTDataSAP_H

#include <euryale/net/endpoint/SAP.h>
#include <stheno/core/p2p/p3/ft/net/data/FTDataAcceptor.h>

class P3ReplicationGroup;
class P3FaultTolerance;
class FTDataAcceptor;

class FTDataSAP: public SAP<FTDataAcceptor> {
public:
    FTDataSAP(P3ReplicationGroup* rg);    
    virtual ~FTDataSAP();
    
protected:
    P3ReplicationGroup* m_ft;        
    FTDataAcceptor* createAcceptor();
};

#endif	/* FTDataSAP_H */

