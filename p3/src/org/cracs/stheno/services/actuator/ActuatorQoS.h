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
 * File:   ActuatorQoS.h
 * Author: rmartins
 *
 * Created on October 26, 2010, 1:20 PM
 */

#ifndef ActuatorQOS_H
#define	ActuatorQOS_H
#include <euryale/serialization/Serializable.h>

class ActuatorQoS {
public:
    
    ActuatorQoS(UInt frameRate,UInt frameSize);
    ActuatorQoS(InputStream& inputActuator)  throw (SerializationException&);
    virtual ~ActuatorQoS();

    UInt getFrameRate(){
        return m_frameRate;
    }
    UInt getFrameSize(){
        return m_frameSize;
    }
    
    virtual void serialize(OutputStream& outputActuator) throw (SerializationException&);
    virtual void deserialize(InputStream& inputActuator) throw (SerializationException&);

protected:
    UInt m_frameRate;
    UInt m_frameSize;


};

#endif	/* ActuatorQOS_H */

