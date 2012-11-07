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
//
// C++ Interface: UUID
//
// Description:
//
// Adapted from ace/UUID.h by Rolando Martins <rolando.martins@gmail.com>
//
//
#ifndef COM_EFACEC_RTPM_COMMONUUID_H
#define COM_EFACEC_RTPM_COMMONUUID_H
//#include <config.h>
#include /**/ "ace/pre.h"

#include <ace/os_include/os_stddef.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_stdio.h>
#include <ace/Basic_Types.h>
#include <ace/CORBA_macros.h>
#include <ace/SString.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Synch.h>
//#include <ace/Refcounted_Auto_Ptr.h>
#include <ace/Bound_Ptr.h>
#include <euryale/Euryale_export.h>
//#include <exception>
#include <euryale/common/exception/Exception.h>
#include <ace/Containers_T.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/*
 *	External openssl library include 
 */
#include <openssl/rand.h>

//#include <euryale/EuryaleConf.h>
#include <ace/Functor_T.h>
#include <euryale/Conf.h>
#include <euryale/Types.h>

#include <euryale/serialization/Serializable.h>

/*namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

namespace uuid {*/

/**
 @author Rolando Martins <rolando.martins@efacec.pt>
 */

/*class UUID_Exception: public ::std::exception
 {
 public:
 UUID_Exception()
 {}
 const char* what() const throw()
 {
 return "Invalid UUID!";
 }
 };


 template <typename UUIDGenerator>
 class UUID_Traits
 {
 public:
 typedef UUIDGenerator UUID_type;
 UUID_Traits(ACE_UINT32 keySize) : lumbCount(keySize/sizeof(UUIDGenerator))
 {
 if (keySize%sizeof(UUIDGenerator) != 0) {
 throw UUID_Exception();
 }
 }

 const ACE_UINT32 size() const
 {
 return sizeof(UUIDGenerator);
 }
 const ACE_UINT32 lumbCount;

 };





 #if defined (__ia64) || defined(__alpha) || defined (__x86_64__)
 #define RTPM_DEFAULT_UUID_TYPE ACE_UINT64
 #else
 #define RTPM_DEFAULT_UUID_TYPE ACE_UINT32
 #endif

 #define DEFAULT_RTPM_UUID_TRAIT ACE_UINT64

 template <typename UUIDGenerator>
 class UUID
 {
 public:
 typedef UUID_Traits<UUIDGenerator> traits_type;
 typedef typename UUID_Traits<UUIDGenerator>::UUID_type UUID_type ;
 static const ACE_UINT16 DEFAULT_KEY_SIZE = 16; //128 bits

 UUID(ACE_UINT16 keySize = DEFAULT_KEY_SIZE, const char* key = NULL);

 ~UUID();

 const char* getKey();
 const ACE_UINT32 getKeySize() const;

 int test();

 const traits_type m_trait;

 //alloc hook
 ACE_ALLOC_HOOK_DECLARE;
 protected:

 void parseKey(const char* key);

 //string form
 char* m_key;
 //numeric form
 UUID_type *m_lumb;





 };*/

class Euryale_Export UUID_Exception : public Exception {
public:

    enum ExceptionMessages {
        INVALID_UUID,
        ZERO_LENGTH_KEY,
        EXCEPTION_MESSAGE_SIZE
    };

    static const char* EXCEPTION_MESSAGE[];

    UUID_Exception(ExceptionMessages type) {
        m_messageString = 0;
        m_type = type;
    }

    virtual ~UUID_Exception() {
        if (m_messageString != 0)
            delete m_messageString;
    }

    virtual void raise(void) {
        throw *this;
    }

    virtual const String& message() {
        if (m_messageString == 0) {
            m_messageString = new String(EXCEPTION_MESSAGE[m_type]);
        }
        return *m_messageString;
    }

    /*int
     * Object inheritance
     * */
    virtual const char* objectID() const {
        return "565C0BA07A658904FFFCF4313467DD48";
    }

