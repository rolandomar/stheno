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
 * File:   CgroupException.h
 * Author: rmartins
 *
 * Created on November 30, 2009, 11:28 AM
 */

#ifndef _CGROUPEXCEPTION_H
#define	_CGROUPEXCEPTION_H


#include <euryale/common/exception/Exception.h>

/*namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

namespace qos {

namespace hadesd {

namespace exception {*/

class CgroupException : public Exception {
public:
    CgroupException(const char* exception);
    virtual ~CgroupException();

    virtual void raise(void) {
        throw *this;
    }

    virtual const String& message() {
        return *m_messageString;
    }

    virtual const char* objectID() const {
        return "b129a7830fbff657573b1687d34b5be0";
    }

    virtual const String& toString() {
        return this->message();
    }

private:
    String* m_messageString;
};


/*}

}

}

}

}

}

}

using pt::efacec::rto::runtime::qos::hadesd::exception::CgroupException;
 */
#endif	/* _CGROUPEXCEPTION_H */

