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
 * C++ Implementation: UUID
 *
 * Description: 
 *
 *
 * Adapted from ace/UUID.cpp by Rolando Martins <rolando.martins@gmail.com>
 *
 */
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef COM_EFACEC_RTM_COMMONUUID_CPP
#define COM_EFACEC_RTM_COMMONUUID_CPP

#include <euryale/common/uuid/UUID.h>
#include <ace/OS_Memory.h>

//#if !defined (ACE_LACKS_PRAGMA_ONCE)
//#pragma once
//#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (__ACE_INLINE__)
#include <euryale/common/uuid/UUID.inl>
#endif /* __ACE_INLINE__ */

#include <euryale/common/exception/system/NullPointerException.h>
#include <euryale/common/io/StreamSize.h>

/*namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

namespace uuid {*/

ACE_ALLOC_HOOK_DEFINE(UUID);

const char* UUID_Exception::EXCEPTION_MESSAGE[] = {"Invalid UUID",
    "Zero length key"};

String UUID::INVALID_ID_STR("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
UUIDPtr UUID::INVALID_ID(new UUID(INVALID_ID_STR.c_str()));

UUID::UUID(size_t size,
        UUID_Type type,
        const char *data,
        ACE_Allocator *allocator_strategy,
        ACE_Lock *locking_strategy,
        ACE_Allocator *data_block_allocator,
        ACE_Allocator *message_block_allocator) :
//m_keyString(0), m_db(0), m_flags(0), m_allocator(0), m_uuid_data_allocator(
m_db(0), m_flags(0), m_allocator(0), m_uuid_data_allocator(
0), m_uuid_allocator(0) {
    ACE_TRACE("UUID::UUID");

    if (this->init_i(size, type, data, allocator_strategy, locking_strategy,
            data ? UUID::DONT_DELETE : 0, 0, // data block
            data_block_allocator, message_block_allocator) == -1)
        ACE_ERROR((LM_ERROR,
            ACE_TEXT("(%T)UUID")));
}

UUID::UUID() :
//m_keyString(0), m_db(0), m_flags(0), m_allocator(0),
m_db(0), m_flags(0), m_allocator(0),
m_uuid_data_allocator(0), m_uuid_allocator(0) {
    ACE_TRACE("UUID()");
    this->m_allocator = ACE_Allocator::instance();
}

UUID::UUID(InputStream& inputStream):
                m_db(0), m_flags(0), m_allocator(0),
    m_uuid_data_allocator(0), m_uuid_allocator(0) {
    ACE_TRACE("UUID(inputStream)");
    this->m_allocator = ACE_Allocator::instance();
        deserialize(inputStream);
    }

UUID::UUID(ACE_Allocator *allocator) :
//m_keyString(0), m_db(0), m_flags(0), m_allocator(0),
m_db(0), m_flags(0), m_allocator(0),
m_uuid_data_allocator(0), m_uuid_allocator(0) {
    ACE_TRACE("UUID(ACE_Allocator *allocator)");
    if (allocator == 0) {
        this->m_allocator = ACE_Allocator::instance();
    }
}

UUID* UUID::generateUUIDWithPrefix(const char* prefix, ACE_UINT16 keySize) {
    int prefixStringLen = ACE_OS::strlen(prefix);
    UUID* uuid = new UUID(keySize, 0);

    char* str = (char*) prefix;
    int* data = (int*) uuid->getKey();
    int prefixParseLen = prefixStringLen / 8;
    int prefixKeyLen = prefixParseLen * 4;
    int leftKeySize = DEFAULT_KEY_SIZE - prefixKeyLen;
    ;
    for (int i = 0; i < prefixParseLen; i++) {
        if (sscanf(str, "%8x", &data[i]) != 1) {
            throw(UUID_Exception(UUID_Exception::INVALID_UUID));
        }
        str = str + 8;
    }
    if (RAND_bytes((unsigned char*) (uuid->getKey() + prefixKeyLen),
            leftKeySize) != 1) {
        throw UUID_Exception(UUID_Exception::INVALID_UUID);
    }
    return uuid;
}

UUID::UUID(const char* keyString, ACE_Allocator *allocator) :
//m_keyString(0), m_db(0), m_flags(0), m_allocator(0), m_uuid_data_allocator(
m_db(0), m_flags(0), m_allocator(0), m_uuid_data_allocator(
0), m_uuid_allocator(0) {
    ACE_TRACE("UUID::UUID(const char* keyString, ACE_Allocator *allocator)");
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)UUID::UUID(const char* keyString, ACE_Allocator *allocator)\n");
    if (allocator == 0) {
        this->m_allocator = ACE_Allocator::instance();
    }
    //m_keyString = 0;
    //UUID(DEFAULT_KEY_SIZE,key);
    if (keyString == 0) {
        throw UUID_Exception(UUID_Exception::INVALID_UUID);
    }

    ssize_t keyLen = ACE_OS::strlen(keyString);
    if (keyLen == 0) {
        throw UUID_Exception(UUID_Exception::INVALID_UUID);
    }
    //m_keySize = keyLen>>1;
    if (this->init_i(keyLen >> 1, // size
            UUID_OBJECT, // type
            0, // data
            0, // allocator
            0, // locking strategy
            0, // flags
            0, // data block
            0, m_allocator) == -1)
        ACE_ERROR((LM_ERROR,
            ACE_TEXT("(%T)UUID")));
    parse(keyString);
    //ACE_NEW_THROW_EX (m_key, char[m_keySize] ,UUID_Exception());
    //parse(keyString);
}

