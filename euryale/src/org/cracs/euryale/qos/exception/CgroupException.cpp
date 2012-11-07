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
 * File:   CgroupException.cpp
 * Author: rmartins
 * 
 * Created on November 30, 2009, 11:28 AM
 */

#include "CgroupException.h"

/*namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

namespace qos {

namespace hadesd {

namespace exception {*/

//const char* CgroupException::CgroupExceptionMessage = "CgroupExceptionException";


CgroupException::CgroupException(const char* exception){//:m_messageString(exception) {
    m_messageString = new String(exception);	
}

CgroupException::~CgroupException() {	
    if(m_messageString!=0){
        delete m_messageString;
    }
}

/*
}

}

}

}

}

}
}
*/