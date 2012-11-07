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
 * File:   CellDiscoveryException.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 26, 2010, 4:04 PM
 */

#ifndef CELLDISCOVERYEXCEPTION_H
#define	CELLDISCOVERYEXCEPTION_H

class CellDiscoveryException {
public:
    CellDiscoveryException();
    CellDiscoveryException(const CellDiscoveryException& orig);
    virtual ~CellDiscoveryException();
private:

};

#endif	/* CELLDISCOVERYEXCEPTION_H */

