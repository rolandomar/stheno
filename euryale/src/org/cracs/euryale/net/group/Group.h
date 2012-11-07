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
 * File:   Group.h
 * Author: rmartins
 *
 * Created on February 27, 2012, 5:42 PM
 */

#ifndef GROUP_H
#define	GROUP_H

#include <euryale/net/group/GMS.h>

class Group {
public:
    enum {FIFO_ORDER = 1, CAUSAL_ORDER = 2, TOTAL_ORDER = 3};
    Group();
    Group(const Group& orig);
    virtual ~Group();
    
    virtual const GMS* getGMS() = 0;
    int getOrder();
protected:
    //GMS* m_gms;
    int m_order;
};

#endif	/* GROUP_H */