int UUID::compare(UUID &mb){
    String& s1 = toString();
    String& s2 = mb.toString();
    int ret = ACE_OS::strcmp(s1.c_str(),s2.c_str());
    return ret;
}

void UUID::operator=(const char* key) {
    /*Release pending data*/
    if (ACE_BIT_DISABLED(this->m_flags,
            UUID::DONT_DELETE) && this->getKey()) {
        this->m_db->release();
    }
    /*if (this->m_keyString != 0) {
        delete this->m_keyString;
    }*/
    this->prev_ = 0;
    this->next_ = 0;
    /* Released data */

    if (this->m_allocator == 0) {
        this->m_allocator = ACE_Allocator::instance();
    }

    if (key == NULL) {
        //throw UUID_Exception(UUID_Exception::NULL_POINTER_EXCEPTION);
        throw NullPointerException();
    }

    ssize_t keyLen = ACE_OS::strlen(key);
    if (keyLen == 0) {
        throw UUID_Exception(UUID_Exception::ZERO_LENGTH_KEY);
    }

    if (this->init_i(keyLen >> 1, // size
            UUID_OBJECT, // type
            0, // data
            0, // allocator
            0, // locking strategy
            0, // flags
            0, // data block
            0, m_allocator) == -1)
        ACE_ERROR((LM_ERROR,
            ACE_TEXT("(%T)UUID")));
    parse(key);
}

void UUID::operator=(const UUID& mb) {
    /*Release pending data*/
    if (ACE_BIT_DISABLED(this->m_flags,
            UUID::DONT_DELETE) && this->getKey()) {
        this->m_db->release();
    }

    /*if (m_keyString != 0) {
        delete m_keyString;
    }*/
    this->prev_ = 0;
    this->next_ = 0;
    /* Released data */

    if (ACE_BIT_DISABLED(mb.m_flags,
            UUID::DONT_DELETE)) {
        if (this->init_i((size_t) mb.getKeySize(), // size
                UUID::UUID_OBJECT, // type
                0, // data
                0, // allocator_strategy
                0, // locking strategy,
                UUID::DONT_DELETE,
                mb.getUUIDData()->duplicate(), // data block
                mb.getUUIDData()->data_block_allocator(),
                mb.m_uuid_allocator)
                == -1)
            ACE_ERROR((LM_ERROR,
                ACE_TEXT("(%T)UUID")));
    } else {
        if (this->init_i((size_t) mb.getKeySize(), // size
                UUID::UUID_OBJECT, // type
                0, // data
                0, // allocator
                0, // locking strategy,
                0,
                //mb.getUUIDData()->clone_nocopy(),// data block
                mb.getUUIDData()->clone(), // data block
                mb.getUUIDData()->data_block_allocator(), mb.m_uuid_allocator)
                == -1)
            ACE_ERROR((LM_ERROR,
                ACE_TEXT("(%T)UUID")));

    }
}


/*
 * 
 * Binary format key -> Dont parse
 * */

