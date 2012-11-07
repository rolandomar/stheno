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
 * File:   IOReservationGroup.h
 * Author: rmartins
 *
 * Created on November 27, 2009, 3:37 PM
 */

#ifndef _IORESERVATIONGROUP_H
#define	_IORESERVATIONGROUP_H

#include <ace/OS_NS_Thread.h>
#include <euryale/qos/ReservationGroup.h>
#include <euryale/qos/io_qos/IOReservation.h>
#include <euryale/qos/io_qos/IOQoS.h>


class IOReservationGroup: public ReservationGroup{
public:
    IOReservationGroup();
    ~IOReservationGroup();

    IOReservation* reserveIO(ACE_hthread_t handle,IOQoS& qos){return 0;}

    IOReservationGroup* createSubgroup(){return 0;}
    void deleteSubgroup(IOReservationGroup* group){}
};

#endif	/* _IORESERVATIONGROUP_H */

