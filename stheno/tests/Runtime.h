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
 * File:   Runtime.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 10, 2010, 1:00 PM
 */

#ifndef RUNTIME_H
#define	RUNTIME_H

#include <cppunit/extensions/HelperMacros.h>

class Runtime : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(Runtime);
    CPPUNIT_TEST(testMethod);    
    CPPUNIT_TEST_SUITE_END();

public:
    Runtime();
    virtual ~Runtime();
    void setUp();
    void tearDown();

private:   
    void testMethod();    
};

#endif	/* RUNTIME_H */