/*UUID::UUID(ACE_UINT16 keySize,
 const char* key,
 UUID_Flags flags,
 ACE_Allocator *allocator) :
 m_keyString(0), m_db(0), m_flags(0), m_allocator(0),
 m_uuid_data_allocator(0), m_uuid_allocator(0) {
 if (allocator == 0) {
 this->m_allocator = ACE_Allocator::instance();
 }
 if (this->init((size_t)keySize, // size
 UUID_OBJECT, // type
 0, // data
 0, // allocator
 0, // locking strategy
 0, m_allocator) == -1)
 ACE_ERROR ((LM_ERROR,
 ACE_TEXT ("UUID")));
 
 }*/

UUID::UUID(const UUID &mb)THROW(UUID_Exception&) :
//m_keyString(0), m_db(0), m_flags(0), m_allocator(0),
m_db(0), m_flags(0), m_allocator(0),
m_uuid_data_allocator(0), m_uuid_allocator(0) {
    ACE_TRACE("UUID::UUID(const UUID&)");
    if (ACE_BIT_DISABLED(mb.m_flags,
            UUID::DONT_DELETE)) {
        if (this->init_i((size_t) mb.getKeySize(), // size
                UUID::UUID_OBJECT, // type
                0, // data
                0, // allocator_strategy
                0, // locking strategy,
                UUID::DONT_DELETE,
                mb.getUUIDData()->duplicate(), // data block
                mb.getUUIDData()->data_block_allocator(),
                mb.m_uuid_allocator)
                == -1)
            ACE_ERROR((LM_ERROR,
                ACE_TEXT("(%T)UUID")));
    } else {
        if (this->init_i((size_t) mb.getKeySize(), // size
                UUID::UUID_OBJECT, // type
                0, // data
                0, // allocator
                0, // locking strategy,
                0,
                //mb.getUUIDData()->clone_nocopy(),// data block
                mb.getUUIDData()->clone(), // data block
                mb.getUUIDData()->data_block_allocator(), mb.m_uuid_allocator)
                == -1)
            ACE_ERROR((LM_ERROR,
                ACE_TEXT("(%T)UUID")));

    }
}

UUID_Data* UUID::getUUIDData() const {
    ACE_TRACE("UUID::getUUIDData");
    return m_db;

}

UUID::~UUID() {
    /*if (m_key != 0) {
     delete [] m_key;
     }*/
    ACE_TRACE("UUID::~UUID");

    if (ACE_BIT_DISABLED(this->m_flags,
            UUID::DONT_DELETE) && this->getKey())
        this->m_db->release();

    /*if (m_keyString != 0) {
        delete m_keyString;
    }*/

    this->prev_ = 0;
    this->next_ = 0;
}

/*ACE_INLINE*/ String& UUID::toString() {
    /*if (m_keyString != 0) {
        return *m_keyString;
    }*/
    int stringSize = m_db->size() << 1;
    const int finalBufferSize = stringSize + 1;
    //char* dataBuffer = new char[finalBufferSize];
    //char* str = dataBuffer;
    //str[stringSize] = 0;
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)%d\n",stringSize);
    unsigned char* data = (unsigned char*) m_db->base();
    unsigned char dst[finalBufferSize];
    unsigned char* s = dst;
    dst[stringSize] = 0;
    int keySize = m_db->size() / 1;
    for (int i = 0; i < keySize; i++) {
      unsigned int num = data[i]; 
      ACE_OS::sprintf((char*)s, /*HEX_FORMAT*/"%02X", num);
        s = s + 2;
    }
    m_keyString = (char*)dst;
    return m_keyString;
    //m_keyString = new String(dataBuffer);    
    //delete dataBuffer;
    //return *m_keyString;
}

/*ACE_INLINE*/ void UUID::parse(const char* keyString) THROW(UUID_Exception&) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)UUID:parse(%s)\n",keyString);
    if (keyString == 0) {

        throw(UUID_Exception(UUID_Exception::INVALID_UUID));
        return;
    }

    char* str = (char*) keyString;
    unsigned char* data = (unsigned char*) m_db->base();
    int keyStringLen = ACE_OS::strlen(keyString);
    if (keyStringLen == 0 || keyStringLen % 2 != 0) {
        throw(UUID_Exception(UUID_Exception::INVALID_UUID));
    }
    int keySize = m_db->size() / 1;
    for (int i = 0; i < keySize; i++) {
      //if (sscanf(str, "%02X", &data[i]) != 1) {
      unsigned int num = 0;
      if (sscanf(str, "%02X", &num) != 1) {
	throw(UUID_Exception(UUID_Exception::INVALID_UUID));
      }
      data[i]=num;
      str = str + 2;
    }
}

