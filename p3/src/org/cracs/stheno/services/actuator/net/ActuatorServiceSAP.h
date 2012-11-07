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
 * File:   StreamServiceSAP.h
 * Author: rmartins
 *
 * Created on November 24, 2010, 12:08 PM
 */

#ifndef ActuatorServiceSAP_H
#define	ActuatorServiceSAP_H

#include <euryale/net/endpoint/SAP.h>
#include <stheno/services/actuator/net/ActuatorServiceAcceptor.h>

class ActuatorService;
class ActuatorServiceSAP: public SAP<ActuatorServiceAcceptor> {
public:
    ActuatorServiceSAP(ActuatorService* discovery);    
    virtual ~ActuatorServiceSAP();
    
protected:
    ActuatorService* m_discovery;    
    //virtual abstract method impl
    ActuatorServiceAcceptor* createAcceptor();

};

#endif	/* ActuatorServiceSAP_H */

