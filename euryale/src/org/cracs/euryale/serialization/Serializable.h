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
#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_
/************************************
 *
 *
 * Serialization interface
 *
 *
 ************************************/
#include <euryale/Types.h>
#include <euryale/common/io/InputStream.h>
#include <euryale/common/io/OutputStream.h>
#include <euryale/common/io/StreamSize.h>
#include <euryale/serialization/SerializationException.h>
#include <euryale/common/io/Alignment.h>


class Serializable {
public:
    /**
     * Document serialize(OutputStream&) here...
     * @param outputStream
     */
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&) = 0;
    /**
     * Document deserialize(InputStream&) here...
     * @param inputStream
     */
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) = 0;

    /// Serialization size depends on the alignment
    //virtual UInt getSerializationSize(Alignment* align = 0) throw (SerializationException&) = 0;
    /**
     * Document getSerializationSize(StreamSize&) here...
     * @param streamSize
     * @return ...
     */
    virtual UInt getSerializationSize(StreamSize& streamSize) throw (SerializationException&);    
    virtual UInt getSerializationSize();
};


#endif /*SERIALIZABLE_H_*/