UUID* UUID::generateUUID(ACE_UINT16 keySize) {
    UUID* uuid = new UUID(keySize, 0);
    /*
     if (RAND_bytes((unsigned char*)uuid->m_key, keySize) != 1) {
     throw UUID_Exception();
     }*/
    if (RAND_bytes((unsigned char*) uuid->getKey(), keySize) != 1) {
        throw UUID_Exception(UUID_Exception::INVALID_UUID);
    }

    return uuid;
}

void UUID::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    if (getKey() == 0) {
        throw (new SerializationException(SerializationException::NOT_INITIALIZED));
    }
    //outputStream.write_size(m_keySize);
    //outputStream.write_octet_array((Byte*)m_key, m_keySize);
    //outputStream.write_size(this->getKeySize());
    outputStream.write_ulong(this->getKeySize());
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: UUID::serialize   size=%d\n"),getKeySize()));
    outputStream.write_octet_array((Byte*)this->getKey(), this->getKeySize());
}

void UUID::deserialize(InputStream& inputStream)
throw (SerializationException&) {
    //UShort size = 0;
    /*RTo_ULong size = 0;
     //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ret=%d\n", inputStream.read_ushort(size));
     inputStream.read_size(size);
     Byte* buf = new Byte[size];
     inputStream.read_octet_array(buf, size);
     //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SIZE=%d\n buf[0]=%d", size,buf[0]);
	 
     //init(size, (const char*)buf);
     this->init((size_t)size,UUID::UUID_OBJECT,(const char*)buf,0,0,0,0);
     //delete [] buf;*/
    UInt size = 0;
    inputStream.read_ulong(size);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: UUID::deserialize   size=%d\n"),size));
    this->init((size_t) size, UUID::UUID_OBJECT, 0, 0, 0, 0, 0);
    inputStream.read_octet_array((Byte*)this->getKey(), size);
}

UUID::UUID(size_t size,
        UUID_Type type,
        const char *data,
        ACE_Allocator *allocator_strategy,
        ACE_Lock *locking_strategy,
        UUID_Flags flags,
        UUID_Data *db,
        ACE_Allocator *data_block_allocator,
        ACE_Allocator *message_block_allocator) :
//m_keyString(0), next_(0), prev_(0), m_db(0), m_flags(0), m_allocator(0), m_uuid_data_allocator(0), m_uuid_allocator(0)
next_(0), prev_(0), m_db(0), m_flags(0), m_allocator(0), m_uuid_data_allocator(0), m_uuid_allocator(0) {
    ACE_TRACE("UUID::UUID");

    if (this->init_i(size,
            type,
            data,
            allocator_strategy,
            locking_strategy,
            flags,
            db,
            data_block_allocator,
            message_block_allocator) == -1)
        ACE_ERROR((LM_ERROR,
            ACE_TEXT("(%T)UUID")));
}

UUID *
UUID::duplicate(void) const {
    ACE_TRACE("UUID::duplicate");

    UUID *nb = 0;

    // Create a new <ACE_Message_Block> that contains unique copies of
    // the packet block fields, but a reference counted duplicate of
    // the <ACE_Data_Block>.

    // If there is no allocator, use the standard new and delete calls.
    if (this->m_uuid_allocator == 0)
        ACE_NEW_RETURN(nb,
            UUID(0, // size
            m_db->type(), // type
            0, // data
            0, // allocator
            0, // locking strategy
            0, // flags
            this->getUUIDData()->duplicate(),
            this->getUUIDData()->data_block_allocator(),
            this->m_uuid_allocator),
            0);
    else // Otherwise, use the message_block_allocator passed in.
        ACE_NEW_MALLOC_RETURN(nb,
            static_cast<UUID*> (
            m_uuid_allocator->malloc(sizeof (UUID))),
            UUID(0, // size
            m_db->type(), // type
            0, // data
            0, // allocator
            0, // locking strategy
            0, // flags
            this->getUUIDData()->duplicate(),
            this->getUUIDData()->data_block_allocator(),
            this->m_uuid_allocator),
            0);

    return nb;
}

int UUID::init(size_t size,
        UUID_Type type,
        const char *data,
        ACE_Allocator *allocator_strategy,
        ACE_Lock *locking_strategy,
        ACE_Allocator *data_block_allocator,
        ACE_Allocator *uuid_allocator) {
    return this->init_i(size, type, data, allocator_strategy, locking_strategy,
            data ? UUID::DONT_DELETE : 0, 0, data_block_allocator,
            uuid_allocator);
}

