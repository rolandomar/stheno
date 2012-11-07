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
 * File:   AudioQoS.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 8, 2010, 8:48 PM
 */

#ifndef AUDIOQOS_H
#define	AUDIOQOS_H

#include <euryale/Types.h>

class AudioQoS {
public:
    AudioQoS(UInt frameCount,UInt frameSize);
    virtual ~AudioQoS();

    UInt getFrameCount(){
        return m_frameCount;
    }
    UInt getFrameSize(){
        return m_frameSize;
    }

protected:
    UInt m_frameCount;
    UInt m_frameSize;
};

#endif	/* AUDIOQOS_H */

