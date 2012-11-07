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
#ifndef OBJECT_H_
#define OBJECT_H_

//#include <ace/SString.h>
#include <euryale/Types.h>
#include <ace/OS_NS_stdio.h>

class Object{
public:

    Object() : m_toString(0) {
        ACE_TRACE("Object::Object()");
    }

    Object(const Object& o) : m_toString(0) {
        ACE_TRACE("Object::Object(const Object&)");
        //lazy creation of rep. string
    }

    virtual ~Object() {
        ACE_TRACE("Object::~Object()");
        if (m_toString != 0)
            delete m_toString;
    }

    //define the const object ID
    virtual const char* objectID() const = 0;

    virtual const String& toString() {
        ACE_TRACE("Object::toString()");
        if (m_toString == 0) {
            char buffer[20];
            ACE_OS::sprintf(buffer, "Object(%p)", this);
            m_toString = new String(buffer);
        }
        return *m_toString;
    }

    virtual bool operator==(const Object& b) const {
        ACE_TRACE("Object::operator==");
        return (this == &b);
    }

protected:
    String* m_toString;

};

#endif /*OBJECT_H_*/