int UUID::init_i(size_t size,
        UUID_Type msg_type,
        const char *msg_data,
        ACE_Allocator *allocator_strategy,
        ACE_Lock *locking_strategy,
        UUID_Flags flags,
        UUID_Data *db,
        ACE_Allocator *data_block_allocator,
        ACE_Allocator *uuid_allocator) {
    ACE_TRACE("UUID::init_i");
    //ACE_FUNCTION_TIMEPROBE (ACE_MESSAGE_BLOCK_INIT_I_ENTER);

    this->next_ = 0;
    this->prev_ = 0;

    this->m_uuid_allocator = uuid_allocator;
    /*if(m_uuid_allocator == 0){
     ACE_ALLOCATOR_RETURN (m_uuid_allocator,
     ACE_Allocator::instance (),
     -1);
     }*/

    if (this->m_db != 0) {
        this->m_db->release();
        this->m_db = 0;
    }

    if (db == 0) {
        if (data_block_allocator == 0)
            ACE_ALLOCATOR_RETURN(data_block_allocator,
                ACE_Allocator::instance(),
                -1);

        //ACE_TIMEPROBE (ACE_MESSAGE_BLOCK_INIT_I_DB_ALLOC);

        // Allocate the <ACE_Data_Block> portion, which is reference
        // counted.
        ACE_NEW_MALLOC_RETURN(db,
                static_cast<UUID_Data *> (
                data_block_allocator->malloc(sizeof (UUID_Data))),
                UUID_Data(size,
                msg_type,
                msg_data, //this is the data holding facility
                allocator_strategy,
                locking_strategy,
                flags,
                data_block_allocator),
                -1);
        //ACE_TIMEPROBE (ACE_MESSAGE_BLOCK_INIT_I_DB_CTOR);

        // Packet block initialization may fail, while the construction
        // succeds.  Since ACE may throw no exceptions, we have to do a
        // separate check and clean up, like this:
        if (db != 0 && db->size() < size) {
            db->UUID_Data::~UUID_Data(); // placement destructor ...
            data_block_allocator->free(db); // free ...
            errno = ENOMEM;
            return -1;
        }
    }

    // Reset the data_block_ pointer.
    this->setUUIDData(db);

    return 0;
}

void UUID::setUUIDData(UUID_Data* db) {
    ACE_TRACE("UUID::setUUIDData");
    if (ACE_BIT_DISABLED(this->m_flags,
            UUID::DONT_DELETE)
            && this->m_db != 0) {
        this->m_db->release();
    }

    this->m_db = db;

}

UUID_Data::~UUID_Data(void) {
    // Sanity check...
    ACE_TRACE("UUID_Data::~UUID_Data");
    ACE_ASSERT(this->reference_count_ <= 1);

    // Just to be safe...
    this->reference_count_ = 0;

    if (ACE_BIT_DISABLED(this->flags_,
            UUID::DONT_DELETE)) {
        this->allocator_strategy_->free((void *) this->base_);
        this->base_ = 0;
    }
}

UUID_Data::UUID_Data(void) :
type_(UUID::UUID_OBJECT), cur_size_(0), max_size_(0),
flags_(UUID::DONT_DELETE), base_(0), allocator_strategy_(0),
locking_strategy_(0), reference_count_(1), uuid_data_allocator_(0) {
    ACE_TRACE("UUID_Data::UUID_Data");
    //ACE_FUNCTION_TIMEPROBE (ACE_DATA_BLOCK_CTOR1_ENTER);

    ACE_ALLOCATOR(this->allocator_strategy_,
            ACE_Allocator::instance());

    ACE_ALLOCATOR(this->uuid_data_allocator_,
            ACE_Allocator::instance());
}

UUID_Data::UUID_Data(size_t size,
        UUID::UUID_Type type,
        const char *data,
        ACE_Allocator *allocator_strategy,
        ACE_Lock *locking_strategy,
        UUID::UUID_Flags flags,
        ACE_Allocator *uuid_data_allocator) :
