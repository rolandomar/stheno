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
 * File:   ByteInputStream.h
 * Author: rmartins
 *
 * Created on January 12, 2010, 12:47 PM
 */

#ifndef _BYTEINPUTSTREAM_H
#define	_BYTEINPUTSTREAM_H

#include <euryale/Euryale_export.h>
#include <euryale/Types.h>
#include <euryale/common/io/ByteStream.h>
#include <euryale/common/io/InputStream.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <ace/SStringfwd.h>
#include <ace/Message_Block.h>
#include <ace/OS_Memory.h>

class CharCodesetTranslator;
class WCharCodesetTranslator;
class ByteOutputStream;
class Euryale_Export ByteInputStream: public InputStream{
public:
    /// The translator need privileged access to efficiently demarshal
    /// arrays and the such
    friend class CharCodesetTranslator;
    friend class WCharCodesetTranslator;

     

    
    ByteInputStream(const char *buf, size_t bufsiz,
            int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    /// Create an empty input stream. The caller is responsible for
    /// putting the right data and providing the right alignment.
    ByteInputStream(size_t bufsiz, int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    
    ByteInputStream(const ACE_Message_Block *data,
            int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION,
            ACE_Lock* lock = 0);

    /// Create an input stream from an ACE_Data_Block. The <flag>
    /// indicates whether the <data> can be deleted by the CDR stream
    /// or not
    ByteInputStream(ACE_Data_Block *data,
            ACE_Message_Block::Message_Flags flag = 0,
            int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    /// Create an input stream from an ACE_Data_Block. It also sets the
    /// read and write pointers at the desired positions. This would be
    /// helpful if the applications desires to create a new CDR stream
    /// from a semi-processed datablock.
    ByteInputStream(ACE_Data_Block *data, ACE_Message_Block::Message_Flags flag,
            size_t read_pointer_position, size_t write_pointer_position,
            int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

   
    ByteInputStream(const ByteInputStream& rhs);

    ByteInputStream& operator=(const ByteInputStream& rhs);

    /// When interpreting indirected TypeCodes it is useful to make a
    /// "copy" of the stream starting in the new position.
    ByteInputStream(const ByteInputStream& rhs, size_t size, Int offset);

    /// This creates an encapsulated stream, the first byte must be (per
    /// the spec) the byte order of the encapsulation.
    ByteInputStream(const ByteInputStream& rhs, size_t size);

    /// Create an input CDR from an output CDR.
    ByteInputStream(const ByteOutputStream& rhs,
            ACE_Allocator* buffer_allocator = 0,
            ACE_Allocator* data_block_allocator = 0,
            ACE_Allocator* message_block_allocator = 0);

    /// Helper class to transfer the contents from one input CDR to
    /// another without requiring any extra memory allocations, data
    /// copies or too many temporaries.

    struct Euryale_Export Transfer_Contents {
        Transfer_Contents(ByteInputStream & rhs);

        ByteInputStream &rhs_;
    };
    /// Transfer the contents from <rhs> to a new CDR
    ByteInputStream(Transfer_Contents rhs);

    /// Destructor
    ~ByteInputStream(void);

    /// Disambiguate overloading when extracting octets, chars,
    /// booleans, and bounded strings
    //@{ @name Helper classes

    struct Euryale_Export to_boolean {
        explicit to_boolean(Boolean & b);
        Boolean &ref_;
    };

    struct Euryale_Export to_char {
        explicit to_char(Char & c);
        Char &ref_;
    };

    struct Euryale_Export to_wchar {
        explicit to_wchar(WChar & wc);
        WChar &ref_;
    };

    struct Euryale_Export to_octet {
        explicit to_octet(Octet & o);
        Octet &ref_;
    };

    struct Euryale_Export to_string {
        /**
         * @deprecated The constructor taking a non-const string is now
         *             deprecated (C++ mapping 00-01-02), but we keep it
         *             around for backward compatibility.
         */
        to_string(Char *&s, UInt b);
        to_string(const Char *&s, UInt b);
        const Char *&val_;
        UInt bound_;
    };

    struct Euryale_Export to_wstring {
        /// The constructor taking a non-const wstring is
        /// now deprecated (C++ mapping 00-01-02), but we
        /// keep it around for backward compatibility.
        to_wstring(WChar *&ws, UInt b);
        to_wstring(const WChar *&ws, UInt b);
        const WChar *&val_;
        UInt bound_;
    };
    //@}

    /**
     * Return @c false on failure and @c true on success.
     */
    //@{ @name Read basic IDL types
    Boolean read_boolean(Boolean& x);
    Boolean read_char(Char &x);
    Boolean read_wchar(WChar& x);
    Boolean read_octet(Octet& x);
    Boolean read_short(Short &x);
    Boolean read_ushort(UShort &x);
    Boolean read_long(Int &x);
    Boolean read_ulong(UInt &x);
    Boolean read_longlong(LongLong& x);
    Boolean read_ulonglong(ULongLong& x);
    Boolean read_float(Float &x);
    Boolean read_double(Double &x);
    Boolean read_longdouble(LongDouble &x);

    Boolean read_string(Char *&x);
    Boolean read_string(ACE_CString &x);
    Boolean read_wstring(WChar*& x);
    //@}

    /**
     * The buffer @a x must be large enough to contain @a length
     * elements.
     * Return @c false on failure and @c true on success.
     */
    //@{ @name Read basic IDL types arrays
    Boolean read_boolean_array(Boolean* x, UInt length);
    Boolean read_char_array(Char *x, UInt length);
    Boolean read_wchar_array(WChar* x, UInt length);
    Boolean read_octet_array(Octet* x, UInt length);
    Boolean read_short_array(Short *x, UInt length);
    Boolean read_ushort_array(UShort *x, UInt length);
    Boolean read_long_array(Int *x, UInt length);
    Boolean read_ulong_array(UInt *x, UInt length);
    Boolean read_longlong_array(LongLong* x, UInt length);
    Boolean read_ulonglong_array(ULong* x, UInt length);
    Boolean read_float_array(Float *x, UInt length);
    Boolean read_double_array(Double *x, UInt length);
    Boolean read_longdouble_array(LongDouble* x, UInt length);
    //@}

    /**
     * Return @c false on failure and @c true on success.
     */
    //@{ @name Skip elements
    Boolean skip_boolean(void);
    Boolean skip_char(void);
    Boolean skip_wchar(void);
    Boolean skip_octet(void);
    Boolean skip_short(void);
    Boolean skip_ushort(void);
    Boolean skip_long(void);
    Boolean skip_ulong(void);
    Boolean skip_longlong(void);
    Boolean skip_ulonglong(void);
    Boolean skip_float(void);
    Boolean skip_double(void);
    Boolean skip_longdouble(void);
    //@}

    /****
     *
     * TODO: work in progress
     *
     */
    virtual const char* objectID()
    {
        return "be9533f07177ac7d899f63c2262a08be";
    }
    /**
     * The next field must be a string, this method skips it. It is
     * useful in parsing a TypeCode.
     * @return @c false on failure and @c true on success.
     */
    Boolean skip_wstring(void);
    Boolean skip_string(void);

    /// Skip @a n bytes in the CDR stream.
    /**
     * @return @c false on failure and @c true on success.
     */
    Boolean skip_bytes(size_t n);

    /// returns @c false if a problem has been detected.
    bool good_bit(void) const;

    /**
     * @return The start of the packet block chain for this CDR
     *         stream.
     *
     * @note In the current implementation the chain has length 1, but
     *       we are planning to change that.
     */
    const ACE_Message_Block* start(void) const;

    // = The following functions are useful to read the contents of the
    //   CDR stream from a socket or file.

    /**
     * Grow the internal buffer, reset @c rd_ptr to the first byte in
     * the new buffer that is properly aligned, and set @c wr_ptr to @c
     * rd_ptr @c + @c newsize
     */
    int grow(size_t newsize);

    /**
     * After reading and partially parsing the contents the user can
     * detect a change in the byte order, this method will let him/her
     * change it.
     */
    void reset_byte_order(int byte_order);

    /// Re-initialize the CDR stream, copying the contents of the chain
    /// of message_blocks starting from @a data.
    void reset(const ACE_Message_Block *data, int byte_order);

    /// Steal the contents from the current CDR.
    ACE_Message_Block *steal_contents(void);

    /// Steal the contents of @a cdr and make a shallow copy into this
    /// stream.
    void steal_from(ByteInputStream &cdr);

    /// Exchange data blocks with the caller of this method. The read
    /// and write pointers are also exchanged.
    /**
     * @note We now do only with the start_ packet block.
     */
    void exchange_data_blocks(ByteInputStream &cdr);

    /// Copy the data portion from the @c cdr to this cdr and return the
    /// data content (ie. the ACE_Data_Block) from this CDR to the
    /// caller.
    /**
     * @note The caller is responsible for managing the memory of the
     *       returned ACE_Data_Block.
     */
    ACE_Data_Block* clone_from(ByteInputStream &cdr);

    /// Re-initialize the CDR stream, forgetting about the old contents
    /// of the stream and allocating a new buffer (from the allocators).
    void reset_contents(void);

    /// Returns the current position for the @c rd_ptr.
    char* rd_ptr(void);

    /// Returns the current position for the @c wr_ptr.
    char* wr_ptr(void);

    /// Return how many bytes are left in the stream.
    size_t length(void) const;

    /**
     * Utility function to allow the user more flexibility.
     * Skips up to the nearest @a alignment-byte boundary.
     * Argument MUST be a power of 2.
     *
     * @return 0 on success and -1 on failure.
     */
    int align_read_ptr(size_t alignment);

    /// If @c true then this stream is writing in non-native byte order.
    /// This is only meaningful if ACE_ENABLE_SWAP_ON_WRITE is defined.
    bool do_byte_swap(void) const;

    /// If @c do_byte_swap() returns @c false, this returns
    /// RDR_BYTE_ORDER else it returns !RDR_BYTE_ORDER.
    int byte_order(void) const;

    /// Access the codeset translators. They can be nil!
    CharCodesetTranslator *char_translator(void) const;
    WCharCodesetTranslator *wchar_translator(void) const;

    /// Set the codeset translators.
    void char_translator(CharCodesetTranslator *);
    void wchar_translator(WCharCodesetTranslator *);

    /**
     * Returns (in @a buf) the next position in the buffer aligned to
     * @a size.  It advances the Message_Block @c rd_ptr past the data
     * (i.e., @c buf @c + @c size).  Sets the good_bit to @c false and
     * returns a -1 on failure.
     */
    int adjust(size_t size, char *&buf);

    /// As above, but now the size and alignment requirements may be
    /// different.
    int adjust(size_t size, size_t align, char *&buf);

    /// Set the underlying GIOP version..
    void set_version(Octet major, Octet minor);

    /// Set the underlying GIOP version..
    void get_version(Octet &major, Octet &minor);

protected:

    /// The start of the chain of packet blocks, even though in the
    /// current version the chain always has length 1.
    ACE_Message_Block start_;

    /// The CDR stream byte order does not match the one on the machine,
    /// swapping is needed while reading.
    bool do_byte_swap_;

    /// set to @c false when an error occurs.
    bool good_bit_;

    /// The GIOP versions for this stream
    Octet major_version_;
    Octet minor_version_;

    /// If not nil, invoke for translation of character and string data.
    CharCodesetTranslator *char_translator_;
    WCharCodesetTranslator *wchar_translator_;

private:

    Boolean read_1(Octet *x);
    Boolean read_2(UShort *x);
    Boolean read_4(UInt *x);
    Boolean read_8(ULongLong *x);
    Boolean read_16(LongDouble *x);

    // Several types can be read using the same routines, since TAO
    // tries to use native types with known size for each CORBA type.
    // We could use void* or char* to make the interface more
    // consistent, but using native types let us exploit the strict
    // alignment requirements of CDR streams and implement the
    // operations using asignment.

    /**
     * Read an array of @a length elements, each of @a size bytes and the
     * start aligned at a multiple of <align>. The elements are assumed
     * to be packed with the right alignment restrictions.  It is mostly
     * designed for buffers of the basic types.
     *
     * This operation uses <memcpy>; as explained above it is expected
     * that using assignment is faster that <memcpy> for one element,
     * but for several elements <memcpy> should be more efficient, it
     * could be interesting to find the break even point and optimize
     * for that case, but that would be too platform dependent.
     */
    Boolean read_array(void* x, size_t size, size_t align,
            UInt length);

    /**
     * On those occasions when the native codeset for wchar is smaller than
     * the size of a wchar_t, such as using UTF-16 with a 4-byte wchar_t, a
     * special form of reading the array is needed. Actually, this should be
     * a default translator.
     */
    Boolean read_wchar_array_i(WChar * x, UInt length);

    /// Move the rd_ptr ahead by <offset> bytes.
    void rd_ptr(size_t offset);

    /// Points to the continuation field of the current packet block.
    char* end(void);
};


#endif	/* _BYTEINPUTSTREAM_H */