    virtual const String& toString() {
        if (m_toString == 0)
            m_toString = new String("UUID_Exception");
        return *m_toString;
    }

protected:
    String *m_messageString;
    ExceptionMessages m_type;

};

class UUID_Data;
class UUID;

typedef ACE_Strong_Bound_Ptr<UUID, ACE_SYNCH_MUTEX> UUIDPtr;

class Euryale_Export UUID : public Serializable {
public:
    static String INVALID_ID_STR;
    static UUIDPtr INVALID_ID;

    enum {
        UUID_PEER = 0x01,
        UUID_GROUP = 0x02,
        UUID_OBJECT = 0x03
    };

    typedef int UUID_Type;
    typedef unsigned long UUID_Flags;

    enum {
        /// Don't delete the data on exit since we don't own it.
        DONT_DELETE = 0x01,
        /// user defined flags start here
        USER_FLAGS = 0x1000
    };

    static const ACE_UINT16 DEFAULT_KEY_SIZE = 16; //128 bits

    UUID();

    UUID(InputStream& inputStream);

    UUID(ACE_Allocator *allocator);

    UUID(const char* keyString, ACE_Allocator *allocator = 0);

    UUID(size_t size,
            UUID_Type type = UUID_OBJECT,
            const char *data = 0,
            ACE_Allocator *allocator_strategy = 0,
            ACE_Lock *locking_strategy = 0,
            ACE_Allocator *data_block_allocator = 0,
            ACE_Allocator *message_block_allocator = 0);

    UUID(const UUID &mb) THROW(UUID_Exception&);

    static UUID* generateUUID(ACE_UINT16 keySize = DEFAULT_KEY_SIZE);
    static UUID* generateUUIDWithPrefix(const char* prefix,
            ACE_UINT16 keySize = DEFAULT_KEY_SIZE);

    virtual ~UUID();

    const char* getKey();
    const ACE_UINT32 getKeySize() const;
    virtual String& toString();

    bool isEqual(const UUID& uuid) const;
    bool isEqual(const UUIDPtr& uuid) const;

    void operator=(const UUID& uuid);
    void operator=(const char* key);
    bool operator==(const UUID& uuid) const;
    bool operator!=(UUID& uuid);
    int compare(UUID &mb);

    /*u_long hash(){
        return toString().hash();
    }*/

    virtual void serialize(OutputStream& outputStream) THROW(SerializationException&);
    virtual void deserialize(InputStream& inputStream) THROW(SerializationException&);

    int init(size_t size,
            UUID_Type type = UUID_OBJECT,
            const char *data = 0,
            ACE_Allocator *allocator_strategy = 0,
            ACE_Lock *locking_strategy = 0,
            ACE_Allocator *data_block_allocator = 0,
            ACE_Allocator *uuid_allocator = 0);

    virtual UUID* duplicate(void) const;

    //alloc hook
    ACE_ALLOC_HOOK_DECLARE;


protected:
    UUID(size_t size,
            UUID_Type type,
            const char *data,
            ACE_Allocator *allocator_strategy,
            ACE_Lock *locking_strategy,
            UUID_Flags flags,
            UUID_Data *db,
            ACE_Allocator *data_block_allocator,
            ACE_Allocator *message_block_allocator);


    virtual void parse(const char* keyString) throw (UUID_Exception&);
    //void init(ACE_UINT16 keySize, const char* key) THROW (UUID_Exception&);
    int init_i(size_t size,
            UUID_Type msg_type,
            const char *msg_data,
            ACE_Allocator *allocator_strategy,
            ACE_Lock *locking_strategy,
            UUID_Flags flags,
            UUID_Data *db,
            ACE_Allocator *uuid_data_allocator,
            ACE_Allocator *uuid_allocator);

    void setUUIDData(UUID_Data* db);
    UUID_Data* getUUIDData() const;

    //string rep
    String m_keyString;

    UUID *next_;

    /// Pointer to previous packet in the list.
    UUID *prev_;

