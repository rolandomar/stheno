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
 * NullPointerException.cpp
 *
 *  Created on: Jan 18, 2008
 *      Author: rmartins
 */

#include "NullPointerException.h"

namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

namespace exception {

namespace system {

const char* NullPointerException::NullPointerExceptionMessage = "Null pointer exception";

NullPointerException::NullPointerException():m_messageString(0) {
	// TODO Auto-generated constructor stub

}

NullPointerException::~NullPointerException() {
	// TODO Auto-generated destructor stub
}

}

}

}

}

}

}
