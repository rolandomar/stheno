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
// Adapted from ace/UUID.inl by Rolando Martins <rolando.martins@gmail.com>
/*namespace pt
{

namespace efacec
{

namespace rto
{

namespace runtime
{

namespace uuid
{*/

#include "UUID.h"


#if defined(ACE_WIN32)
#define HEX_FORMAT "%08x"
#else
#define HEX_FORMAT "%08x"
#endif

/*ACE_INLINE String& UUID::toString()
{
        if (m_keyString != 0) {
                return *m_keyString;
        }
        int stringSize = m_db->size()<<1;
                const int finalBufferSize = stringSize+1;
        char* dataBuffer = new char[finalBufferSize];
        char* str = dataBuffer;
        str[stringSize] = 0;
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)%d\n",stringSize);
        int* data = (int*)m_db->base();

        int keySize = m_db->size() / 4;
        for (int i=0; i < keySize; i++) {
                        ACE_OS::sACE_DEBUG((LM_DEBUG,ACE_TEXT(str,HEX_FORMAT,data[i]);
                str = str+8;
        }
        m_keyString = new String(dataBuffer);
                delete dataBuffer;
        return *m_keyString;
}*/

ACE_INLINE const char* UUID::getKey() {
    //return m_key;
    if (m_db != 0) {
        return m_db->base();
    } else {
        return 0;
    }
}

ACE_INLINE const ACE_UINT32 UUID::getKeySize() const {
    return m_db->size();
}

/*ACE_INLINE void UUID::parse(const char* keyString) THROW (UUID_Exception&)
{
                ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)UUID:parse()\n");
                if(keyString == 0){
			
                        ACE_THROW(UUID_Exception(UUID_Exception::INVALID_UUID));
                        return;
                }
		
        char* str = (char*)keyString;
        int* data = (int*)m_db->base();
        int keyStringLen = ACE_OS::strlen(keyString);
        if(keyStringLen == 0 || keyStringLen % 2 != 0){
                ACE_THROW(UUID_Exception(UUID_Exception::INVALID_UUID));
        }
        int keySize = m_db->size() / 4;        
        for (int i=0; i < keySize; i++) {
                if (sscanf(str,"%8x",&data[i]) != 1) {
                        ACE_THROW(UUID_Exception(UUID_Exception::INVALID_UUID));
                }
                str = str+8;
        }
}*/

/*ACE_INLINE void UUID::init(ACE_UINT16 keySize, const char* key)  THROW (UUID_Exception&)
{
        
        if (keySize%2 != 0) {
                ACE_THROW(UUID_Exception());
        }

        m_keySize = keySize;
        ACE_NEW_THROW_EX (m_key, char[m_keySize] ,UUID_Exception());
        if (key != 0) {        
                ACE_OS::memcpy(m_key,key,keySize);
        } else {        
                ACE_OS::memset ((void *) m_key, 0, keySize);                
        }
}*/

/*ACE_INLINE void UUID::operator=(const UUID& uuid)// throw (UUID_Exception&)
{
                ACE_TRACE("void UUID::operator= (const UUID& uuid)");
        if (m_keyString != 0) {
                delete m_keyString;
                m_keyString = 0;
        }
        if (m_key != 0) {
                delete [] m_key;
                m_key = 0;
        }
        
        this->m_allocator = uuid.m_allocator;
        
        init(uuid.m_keySize,uuid.m_key);
}*/

/*ACE_INLINE bool UUID::operator==(const UUID& uuid) const
{
        ACE_TRACE("UUID::operator==(const UUID& uuid)");
        if (m_keySize != uuid.m_keySize) {
                return false;
        }
        return ACE_OS::memcmp(m_key,uuid.m_key,m_keySize)==0?true:false;
}*/

ACE_INLINE bool UUID::operator==(const UUID& uuid) const {
    ACE_TRACE("UUID::operator==(const UUID& uuid)");
    //return (*this->m_db) == (*uuid.m_db);
    if(ACE_OS::memcmp(this->m_db->base(),uuid.m_db->base(),this->getKeySize()) == 0){
        return true;
    }
    return false;
}

ACE_INLINE bool UUID::isEqual(const UUID& uuid) const{
    //return (*this->m_db) == (*uuid.m_db);
    if(ACE_OS::memcmp(this->m_db->base(),uuid.m_db->base(),this->getKeySize()) == 0){
        return true;
    }
    return false;
}

ACE_INLINE bool UUID::isEqual(const UUIDPtr& uuid) const{
    return isEqual(*(uuid.get()));
    //return false;
}

ACE_INLINE bool UUID::operator!=(UUID& uuid) {
    ACE_TRACE("UUID::operator!= (UUID& uuid)");
    return !operator==(uuid);
}






/*}
}
}
}
}
 */