    UUID_Data* m_db;
    /// Misc flags (e.g., DONT_DELETE and USER_FLAGS).
    UUID::UUID_Flags m_flags;

    ACE_Allocator *m_allocator;

    //ACE_Allocator *m_allocator_strategy;
    ACE_Allocator *m_uuid_data_allocator;
    ACE_Allocator *m_uuid_allocator;

};

class Euryale_Export UUID_Data {
public:
    UUID_Data();

    UUID_Data(size_t size,
            UUID::UUID_Type type,
            const char *data,
            ACE_Allocator *allocator_strategy,
            ACE_Lock *locking_strategy,
            UUID::UUID_Flags flags,
            ACE_Allocator *uuid_data_allocator);

    virtual ~UUID_Data();

    UUID::UUID_Type type(void) const;

    /// Set type of the packet.
    void type(UUID::UUID_Type type);

    /// Get packet data pointer
    char *base(void) const;

    /// Set packet data pointer (doesn't reallocate).
    void base(char *data,
            size_t size,
            UUID::UUID_Flags mflags = UUID::DONT_DELETE);

    /// Get the total amount of allotted space in the packet.  The amount of
    /// allotted space may be less than allocated space.
    size_t size(void) const;

    /// Set the total amount of space in the packet.  Returns 0 if
    /// successful, else -1.
    int size(size_t length);

    /// Get the total amount of allocated space.
    size_t capacity(void) const;

    /**
     * Return an exact "deep copy" of the packet, i.e., create fresh
     * new copies of all the Data_Blocks and continuations.
     * Notice that Data_Blocks can act as "Prototypes", i.e. derived
     * classes can override this method and create instances of
     * themselves.
     */
    virtual UUID_Data *clone(UUID::UUID_Flags mask = 0) const;

    /**
     * As clone above, but it does not copy the contents of the buffer,
     * i.e., create a new Data_Block of the same dynamic type, with the
     * same allocator, locking_strategy, and with the same amount of
     * storage available (if @a max_size is zero) but the buffer is unitialized.
     * If @a max_size is specified other than zero, it will be used when
     * creating the new data block.
     */
    virtual UUID_Data *clone_nocopy(UUID::UUID_Flags mask = 0,
            size_t max_size = 0) const;

    /// Return a "shallow" copy that increments our reference count by 1.
    UUID_Data *duplicate(void);

    /**
     * Decrease the shared reference count by 1.  If the reference count
     * is > 0 then return this; else if reference count == 0 then delete
     * @c this and @a mb and return 0.  Behavior is undefined if reference
     * count < 0.
     */
    UUID_Data *release(ACE_Lock *lock = 0);

    // = Packet flag accessors and mutators.
    /// Bitwise-or the <more_flags> into the existing packet flags and
    /// return the new value.
    UUID::UUID_Flags set_flags(UUID::UUID_Flags more_flags);

    /// Clear the packet flag bits specified in <less_flags> and return
    /// the new value.
    UUID::UUID_Flags clr_flags(UUID::UUID_Flags less_flags);

    /// Get the current packet flags.
    UUID::UUID_Flags flags(void) const;

    /// Obtain the allocator strategy.
    ACE_Allocator *allocator_strategy(void) const;

    // = The locking strategy prevents race conditions.
    /// Get the locking strategy.
    ACE_Lock *locking_strategy(void);

    /// Set a new locking strategy and return the hold one.
    ACE_Lock *locking_strategy(ACE_Lock *);

    /// Dump the state of an object.
    void dump(void) const;

    /// Get the current reference count.
    int reference_count(void) const;

    /// Get the allocator used to create this object
    ACE_Allocator *data_block_allocator(void) const;

    //void operator=(const UUID_Data& uuidData);
    bool operator==(const UUID_Data& uuidData) const;
    bool operator!=(const UUID_Data& uuidData);    

protected:

    /// Internal release implementation
    virtual UUID_Data *release_i(void);

