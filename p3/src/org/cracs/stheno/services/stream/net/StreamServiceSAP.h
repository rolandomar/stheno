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

#ifndef STREAMSERVICESAP_H
#define	STREAMSERVICESAP_H

#include <euryale/net/endpoint/SAP.h>
#include <stheno/services/stream/net/StreamServiceAcceptor.h>

class StreamService;
class StreamServiceSAP: public SAP<StreamServiceAcceptor> {
public:
    StreamServiceSAP(StreamService* discovery);    
    virtual ~StreamServiceSAP();
    
protected:
    StreamService* m_discovery;    
    //virtual abstract method impl
    StreamServiceAcceptor* createAcceptor();

};

#endif	/* STREAMSERVICESAP_H */

