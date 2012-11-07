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
 * File:   PacketException.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 27, 2010, 5:00 PM
 */

#ifndef PACKETEXCEPTION_H
#define	PACKETEXCEPTION_H

#include <euryale/common/exception/Exception.h>

class PacketException : public Exception {
public:
	PacketException(const char* exception);
	virtual ~PacketException();

	virtual void raise(void) {
		throw *this;
	}

	virtual const String& message() {
            return *m_messageString;
	}


	virtual const char* objectID() const {
		return "af34bbf0fadf657573b1687d34b5129";
	}

	virtual const String& toString() {
            return this->message();
	}

private:
	String* m_messageString;
};

#endif	/* PACKETEXCEPTION_H */