type_(type),
cur_size_(0), // Reset later if memory alloc'd ok
max_size_(0), flags_(flags), base_(const_cast<char *> (data)),
allocator_strategy_(allocator_strategy),
locking_strategy_(locking_strategy), reference_count_(1),
uuid_data_allocator_(uuid_data_allocator) {
    ACE_TRACE("UUID_Data::UUID_Data");
    //ACE_FUNCTION_TIMEPROBE(ACE_DATA_BLOCK_CTOR2_ENTER);

    // If the user didn't pass one in, let's use the
    // <ACE_Allocator::instance>.
    if (this->allocator_strategy_ == 0)
        ACE_ALLOCATOR(this->allocator_strategy_,
            ACE_Allocator::instance());

    if (this->uuid_data_allocator_ == 0)
        ACE_ALLOCATOR(this->uuid_data_allocator_,
            ACE_Allocator::instance());

    if (data == 0) {
        ACE_ALLOCATOR(this->base_,
                (char *) this->allocator_strategy_->malloc(size));
        //#if defined (ACE_INITIALIZE_MEMORY_BEFORE_USE)
        (void) ACE_OS::memset(this->base_,
                0, //'\0',
                size);
        //#endif /* ACE_INITIALIZE_MEMORY_BEFORE_USE */
    }

    // ACE_ALLOCATOR returns on alloc failure but we cant throw, so setting
    // the size to 0 (i.e. "bad bit") ...
    if (this->base_ == 0) {
        size = 0;
    }

    // The memory is legit, whether passed in or allocated, so set
    // the size.
    this->cur_size_ = this->max_size_ = size;

}

UUID_Data * UUID_Data::release_i(void) {
    ACE_TRACE("UUID_Data::release_i");

    ACE_ASSERT(this->reference_count_ > 0);

    UUID_Data *result = 0;

    // decrement reference count
    --this->reference_count_;

    if (this->reference_count_ == 0) {
        // this will cause deletion of this
        result = 0;
    } else {
        result = this;
    }

    return result;
}

UUID_Data * UUID_Data::release_no_delete(ACE_Lock *lock) {
    ACE_TRACE("UUID_Data::release_no_delete");

    UUID_Data *result = 0;
    ACE_Lock *lock_to_be_used = 0;

    // Check if we were passed in a lock
    if (lock != 0) {
        // Make sure that the lock passed in and our lock are the same
        if (lock == this->locking_strategy_)
            // In this case no locking is required.
            lock_to_be_used = 0;

            // The lock passed in does not match our lock
        else
            // Lock to be used is our lock
            lock_to_be_used = this->locking_strategy_;
    }// This is the case when no lock was passed in
    else
        // Lock to be used is our lock
        lock_to_be_used = this->locking_strategy_;

    // If there's a locking strategy then we need to acquire the lock
    // before decrementing the count.
    if (lock_to_be_used != 0) {
        ACE_GUARD_RETURN(ACE_Lock, ace_mon, *lock_to_be_used, 0)
                ;

        result = this->release_i();
    } else
        result = this->release_i();

    return result;
}

UUID_Data * UUID_Data::release(ACE_Lock *lock) {
    ACE_TRACE("UUID_Data::release");

    ACE_Allocator *allocator = this->uuid_data_allocator_;

    UUID_Data *result = this->release_no_delete(lock);

    // We must delete this outside the scope of the locking_strategy_
    // since otherwise we'd be trying to "release" through a deleted
    // pointer!
    if (result == 0)
        ACE_DES_FREE(this,
            allocator->free,
            UUID_Data);
    return result;
}

UUID_Data * UUID_Data::duplicate(void) {
    ACE_TRACE("UUID_Data::duplicate");

    // Create a new <ACE_Message_Block>, but share the <base_> pointer
    // data (i.e., don't copy that).
    if (this->locking_strategy_) {
        // We need to acquire the lock before incrementing the count.
        ACE_GUARD_RETURN(ACE_Lock, ace_mon, *this->locking_strategy_, 0)
                ;
        ++this->reference_count_;
    } else
        ++this->reference_count_;

    return this;
}

void UUID_Data::dump(void) const {
#if defined (ACE_HAS_DUMP)
    ACE_TRACE("ACE_Data_Block::dump");
    ACE_DEBUG((LM_DEBUG, ACE_BEGIN_DUMP, this));
    ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T)-----( Data Block )-----\n")
            ACE_TEXT("(%T)type_ = %d\n")
            ACE_TEXT("(%T)cur_size_ = %u\n")
            ACE_TEXT("(%T)max_size_ = %u\n")
            ACE_TEXT("(%T)flags_ = %u\n")
            ACE_TEXT("(%T)base_ = %u\n")
            ACE_TEXT("(%T)locking_strategy_ = %u\n")
            ACE_TEXT("(%T)reference_count_ = %u\n")
            ACE_TEXT("(%T)---------------------------\n"),
            this->type_,
            this->cur_size_,
            this->max_size_,
            this->flags_,
            this->base_,
            this->locking_strategy_,
            this->reference_count_));
    this->allocator_strategy_->dump();
    ACE_DEBUG((LM_DEBUG, ACE_END_DUMP));
