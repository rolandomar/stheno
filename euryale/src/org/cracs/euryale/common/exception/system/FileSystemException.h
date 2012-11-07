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
 * File:   FileSystemException.h
 * Author: rmartins
 *
 * Created on November 30, 2009, 11:07 AM
 */

#ifndef _FILESYSTEMEXCEPTION_H
#define	_FILESYSTEMEXCEPTION_H
#include <euryale/common/exception/Exception.h>

namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

namespace exception {

namespace system {

class FileSystemException : public Exception {
public:
	FileSystemException();
	virtual ~FileSystemException();

	virtual void raise(void) {
		throw *this;
	}

	virtual const String& message() {
		if (m_messageString == 0) {
			m_messageString = new String(FileSystemExceptionMessage);
		}
		return *m_messageString;
	}


	/*int
	 * Object inheritance
	 * */
	virtual const char* objectID() const {
		return "f529a7830fbff657573b1687d34b5be0";
	}

	virtual const String& toString() {
		if (m_toString == 0)
			m_toString = new String("FileSystemException");
		return *m_toString;
	}

private:
	static const char* FileSystemExceptionMessage;
	String* m_messageString;
};


}

}

}

}

}

}

using pt::efacec::rto::runtime::exception::system::FileSystemException;

#endif /* _FILESYSTEMEXCEPTION_H */