    /// Internal get the current reference count.
    int reference_count_i(void) const;

    /**
     * Decrease the reference count, but don't delete the object.
     * Returns 0 if the object should be removed.
     * If <lock> is equal to the locking strategy then we assume that
     * the lock is beign held by the current thread; this is used to
     * release all the data blocks in a chain while holding a single
     * lock.
     */
    friend class UUID;
    UUID_Data *release_no_delete(ACE_Lock *lock);

    /// Type of packet.
    UUID::UUID_Type type_;

    /// Current size of packet block.
    size_t cur_size_;

    /// Total size of buffer.
    size_t max_size_;

    /// Misc flags (e.g., DONT_DELETE and USER_FLAGS).
    UUID::UUID_Flags flags_;

    /// Pointer To beginning of packet payload.
    char *base_;

    // = Strategies.
    /**
     * Pointer to the allocator defined for this ACE_Data_Block.  Note
     * that this pointer is shared by all owners of this
     * ACE_Data_Block.
     */
    ACE_Allocator *allocator_strategy_;

    /**
     * Pointer to the locking strategy defined for this
     * ACE_Data_Block.  This is used to protect regions of code that
     * access shared ACE_Data_Block state.  Note that this lock is
     * shared by all owners of the ACE_Data_Block's data.
     */
    ACE_Lock *locking_strategy_;

    /**
     * Reference count for this ACE_Data_Block, which is used to avoid
     * deep copies (i.e., <clone>).  Note that this pointer value is
     * shared by all owners of the <Data_Block>'s data, i.e., all the
     * ACE_Message_Blocks.
     */
    int reference_count_;

    /// The allocator use to destroy ourselves.
    ACE_Allocator *uuid_data_allocator_;

};

//typedef ACE_Strong_Bound_Ptr<UUID,ACE_SYNCH_MUTEX> UUIDPtr;
typedef ACE_Double_Linked_List<UUID> UUIDList;

class Euryale_Export UUIDKeyType {
public:

    bool operator==(const UUIDKeyType& b) const {
        return (m_uuid == b.m_uuid);
    }

    UUIDKeyType() {
        ACE_TRACE("UUIDKeyType()");

    }

    UUIDKeyType(UUID& uuid) :
    m_uuid(uuid) {
        ACE_TRACE("UUIDKeyType(UUID&)");
    }

    ~UUIDKeyType() {
        ACE_TRACE("~UUIDKeyType()");
        //delete m_uuid;
    }

    UUIDKeyType(const UUIDKeyType& kt) {
        ACE_TRACE("const UUIDKeyType& kt");
        this->m_uuid = kt.m_uuid;
    }

    UUIDKeyType(UUIDKeyType& kt) {
        ACE_TRACE("UUIDKeyType& kt");
        this->m_uuid = kt.m_uuid;
    }

    UUID& getUUID() {
        return m_uuid;
    }
private:
    UUID m_uuid;
};

class Euryale_Export UUIDPtrKeyType {
public:

    bool operator==(const UUIDPtrKeyType& b) const {
        return (*m_uuid == *(b.m_uuid));
    }

    UUIDPtrKeyType() {
        ACE_TRACE("UUIDPtrKeyType()");

    }

    UUIDPtrKeyType(UUIDPtr& uuid) :
    m_uuid(uuid) {
        ACE_TRACE("UUIDPtrKeyType(UUID&)");
    }

    ~UUIDPtrKeyType() {
        ACE_TRACE("~UUIDPtrKeyType()");
        //delete m_uuid;
    }

    UUIDPtrKeyType(const UUIDPtrKeyType& kt) {
        ACE_TRACE("const UUIDPtrKeyType& kt");
        this->m_uuid = kt.m_uuid;
    }

    UUIDPtrKeyType(UUIDPtrKeyType& kt) {
        ACE_TRACE("UUIDPtrKeyType& kt");

        this->m_uuid = kt.m_uuid;
    }

    UUIDPtr& getUUIDPtr() {
        return m_uuid;
    }

