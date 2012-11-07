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
 * File:   NetReservationGroup.h
 * Author: rmartins
 *
 * Created on November 27, 2009, 3:38 PM
 */

#ifndef _NETRESERVATIONGROUP_H
#define	_NETRESERVATIONGROUP_H
#include <ace/OS_NS_Thread.h>
#include <euryale/qos/ReservationGroup.h>
#include <euryale/qos/net_qos/NetReservation.h>
#include <euryale/qos/net_qos/NetQoS.h>

class NetReservationGroup: public ReservationGroup{
public:
    NetReservationGroup();
    ~NetReservationGroup();

    NetReservation* reserveNetwork(ACE_hthread_t handle,NetQoS& qos){return 0;}

    NetReservationGroup* createSubgroup(){return 0;}
    void deleteSubgroup(NetReservationGroup* group){}
};



#endif	/* _NETRESERVATIONGROUP_H */

