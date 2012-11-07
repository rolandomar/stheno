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
 * File:   UUID.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 11, 2010, 4:03:47 PM
 */

#include "UUIDTest.h"


CPPUNIT_TEST_SUITE_REGISTRATION(UUIDTest);

#include <euryale/common/uuid/UUID.h>
#include <iostream>
using namespace std;
UUIDTest::UUIDTest() {
}

UUIDTest::~UUIDTest() {
}

void UUIDTest::setUp() {
}

void UUIDTest::tearDown() {
}

void UUIDTest::testMethod() {
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)\nUUIDTest\n")));
    UUID* uuid = UUID::generateUUID();
    cout << endl << uuid->toString() << endl;
    delete uuid;
    CPPUNIT_ASSERT(true);
}