#endif /* ACE_HAS_DUMP */
}

int UUID_Data::reference_count(void) const {
    if (this->locking_strategy_) {
        // We need to acquire the lock before retrieving the count
        ACE_GUARD_RETURN(ACE_Lock, ace_mon, *this->locking_strategy_, 0)
                ;

        return this->reference_count_i();
    }

    return this->reference_count_i();
}

int UUID_Data::size(size_t length) {
    ACE_TRACE("UUID::size");

    if (length <= this->max_size_)
        this->cur_size_ = length;
    else {
        // We need to resize!
        char *buf = 0;
        ACE_ALLOCATOR_RETURN(buf,
                (char *) this->allocator_strategy_->malloc(length),
                -1);

        ACE_OS::memcpy(buf, this->base_, this->cur_size_);
        if (ACE_BIT_DISABLED(this->flags_,
                UUID::DONT_DELETE))
            this->allocator_strategy_->free((void *) this->base_);
        else
            // We now assume ownership.
            ACE_CLR_BITS(this->flags_,
                UUID::DONT_DELETE);
        this->max_size_ = length;
        this->cur_size_ = length;
        this->base_ = buf;
    }
    return 0;
}

void UUID_Data::base(char *msg_data,
        size_t msg_length,
        UUID::UUID_Flags msg_flags) {
    if (ACE_BIT_DISABLED(this->flags_,
            UUID::DONT_DELETE))
        this->allocator_strategy_->free(this->base_);

    this->max_size_ = msg_length;
    this->cur_size_ = msg_length;
    this->base_ = msg_data;
    this->flags_ = msg_flags;
}

UUID_Data * UUID_Data::clone(UUID::UUID_Flags mask) const {
    ACE_TRACE("UUID_Data::clone");

    UUID_Data *nb = this->clone_nocopy(mask);

    // Copy all of the payload memory into the new object. The new block
    // was allocated with max_size_ (and, thus, it's cur_size_ is the same
    // as max_size_). Maintain the same "has been written" boundary in the
    // new block by only copying cur_size_ bytes.
    if (nb != 0) {
        ACE_OS::memcpy(nb->base_, this->base_, this->cur_size_);
    }

    return nb;
}

UUID_Data * UUID_Data::clone_nocopy(UUID::UUID_Flags mask, size_t max_size) const {
    //ACE_FUNCTION_TIMEPROBE(ACE_DATA_BLOCK_CLONE_ENTER);

    ACE_TRACE("UUID_Data::clone_nocopy");

    // You always want to clear this one to prevent memory leaks but you
    // might add some others later.
    const UUID::UUID_Flags always_clear = UUID::DONT_DELETE;

    const size_t newsize = max_size == 0 ? this->max_size_ : max_size;

    UUID_Data *nb = 0;

    ACE_NEW_MALLOC_RETURN(nb,
            static_cast<UUID_Data*> (
            this->uuid_data_allocator_->malloc(sizeof (UUID_Data))),
            UUID_Data(newsize, // size
            this->type_, // type
            0, // data
            this->allocator_strategy_, // allocator
            this->locking_strategy_, // locking strategy
            this->flags_, // flags
            this->uuid_data_allocator_),
            0);

    // Packet block initialization may fail while the construction
    // succeds.  Since as a matter of policy, ACE may throw no
    // exceptions, we have to do a separate check like this.
    if (nb != 0 && nb->size() < newsize) {
        nb->UUID_Data::~UUID_Data(); // placement destructor ...
        this->uuid_data_allocator_->free(nb); // free ...
        errno = ENOMEM;
        return 0;
    }

    // Set new flags minus the mask...
    nb->clr_flags(mask | always_clear);
    return nb;
}

ACE_INLINE int
UUID_Data::reference_count_i(void) const {
    return reference_count_;
}

/*ACE_INLINE int
 UUID_Data::reference_count (void) const
 {
 return data_block () ? data_block ()->reference_count () : 0;
 }*/