    UUID& getUUID() {
        return (*m_uuid);
    }
private:
    UUIDPtr m_uuid;
};

/*}
}
}
}
}*/

#if defined (__ACE_INLINE__)
#include <euryale/common/uuid/UUID.inl>
#endif /* __ACE_INLINE__ */

/*using pt::efacec::rto::runtime::uuid::UUID;
using pt::efacec::rto::runtime::uuid::UUID_Exception;
using pt::efacec::rto::runtime::uuid::UUID_Data;
using pt::efacec::rto::runtime::uuid::UUIDPtr;
using pt::efacec::rto::runtime::uuid::UUIDList;*/

// Specialize the hash functor.

template<>
class ACE_Hash<UUID> {
public:

    u_long operator() (const UUID& kt) const {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)hash=%d\n",((pt::efacec::rto::runtime::uuid::UUIDKeyType)kt)->getSourceUUID()->toString().hash());
        return ((UUID) kt).toString().hash();
        //return 0;
    }

    /*u_long operator() (const pt::efacec::rto::runtime::uuid::UUIDKeyType& kt) const
     {
     return operator() ((pt::efacec::rto::runtime::uuid::UUIDKeyType)kt);
     }*/
};
// Specialize the equality functor.

template<>
class ACE_Equal_To<UUID> {
public:

    int operator() (const UUID& kt1,
            const UUID& kt2) const {
        return ((kt1) == (kt2)); //?0:-1;
    }
};

/*****************************************************************************************************/

// Specialize the hash functor.

template<>
class ACE_Hash<UUIDPtrKeyType> {
public:

    u_long operator() (const UUIDPtrKeyType& kt) const {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)hash=%d\n",((pt::efacec::rto::runtime::uuid::UUIDKeyType)kt)->getSourceUUID()->toString().hash());
        return ((UUIDPtrKeyType) kt).getUUID().toString().hash();
        //return 0;
    }

    /*u_long operator() (const pt::efacec::rto::runtime::uuid::UUIDKeyType& kt) const
     {
     return operator() ((pt::efacec::rto::runtime::uuid::UUIDKeyType)kt);
     }*/
};
// Specialize the equality functor.

template<>
class ACE_Equal_To<UUIDPtrKeyType> {
public:

    int operator() (const UUIDPtrKeyType& kt1,
            const UUIDPtrKeyType& kt2) const {
        return (((UUIDPtrKeyType&) kt1).getUUID() == ((UUIDPtrKeyType&) kt2).getUUID()); //?0:-1;
    }
};

// Specialize the hash functor.

template<>
class ACE_Hash<UUIDKeyType> {
public:

    u_long operator() (const UUIDKeyType& kt) const {
        return ((UUIDPtrKeyType&) kt).getUUID().toString().hash();
    }

};
// Specialize the equality functor.

template<>
class ACE_Equal_To<UUIDKeyType> {
public:

    int operator() (const UUIDKeyType& kt1,
            const UUIDKeyType& kt2) const {
        return (kt1 == kt2);
    }
};


/****************************/
// Specialize the hash functor.

template<>
//class ACE_Hash<pt::efacec::rto::runtime::uuid::UUIDPtr>
class ACE_Hash<UUIDPtr> {
public:
    //u_long operator() (const pt::efacec::rto::runtime::uuid::UUIDPtr& kt) const

    u_long operator() (const UUIDPtr& kt) const {
        return kt->toString().hash();
    }

};
// Specialize the equality functor.

template<>
class ACE_Equal_To<UUIDPtr> {
public:

    int operator() (const UUIDPtr& kt1,
            const UUIDPtr& kt2) const {
        return (kt1->toString() == kt2->toString());
    }
};



//#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
//#include <UUID.cpp>
//#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

//#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
//#pragma implementation ("UUID.cpp")
//#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */

#include /**/ <ace/post.h>

#endif /* COM_EFACEC_RTPM_COMMONUUID_H */

