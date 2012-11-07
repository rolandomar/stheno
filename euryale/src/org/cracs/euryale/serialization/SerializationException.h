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
#ifndef SERIALIZATIONEXCEPTION_H_
#define SERIALIZATIONEXCEPTION_H_

//#include <euryale/common/exception/Exception.h>
#include <euryale/common/Object.h>
#include <ace/SString.h>


class SerializationException : public Object{//Exception {
public:
	enum SerializationError {
		GENERAL_ERROR = 0x00,
                BAD_STREAM,
		MEMORY_ERROR,
		STREAM_LOCALTION_NOTFOUND,
                NOT_INITIALIZED,
                NULL_OBJECT
	};
	static const char* ERROR_MESSAGES[];

	SerializationException(SerializationError errorCode):
		m_strMessage(ERROR_MESSAGES[errorCode]), m_strEx(objectID()){
		
	}
	
	
	virtual void raise(void){
		//throw *this;
	}

	virtual ~SerializationException();

	virtual const String& message() {
		return m_strMessage;
	}

	/*
	 * Object inheritance
	 * */
	virtual const char* objectID() const{
		return "e326140fe51234524a4081b07d7e04dd";
	}

	virtual const String& toString() {
		return m_strEx;
	}

protected:
	String m_strEx;
	String m_strMessage;

	
	
	//SerializationException(){}
	
};

//using pt::efacec::rto::serialization::SerializationException;

#endif /*SERIALIZATIONEXCEPTION_H_*/
