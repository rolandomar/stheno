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
 * NotImplementedException.h
 *
 *  Created on: Sep 4, 2008
 *      Author: rmartins
 */

#ifndef NOTIMPLEMENTEDEXCEPTION_H_
#define NOTIMPLEMENTEDEXCEPTION_H_

#include <euryale/common/exception/Exception.h>

namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

namespace exception {

namespace system {

class NotImplementedException : public Exception{
public:
	NotImplementedException();
	virtual ~NotImplementedException();

	virtual void raise(void) {
		throw *this;
	}

	virtual const String& message() {
		if (m_messageString == 0) {
			m_messageString = new String(NotImplementedErrorMessage);
		}
		return *m_messageString;
	}

	/*int
	 * Object inheritance
	 * */
	virtual const char* objectID() const {
		return "7197f47abaa7eb7d2516b293fa9df5b6";
	}

	virtual const String& toString() {
		if (m_toString == 0)
			m_toString = new String("NotImplemented");
		return *m_toString;
	}

private:
	static const char* NotImplementedErrorMessage;
	String* m_messageString;
};

}

}

}

}

}

}

using pt::efacec::rto::runtime::exception::system::NotImplementedException;

#endif /* NOTIMPLEMENTEDEXCEPTION_H_ */
