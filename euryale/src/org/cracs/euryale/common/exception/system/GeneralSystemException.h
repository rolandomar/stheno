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
 * GuardException.h
 *
 *  Created on: Jul 31, 2008
 *      Author: rmartins
 */

#ifndef GUARDEXCEPTION_H_
#define GUARDEXCEPTION_H_

#include <euryale/common/exception/Exception.h>


class GeneralSystemException : public Exception {
public:
	GeneralSystemException();
	virtual ~GeneralSystemException();

	virtual void raise(void) {
		throw *this;
	}

	virtual const String& message() {
		if (m_messageString == 0) {
			m_messageString = new String(GuardExceptionErrorMessage);
		}
		return *m_messageString;
	}


	/*int
	 * Object inheritance
	 * */
	virtual const char* objectID() const {
		return "aafd9e7850fbff351273b1687d34b5bab";
	}

	virtual const String& toString() {
		if (m_toString == 0)
			m_toString = new String("GuardException");
		return *m_toString;
	}

private:
	static const char* GuardExceptionErrorMessage;
	String* m_messageString;
};

//using pt::efacec::rto::runtime::exception::system::GeneralSystemException;

#endif /* GUARDEXCEPTION_H_ */
