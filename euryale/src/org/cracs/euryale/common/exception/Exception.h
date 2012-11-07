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
#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <euryale/common/Object.h>
//#include <euryale/serialization/Serializable.h>

class Exception : public Object {
public:

    Exception() {}
    Exception(const Exception& ex):Object(ex) {}
    virtual ~Exception() {}

    virtual const String& message() = 0;
    virtual const char* objectID() const = 0;
    virtual const String& toString() = 0;   
    
    /*virtual void serialize(OutputStream& outputStream) throw (SerializationException&) {
        String idStr(objectID());
        outputStream.write_string(idStr);
        String str(message());
        outputStream.write_string(str);
    }
    
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        String idStr;
        inputStream.read_string(idStr);    
        String str;
        inputStream.read_string(str);
        setMessage(str);
    }*/
    
    //virtual void setMessage(String& str) = 0;

};

#endif /*EXCEPTION_H_*/
