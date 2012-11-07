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
 * File:   Runtime.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 10, 2010, 1:00 PM
 */

#include "Runtime.h"

CPPUNIT_TEST_SUITE_REGISTRATION(Runtime);

#include <stheno/Stheno.h>
#include <stdio.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/common/uuid/UUIDDistance.h>

Runtime::Runtime() {
}

Runtime::~Runtime() {
}

void Runtime::setUp() {
    
}

void Runtime::tearDown() {
    
}

void Runtime::testMethod() {
    //Stheno runtime;
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)\runtimeTest\n")));
    UUID uuid("00000000000000000000000000000000");
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)->%s\n"),uuid.toString().c_str()));
    //CellID id("01110000000000000000000000000000");
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)DEPTH=%d\n",id.getCellDepth());    
    UUID uuid2("094c57041a707dc479c86059be42ac34");
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)->%s\n"),uuid2.toString().c_str()));
    int ret = UUIDDistance::LD(&uuid,&uuid);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)UUID1=%s UUID2=%s ret=%d compare=%d\n"),uuid.toString().c_str(),uuid.toString().c_str(),ret,uuid.compare(uuid)));

    UUID* u1 = UUID::generateUUID();
    UUID* u2 = UUID::generateUUID();
    ret = UUIDDistance::LD(u1,u2);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)U1=%s U2=%s LD=%d compare=%d\n "),u1->toString().c_str(),u2->toString().c_str(),ret,u1->compare(*u2)));
    delete u1;
    delete u2;
    CPPUNIT_ASSERT(true);
}
