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
 * File:   ContainerSerialization.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 13, 2010, 3:09 PM
 */

#ifndef CONTAINERSERIALIZATION_H
#define	CONTAINERSERIALIZATION_H

#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>
#include <ace/Containers_T.h>

template <class C>
class ContainerSerialization {
public:

    ContainerSerialization() {
    }

    virtual ~ContainerSerialization() {
    }

    static void serialize(list<C*>& container, OutputStream& outputStream) throw (SerializationException&) {
        UInt size = container->size();
        outputStream.write_ulong(size);
        if (size > 0) {
            C::iterator iter = container.begin();
            while (iter != container.end()) {
                (*iter)->serialize(outputStream);
                iter++;
            }
        }
    }

    static void deserialize(list<C*>& container, InputStream& inputStream) throw (SerializationException&) {
        container->clear();
        UInt size = 0;
        inputStream.read_ulong(size);
        if (size > 0) {
            for (int i = 0; i < size; i++) {
                C* item = new C();
                item->deserialize(inputStream);
                container.push_back(item);
            }
        }
    }



};



#endif	/* CONTAINERSERIALIZATION_H */

