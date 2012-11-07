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
 * File:   UUIDDistance.h
 * Author: rmartins
 *
 * Created on August 16, 2010, 12:10 PM
 */

#ifndef UUIDDISTANCE_H
#define	UUIDDISTANCE_H
#include <euryale/common/uuid/UUID.h>
class UUIDDistance {
public:
    UUIDDistance();    
    virtual ~UUIDDistance();
    static int LD(UUIDPtr& u1,UUIDPtr& u2);
    static int LD(UUID* u1, UUID* u2);
    
private:
    //*****************************
    // Compute Levenshtein distance
    //*****************************
    static int LD(String& s, String& t);
    //****************************
    // Get minimum of three values
    //****************************
    static int Minimum(int a, int b, int c);
};

#endif	/* UUIDDISTANCE_H */
