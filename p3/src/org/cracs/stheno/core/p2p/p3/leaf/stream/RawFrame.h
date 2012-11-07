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
 * File:   RawFrame.h
 * Author: rmartins
 *
 * Created on December 16, 2010, 11:40 PM
 */

#ifndef RAWFRAME_H
#define	RAWFRAME_H

#include <ace/Message_Block.h>
#include <euryale/Types.h>

class RawFrame {
public:
    RawFrame(ULong frameNo,ACE_Message_Block* payload);
    RawFrame(const RawFrame& orig);
    virtual ~RawFrame();
protected:
    ULong m_frameNo;
    ACE_Message_Block* m_payload;

};

#endif	/* RAWFRAME_H */

