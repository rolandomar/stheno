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
 * File:   NetReservation.cpp
 * Author: rmartins
 * 
 * Created on November 3, 2009, 5:49 PM
 */

#include "NetReservation.h"
#include <euryale/qos/net_qos/NetReservationGroup.h>
#include <euryale/qos/net_qos/NetQoS.h>


NetReservation::NetReservation(NetQoS* qos,NetReservationGroup* group)
{
}

NetReservation::~NetReservation()
{
}

NetReservationGroup* NetReservation::getReservation(){
    return 0;
}

NetQoS* NetReservation::getQoS(){
    return 0;
}