/*ACE_INLINE*/ char *
UUID_Data::base(void) const {
    ACE_TRACE("UUID_Data::base");
    return this->base_;
}

/*ACE_INLINE*/ size_t
UUID_Data::size(void) const {
    ACE_TRACE("UUID_Data::size");
    return this->cur_size_;
}

ACE_INLINE size_t
UUID_Data::capacity(void) const {
    ACE_TRACE("UUID_Data::capacity");
    return this->max_size_;
}

ACE_INLINE UUID::UUID_Flags
UUID_Data::set_flags(UUID::UUID_Flags more_flags) {
    ACE_TRACE("ACE_Data_Block::set_flags");
    // Later we might mask more_glags so that user can't change internal
    // ones: more_flags &= ~(USER_FLAGS -1).
    return ACE_SET_BITS(this->flags_, more_flags);
}

ACE_INLINE UUID::UUID_Flags
UUID_Data::clr_flags(UUID::UUID_Flags less_flags) {
    ACE_TRACE("ACE_Data_Block::clr_flags");
    // Later we might mask more_flags so that user can't change internal
    // ones: less_flags &= ~(USER_FLAGS -1).
    return ACE_CLR_BITS(this->flags_, less_flags);
}

ACE_INLINE UUID::UUID_Flags
UUID_Data::flags(void) const {
    ACE_TRACE("ACE_Data_Block::flags");
    return this->flags_;
}

ACE_INLINE ACE_Allocator*
UUID_Data::data_block_allocator(void) const {
    ACE_TRACE("ACE_Data_Block::data_block_allocator");
    return this->uuid_data_allocator_;
}

ACE_INLINE UUID::UUID_Type
UUID_Data::type(void) const {
    ACE_TRACE("UUID_Data::msg_type");
    return this->type_;
}

ACE_INLINE void
UUID_Data::type(UUID::UUID_Type t) {
    ACE_TRACE("UUID_Data::msg_type");
    this->type_ = t;
}

ACE_INLINE ACE_Allocator *
UUID_Data::allocator_strategy(void) const {
    ACE_TRACE("ACE_Data_Block::allocator_strategy");
    return this->allocator_strategy_;
}

ACE_INLINE ACE_Lock *
UUID_Data::locking_strategy(void) {
    ACE_TRACE("ACE_Data_Block::locking_strategy");
    return this->locking_strategy_;
}

ACE_INLINE ACE_Lock *
UUID_Data::locking_strategy(ACE_Lock *nls) {
    ACE_TRACE("ACE_Data_Block::locking_strategy");
    ACE_Lock *ols = this->locking_strategy_;

    this->locking_strategy_ = nls;
    return ols;
}

/*void UUID_Data::operator=(const UUID_Data& uuidData)
 {
 ACE_TRACE ("UUID_Data::operator=");

 // You always want to clear this one to prevent memory leaks but you
 // might add some others later.
 const UUID::UUID_Flags always_clear = UUID::DONT_DELETE;

 const size_t newsize = max_size == 0 ? this->max_size_ : max_size;

 UUID_Data *nb = 0;

 ACE_NEW_MALLOC_RETURN (nb,
 static_cast<UUID_Data*> (
 this->uuid_data_allocator_->malloc (sizeof (UUID_Data))),
 UUID_Data (newsize, // size
 this->type_, // type
 0, // data
 this->allocator_strategy_, // allocator
 this->locking_strategy_, // locking strategy
 this->flags_, // flags
 this->uuid_data_allocator_),
 0);

 // Packet block initialization may fail while the construction
 // succeds.  Since as a matter of policy, ACE may throw no
 // exceptions, we have to do a separate check like this.
 if (nb != 0 && nb->size() < newsize) {
 nb->UUID_Data::~UUID_Data(); // placement destructor ...
 this->uuid_data_allocator_->free(nb); // free ...
 errno = ENOMEM;
 return 0;	
 }

 // Set new flags minus the mask...
 nb->clr_flags(uuidData.flags() | always_clear);
 return nb;
 }*/

bool UUID_Data::operator==(const UUID_Data& uuidData) const {
    if (this->size() != uuidData.size()) {
        return false;
    } else {
        return ACE_OS::memcmp(this->base(), uuidData.base(), this->size()) == 0 ? true : false;
    }
}

bool UUID_Data::operator!=(const UUID_Data& uuidData) {
    return !operator==(uuidData);
}

/*}

}

}

}

}*/
#endif

