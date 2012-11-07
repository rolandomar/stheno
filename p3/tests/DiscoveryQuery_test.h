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
 * File:   DiscoveryQuery_test.h
 * Author: rmartins
 *
 * Created on Sep 21, 2010, 9:40:23 AM
 */

#ifndef DISCOVERYQUERY_TEST_H
#define	DISCOVERYQUERY_TEST_H

#include <cppunit/extensions/HelperMacros.h>

class DiscoveryQuery_test : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(DiscoveryQuery_test);

    CPPUNIT_TEST(testMethod);
    //CPPUNIT_TEST(testFailedMethod);

    CPPUNIT_TEST_SUITE_END();

public:
    DiscoveryQuery_test();
    virtual ~DiscoveryQuery_test();
    void setUp();
    void tearDown();

private:
    void testMethod();
    //void testFailedMethod();
};

#endif	/* DISCOVERYQUERY_TEST_H */

