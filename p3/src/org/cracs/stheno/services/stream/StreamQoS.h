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
 * File:   StreamQoS.h
 * Author: rmartins
 *
 * Created on October 26, 2010, 1:20 PM
 */

#ifndef STREAMQOS_H
#define	STREAMQOS_H
#include <euryale/serialization/Serializable.h>

class StreamQoS {
public:
    
    StreamQoS(UInt frameRate,UInt frameSize);
    StreamQoS(InputStream& inputStream)  throw (SerializationException&);
    virtual ~StreamQoS();

    UInt getFrameRate(){
        return m_frameRate;
    }
    UInt getFrameSize(){
        return m_frameSize;
    }
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

protected:
    UInt m_frameRate;
    UInt m_frameSize;


};

#endif	/* STREAMQOS_H */

