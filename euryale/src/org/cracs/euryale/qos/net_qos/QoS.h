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
#ifndef QOS_H_
#define QOS_H_

#include <ace/Basic_Types.h>
#include <ace/Containers_T.h>

#include <euryale/serialization/Serializable.h>


class QoS : public Serializable {
public:
    
    
	QoS(Byte type,ACE_UINT16 priority) :
		m_type(type),m_priority(priority) {
	}
	QoS();
	virtual ~QoS();

	ACE_INLINE void serialize(OutputStream& outputStream) THROW (SerializationException&) {                
		outputStream.write_ushort(m_priority);
	}

	ACE_INLINE void deserialize(InputStream& inputStream) THROW (SerializationException&) {
		inputStream.read_ushort(m_priority);
	}

	const ACE_UINT16 getPriority()
	{
		return m_priority;
	}
protected:
    Byte m_type;
	ACE_UINT16 m_priority;

};


#endif /*QOS_H_*/
