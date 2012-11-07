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
#include <ace/Message_Block.h>
#include <euryale/common/io/OutputStream.h>

#include "Endpoint.h"

/*namespace pt {

namespace efacec {

namespace rto {

namespace runtime {*/

ACE_INLINE void Endpoint::serialize(OutputStream& outputStream) THROW(SerializationException&)
{
    outputStream.write_octet(m_type);
    outputStream.write_string(m_host);
    outputStream.write_ushort(m_port);
    m_qos->serialize(outputStream);
}

ACE_INLINE void Endpoint::deserialize(InputStream& inputStream) THROW(SerializationException&)
{
    inputStream.read_octet(m_type);
    inputStream.read_string(m_host);
    inputStream.read_ushort(m_port);
    m_qos->deserialize(inputStream);
}

/*ACE_INLINE UInt Endpoint::getSerializationSize(Alignment* align) throw (SerializationException&)
{   if(m_qos == 0){
        throw new SerializationException(SerializationException::GENERAL_ERROR);
    }
    if (align == 0 || align->getType() == Alignment::ZERO_ALIGNMENT) {
        return BYTE_SIZE + STRING_SIZE(m_host)+SHORT_SIZE+m_qos->getSerializationSize(align);
    }
    throw new SerializationException(SerializationException::GENERAL_ERROR);
}*/

/*}

}

}

}*/
