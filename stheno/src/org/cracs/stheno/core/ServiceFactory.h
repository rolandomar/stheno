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
 * File:   ServiceFactory.h
 * Author: rmartins
 *
 * Created on August 2, 2010, 12:03 PM
 */

#ifndef SERVICEFACTORY_H
#define	SERVICEFACTORY_H

#include <stheno/service/exception/ServiceException.h>
#include <stheno/service/ServiceAbstract.h>


class ServiceFactory {
public:
    /*ServiceFactory();    
    virtual ~ServiceFactory();*/
    
    virtual ServiceAbstract* getInstance(const UUIDPtr& sid) throw(ServiceException&) = 0;
    virtual bool addService(ServiceAbstract* serviceImpl) = 0;    
private:

};


#endif	/* SERVICEFACTORY_H */

