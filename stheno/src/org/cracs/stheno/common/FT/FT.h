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
 * File:   FT.h
 * Author: rmartins
 *
 * Created on October 1, 2010, 10:02 AM
 */

#ifndef FT_H
#define	FT_H
#include <euryale/Types.h>
class FT {
public:
    FT();
    FT(const FT& orig);
    virtual ~FT();
    
    
    static const Byte FT_ACTIVE_REPLICATION_POLICY= 0x00;
    static const Byte FT_PASSIVE_REPLICATION_POLICY= 0x01;
    
private:

};

#endif	/* FT_H */

