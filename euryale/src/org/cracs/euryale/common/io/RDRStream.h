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
#ifndef RDR_H_
#define RDR_H_

// -*- C++ -*-

//=============================================================================
/**
 *  @file   RDRStream.h
 *
 *  Adapted from CDR_Stream.h by Rolando Martins <rolando.martins@gmail.com>
 *
 * ACE Common Data Representation (CDR) marshaling and demarshaling
 * classes.
 *
 * This implementation was inspired in the CDR class in SunSoft's
 * IIOP engine, but has a completely different implementation and a
 * different interface too.
 *
 * The current implementation assumes that the host has 1-byte,
 * 2-byte and 4-byte integral types, and that it has single
 * precision and double precision IEEE floats.
 * Those assumptions are pretty good these days, with Crays beign
 * the only known exception.
 *
 * Optimizations
 * -------------
 *  ACE_LACKS_CDR_ALIGNMENT
 *  @author Arvind S. Krishna <arvindk@dre.vanderbilt.edu>
 *
 *  CDR stream ignores alignment when marshaling data. Use this option
 *  only when ACE_DISABLE_SWAP_ON_READ can be enabled. This option requires
 *  ACE CDR engine to do both marshaling and demarshaling.
 *
 *
 *  @author TAO version by Aniruddha Gokhale <gokhale@cs.wustl.edu>
 *  @author Carlos O'Ryan <coryan@cs.wustl.edu>
 *  @author ACE version by Jeff Parsons <parsons@cs.wustl.edu>
 *  @author Istvan Buki <istvan.buki@euronet.be>
 *  @author Codeset translation by Jim Rogers <jrogers@viasoft.com>
 */
//=============================================================================

#include /**/ <ace/pre.h>

#define ACE_LACKS_CDR_ALIGNMENT


#include <euryale/Euryale_export.h>
#include <euryale/Types.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <ace/SStringfwd.h>
#include <ace/Message_Block.h>

#if defined (GEN_OSTREAM_OPS)
#include <ace/streams.h>
#endif /* GEN_OSTREAM_OPS */

// Stuff used by the ACE CDR classes.
#if defined ACE_LITTLE_ENDIAN
#define RDR_BYTE_ORDER 1
// little endian encapsulation byte order has value = 1
#else  /* ! ACE_LITTLE_ENDIAN */
#define RDR_BYTE_ORDER 0
// big endian encapsulation byte order has value = 0
#endif /* ! ACE_LITTLE_ENDIAN */

#define ACE_DISABLE_SWAP_ON_READ

/*ACE_BEGIN_VERSIONED_NAMESPACE_DECL


 ACE_END_VERSIONED_NAMESPACE_DECL*/

class RDR_Char_Codeset_Translator;
class RDR_WChar_Codeset_Translator;

class RDRInputStream;

/**
 * @class RDROutputStream
 *
 * @brief A CDR stream for writing, i.e. for marshalling.
 *
 * This class is based on the the CORBA spec for Java (98-02-29),
 * java class omg.org.CORBA.portable.OutputStream.  It diverts in
 * a few ways:
 * + Operations taking arrays don't have offsets, because in C++
 *   it is easier to describe an array starting from x+offset.
 * + Operations return an error status, because exceptions are
 *   not widely available in C++ (yet).
 */
class Euryale_Export RDROutputStream{
public:
    /**
     * The Codeset translators need access to some private members to
     * efficiently marshal arrays
     * For reading from an output CDR stream.
     */
    friend class RDR_Char_Codeset_Translator;
    friend class RDR_WChar_Codeset_Translator;
    friend class RDRInputStream;

    /// Default constructor, allocates @a size bytes in the internal
    /// buffer, if @a size == 0 it allocates the default size.
    RDROutputStream(size_t size = 0, int byte_order = RDR_BYTE_ORDER,
            ACE_Allocator* buffer_allocator = 0,
            ACE_Allocator* data_block_allocator = 0,
            ACE_Allocator* message_block_allocator = 0,
            size_t memcpy_tradeoff = ACE_DEFAULT_CDR_MEMCPY_TRADEOFF,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    /// Build a CDR stream with an initial buffer, it will *not* remove
    /// <data>, since it did not allocated it.  It's important to be careful
    /// with the alignment of <data>.
    /**
     * Create an output stream from an arbitrary buffer, care must be
     * exercised with alignment, because this contructor will align if
     * needed.  In this case @a data will not point to the start of the
     * output stream. @c begin()->rd_ptr() points to the start of the
     * output stream.  See @c ACE_ptr_align_binary() to properly align a
     * pointer and use MAX_ALIGNMENT for the correct alignment.
     */
    RDROutputStream(char *data, size_t size, int byte_order = RDR_BYTE_ORDER,
            ACE_Allocator* buffer_allocator = 0,
            ACE_Allocator* data_block_allocator = 0,
            ACE_Allocator* message_block_allocator = 0,
            size_t memcpy_tradeoff = ACE_DEFAULT_CDR_MEMCPY_TRADEOFF,
            Octet giop_major_version = RDR_GIOP_MAJOR_VERSION,
            Octet giop_minor_version = RDR_GIOP_MINOR_VERSION);

    /// Build a CDR stream with an initial data block, it will *not* remove
    /// <data_block>, since it did not allocated it.  It's important to be
    // careful with the alignment of <data_block>.
    /**
     * Create an output stream from an arbitrary data block, care must be
     * exercised with alignment, because this contructor will align if
     * needed.  In this case @a data_block will not point to the
     * start of the output stream. begin()->rd_ptr() points to the start
     * off the output stream.  See ACE_ptr_align_binary() to properly align a
     * pointer and use MAX_ALIGNMENT for the correct alignment.
     */
    RDROutputStream(ACE_Data_Block *data_block,
            int byte_order = RDR_BYTE_ORDER,
            ACE_Allocator* message_block_allocator = 0,
            size_t memcpy_tradeoff = ACE_DEFAULT_CDR_MEMCPY_TRADEOFF,
            Octet giop_major_version = RDR_GIOP_MAJOR_VERSION,
            Octet giop_minor_version = RDR_GIOP_MINOR_VERSION);

    /// Build a CDR stream with an initial Message_Block chain, it will
    /// *not* remove @a data, since it did not allocate it.
    RDROutputStream(ACE_Message_Block *data, int byte_order = RDR_BYTE_ORDER,
            size_t memcpy_tradeoff = ACE_DEFAULT_CDR_MEMCPY_TRADEOFF,
            Octet giop_major_version = RDR_GIOP_MAJOR_VERSION,
            Octet giop_minor_version = RDR_GIOP_MINOR_VERSION);

    /// destructor
    ~RDROutputStream(void);

    /**
     * Disambiguate overload when inserting booleans, octets, chars, and
     * bounded strings.
     */
    //@{ @name Helper classes

    struct Euryale_Export from_boolean {
        explicit from_boolean(Boolean b);
        Boolean val_;
    };

    struct Euryale_Export from_octet {
        explicit from_octet(Octet o);
        Octet val_;
    };

    struct Euryale_Export from_char {
        explicit from_char(Char c);
        Char val_;
    };

    struct Euryale_Export from_wchar {
        explicit from_wchar(WChar wc);
        WChar val_;
    };

    struct Euryale_Export from_string {
        from_string(Char* s, UInt b, Boolean nocopy = 0);
        from_string(const Char* s, UInt b, Boolean nocopy = 0);
        Char *val_;
        UInt bound_;
        Boolean nocopy_;
    };

    struct Euryale_Export from_wstring {
        from_wstring(WChar* ws, UInt b, Boolean nocopy = 0);
        from_wstring(const WChar* ws, UInt b,
                Boolean nocopy = 0);
        WChar *val_;
        UInt bound_;
        Boolean nocopy_;
    };
    //@}

    // Return 0 on failure and 1 on success.
    //@{ @name Write operations
    Boolean write_boolean(Boolean x);
    Boolean write_char(Char x);
    Boolean write_wchar(WChar x);
    Boolean write_octet(Octet x);
    Boolean write_short(Short x);
    Boolean write_ushort(UShort x);
    Boolean write_long(Int x);
    Boolean write_ulong(UInt x);
    Boolean write_longlong(const LongLong &x);
    Boolean write_ulonglong(const ULongLong &x);
    Boolean write_float(Float x);
    Boolean write_double(const Double &x);
    Boolean write_longdouble(const LongDouble &x);

    // Overwrite the stream at the specified location that is previously
    // written as a long type placeholder. There is no alignment required
    // since the alignment is done before writing the long type placeholder.
    Boolean replace(Int x, char* loc);

    /// For string we offer methods that accept a precomputed length.
    Boolean write_string(const Char *x);
    Boolean write_string(UInt len, const Char *x);
    Boolean write_string(const ACE_CString &x);
    Boolean write_wstring(const WChar *x);
    Boolean write_wstring(UInt length, const WChar *x);
    //@}

    /// @note the portion written starts at <x> and ends
    ///    at <x + length>.
    /// The length is *NOT* stored into the CDR stream.
    //@{ @name Array write operations
    Boolean write_boolean_array(const Boolean *x, UInt length);
    Boolean write_char_array(const Char *x, UInt length);
    Boolean write_wchar_array(const WChar* x, UInt length);
    Boolean write_octet_array(const Octet* x, UInt length);
    Boolean write_short_array(const Short *x, UInt length);
    Boolean write_ushort_array(const UShort *x, UInt length);
    Boolean write_long_array(const Int *x, UInt length);
    Boolean write_ulong_array(const UInt *x, UInt length);
    Boolean
    write_longlong_array(const LongLong* x, UInt length);
    Boolean write_ulonglong_array(const ULong *x,
            UInt length);
    Boolean write_float_array(const Float *x, UInt length);
    Boolean write_double_array(const Double *x, UInt length);
    Boolean write_longdouble_array(const LongDouble* x,
            UInt length);

    /// Write an octet array contained inside a MB, this can be optimized
    /// to minimize copies.
    Boolean write_octet_array_mb(const ACE_Message_Block* mb);
    //@}

    /**
     * Return 0 on failure and 1 on success.
     */
    //@{ @name Append contents of own CDR stream to another
    Boolean append_boolean(RDRInputStream &);
    Boolean append_char(RDRInputStream &);
    Boolean append_wchar(RDRInputStream &);
    Boolean append_octet(RDRInputStream &);
    Boolean append_short(RDRInputStream &);
    Boolean append_ushort(RDRInputStream &);
    Boolean append_long(RDRInputStream &);
    Boolean append_ulong(RDRInputStream &);
    Boolean append_longlong(RDRInputStream &);
    Boolean append_ulonglong(RDRInputStream &);
    Boolean append_float(RDRInputStream &);
    Boolean append_double(RDRInputStream &);
    Boolean append_longdouble(RDRInputStream &);

    Boolean append_wstring(RDRInputStream &);
    Boolean append_string(RDRInputStream &);
    //@}

    /****
     *
     * TODO: work in progress
     *
     */
    virtual const char* objectID()
    {
        return "bb7fee14489a04b28efb0d35f6cb8f9b";
    }

    /// Returns @c false if an error has ocurred.
    /**
     * @note The only expected error is to run out of memory.
     */
    bool good_bit(void) const;

    /// Reuse the CDR stream to write on the old buffer.
    void reset(void);

    /// Add the length of each packet block in the chain.
    size_t total_length(void) const;

    /**
     * Return the start of the packet block chain for this CDR stream.
     * @note The complete CDR stream is represented by a chain of
     * packet blocks.
     */
    const ACE_Message_Block *begin(void) const;

    /// Return the last packet in the chain that is is use.
    const ACE_Message_Block *end(void) const;

    /// Return the <current_> packet block in chain.
    const ACE_Message_Block *current(void) const;

    /// Replace the packet block chain with a single packet block.
    /**
     * Upon successful completion, there will be a single packet block
     * containing the data from the complete packet block chain.
     *
     * @note The only expected error is to run out of memory.
     */
    int consolidate(void);

    /**
     * Access the underlying buffer (read only).  @note This
     * method only returns a pointer to the first block in the
     * chain.
     */
    const char *buffer(void) const;

    /**
     * Return the size of first packet block in the block chain. @note This
     * method only returns information about the first block in the
     * chain.
     */
    size_t length(void) const;

    /**
     * Utility function to allow the user more flexibility.
     * Pads the stream up to the nearest <alignment>-byte boundary.
     * Argument MUST be a power of 2.
     * Returns 0 on success and -1 on failure.
     */
    int align_write_ptr(size_t alignment);

    /// Access the codeset translators. They can be null!
    RDR_Char_Codeset_Translator *char_translator(void) const;
    RDR_WChar_Codeset_Translator *wchar_translator(void) const;

    /// Set the char codeset translator.
    void char_translator(RDR_Char_Codeset_Translator *);
    /// Set the wchar codeset translator.
    void wchar_translator(RDR_WChar_Codeset_Translator *);

    /// set the global size of serialized wchars. This may be different
    /// than the size of a wchar_t.
    static void wchar_maxbytes(size_t max_bytes);

    /// access the serialized size of wchars.
    static size_t wchar_maxbytes(void);

    /**
     * Return alignment of the wr_ptr(), with respect to the start of
     * the CDR stream.  This is not the same as the alignment of
     * current->wr_ptr()!
     */
    size_t current_alignment(void) const;

    /**
     * Returns (in @a buf) the next position in the buffer aligned to
     * @a size, it advances the Message_Block wr_ptr past the data
     * (i.e., @a buf + @a size). If necessary it grows the Message_Block
     * buffer.  Sets the good_bit to false and returns a -1 on failure.
     */
    int adjust(size_t size, char *&buf);

    /// As above, but now the size and alignment requirements may be
    /// different.
    int adjust(size_t size, size_t align, char *&buf);

    /// If non-zero then this stream is writing in non-native byte order,
    /// this is only meaningful if ACE_ENABLE_SWAP_ON_WRITE is defined.
    bool do_byte_swap(void) const;

    /// If <do_byte_swap> returns 0, this returns RDR_BYTE_ORDER else
    /// it returns !RDR_BYTE_ORDER.
    int byte_order(void) const;

    /// For use by a gateway, which creates the output stream for the
    /// reply to the client in its native byte order, but which must
    /// send the reply in the byte order of the target's reply to the
    /// gateway.
    void reset_byte_order(int byte_order);

    /// set GIOP version info
    void set_version(Octet major, Octet minor);

    /// Set the underlying GIOP version..
    void get_version(Octet &major, Octet &minor);

private:

    // Find the packet block in the chain of packet blocks
    // that the provide location locates.
    ACE_Message_Block* find(char* loc);

    /// disallow copying...
    RDROutputStream(const RDROutputStream& rhs);
    RDROutputStream & operator=(const RDROutputStream& rhs);

    Boolean write_1(const Octet *x);
    Boolean write_2(const UShort *x);
    Boolean write_4(const UInt *x);
    Boolean write_8(const ULongLong *x);
    Boolean write_16(const LongDouble *x);

    /**
     * write an array of @a length elements, each of @a size bytes and the
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
    Boolean write_array(const void *x, size_t size, size_t align,
            UInt length);

    Boolean write_wchar_array_i(const WChar* x, UInt length);

    /**
     * Grow the CDR stream. When it returns @a buf contains a pointer to
     * memory in the CDR stream, with at least @a size bytes ahead of it
     * and aligned to an <align> boundary. It moved the <wr_ptr> to <buf
     * + size>.
     */
    int grow_and_adjust(size_t size, size_t align, char *&buf);

private:
    /// The start of the chain of packet blocks.
    ACE_Message_Block start_;

    /// The current block in the chain where we are writing.
    ACE_Message_Block *current_;

    bool m_align;

#if !defined (ACE_LACKS_CDR_ALIGNMENT)
    /**
     * The current alignment as measured from the start of the buffer.
     * Usually this coincides with the alignment of the buffer in
     * memory, but, when we chain another buffer this "quasi invariant"
     * is broken.
     * The current_alignment is used to readjust the buffer following
     * the stolen packet block.
     */
    size_t current_alignment_;
#endif /* ACE_LACKS_CDR_ALIGNMENT */

    /**
     * Is the current block writable.  When we steal a buffer from the
     * user and just chain it into the packet block we are not supposed
     * to write on it, even if it is past the start and end of the
     * buffer.
     */
    bool current_is_writable_;

    /**
     * If not zero swap bytes at writing so the created CDR stream byte
     * order does *not* match the machine byte order.  The motivation
     * for such a beast is that in some setting a few (fast) machines
     * can be serving hundreds of slow machines with the opposite byte
     * order, so it makes sense (as a load balancing device) to put the
     * responsibility in the writers.  THIS IS NOT A STANDARD IN CORBA,
     * USE AT YOUR OWN RISK
     */
    bool do_byte_swap_;

    /// Set to false when an error ocurrs.
    bool good_bit_;

    /// Break-even point for copying.
    size_t const memcpy_tradeoff_;

protected:
    /// GIOP version information
    Octet major_version_;
    Octet minor_version_;

    /// If not nil, invoke for translation of character and string data.
    RDR_Char_Codeset_Translator *char_translator_;
    RDR_WChar_Codeset_Translator *wchar_translator_;

    /**
     * Some wide char codesets may be defined with a maximum number
     * of bytes that is smaller than the size of a wchar_t. This means
     * that the CDR cannot simply memcpy a block of wchars to and from
     * the stream, but must instead realign the bytes appropriately.
     * In cases when wchar i/o is not allowed, such as with GIOP 1.0,
     * or not having a native wchar codeset defined, the maxbytes is
     * set to zero, indicating no wchar data is allowed.
     */
    static size_t wchar_maxbytes_;
};

// ****************************************************************

/**
 * @class RDRInputStream
 *
 * @brief A CDR stream for reading, i.e. for demarshalling.
 *
 * This class is based on the the CORBA spec for Java (98-02-29),
 * java class omg.org.CORBA.portable.InputStream.  It diverts in a
 * few ways:
 * + Operations to retrieve basic types take parameters by
 * reference.
 * + Operations taking arrays don't have offsets, because in C++
 * it is easier to describe an array starting from x+offset.
 * + Operations return an error status, because exceptions are
 * not widely available in C++ (yet).
 */
class Euryale_Export RDRInputStream{
public:
    /// The translator need privileged access to efficiently demarshal
    /// arrays and the such
    friend class RDR_Char_Codeset_Translator;
    friend class RDR_WChar_Codeset_Translator;

    /**
     * Create an input stream from an arbitrary buffer.  The buffer must
     * be properly aligned because this contructor will *not* work if
     * the buffer is aligned unproperly.See ACE_ptr_align_binary() for
     * instructions on how to align a pointer properly and use
     * MAX_ALIGNMENT for the correct alignment.
     */
    RDRInputStream(const char *buf, size_t bufsiz,
            int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    /// Create an empty input stream. The caller is responsible for
    /// putting the right data and providing the right alignment.
    RDRInputStream(size_t bufsiz, int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    /// Create an input stream from an ACE_Message_Block
    /**
     * The alignment of the @arg data block is carried into the new
     * RDRInputStream object. This constructor either increments the
     * @arg data reference count, or copies the data (if it's a compound
     * packet block) so the caller can release the block immediately
     * upon return.
     */
    RDRInputStream(const ACE_Message_Block *data,
            int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION,
            ACE_Lock* lock = 0);

    /// Create an input stream from an ACE_Data_Block. The <flag>
    /// indicates whether the <data> can be deleted by the CDR stream
    /// or not
    RDRInputStream(ACE_Data_Block *data,
            ACE_Message_Block::Message_Flags flag = 0,
            int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    /// Create an input stream from an ACE_Data_Block. It also sets the
    /// read and write pointers at the desired positions. This would be
    /// helpful if the applications desires to create a new CDR stream
    /// from a semi-processed datablock.
    RDRInputStream(ACE_Data_Block *data, ACE_Message_Block::Message_Flags flag,
            size_t read_pointer_position, size_t write_pointer_position,
            int byte_order = RDR_BYTE_ORDER,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    /**
     * These make a copy of the current stream state, but do not copy
     * the internal buffer, so the same stream can be read multiple
     * times efficiently.
     */
    RDRInputStream(const RDRInputStream& rhs);

    RDRInputStream & operator=(const RDRInputStream& rhs);

    /// When interpreting indirected TypeCodes it is useful to make a
    /// "copy" of the stream starting in the new position.
    RDRInputStream(const RDRInputStream& rhs, size_t size, Int offset);

    /// This creates an encapsulated stream, the first byte must be (per
    /// the spec) the byte order of the encapsulation.
    RDRInputStream(const RDRInputStream& rhs, size_t size);

    /// Create an input CDR from an output CDR.
    RDRInputStream(const RDROutputStream& rhs,
            ACE_Allocator* buffer_allocator = 0,
            ACE_Allocator* data_block_allocator = 0,
            ACE_Allocator* message_block_allocator = 0);

    /// Helper class to transfer the contents from one input CDR to
    /// another without requiring any extra memory allocations, data
    /// copies or too many temporaries.

    struct Euryale_Export Transfer_Contents {
        Transfer_Contents(RDRInputStream & rhs);

        RDRInputStream &rhs_;
    };
    /// Transfer the contents from <rhs> to a new CDR
    RDRInputStream(Transfer_Contents rhs);

    /// Destructor
    ~RDRInputStream(void);

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
    Boolean read_ulonglong(ULong& x);
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
    void steal_from(RDRInputStream &cdr);

    /// Exchange data blocks with the caller of this method. The read
    /// and write pointers are also exchanged.
    /**
     * @note We now do only with the start_ packet block.
     */
    void exchange_data_blocks(RDRInputStream &cdr);

    /// Copy the data portion from the @c cdr to this cdr and return the
    /// data content (ie. the ACE_Data_Block) from this CDR to the
    /// caller.
    /**
     * @note The caller is responsible for managing the memory of the
     *       returned ACE_Data_Block.
     */
    ACE_Data_Block* clone_from(RDRInputStream &cdr);

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
    RDR_Char_Codeset_Translator *char_translator(void) const;
    RDR_WChar_Codeset_Translator *wchar_translator(void) const;

    /// Set the codeset translators.
    void char_translator(RDR_Char_Codeset_Translator *);
    void wchar_translator(RDR_WChar_Codeset_Translator *);

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
    RDR_Char_Codeset_Translator *char_translator_;
    RDR_WChar_Codeset_Translator *wchar_translator_;

private:

    Boolean read_1(Octet *x);
    Boolean read_2(UShort *x);
    Boolean read_4(UInt *x);
    Boolean read_8(ULong *x);
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

// ****************************************************************

/**
 * @class RDR_Char_Codeset_Translator
 *
 * @brief Codeset translation routines runtime to both Output and Input
 * CDR streams.
 *
 * This class is a base class for defining codeset translation
 * routines to handle the character set translations required by
 * both CDR Input streams and CDR Output streams.
 *
 * Translators are reference counted. This allows for stateful as well
 * as stateless translators. Stateless translators will be allocated
 * once whereas CDR Streams own their own copy of a stateful translator.
 */
class Euryale_Export RDR_Char_Codeset_Translator {
public:
    virtual ~RDR_Char_Codeset_Translator();

    /// Read a single character from the stream, converting from the
    /// stream codeset to the native codeset
    virtual Boolean read_char(RDRInputStream&, Char&) = 0;

    /// Read a string from the stream, including the length, converting
    /// the characters from the stream codeset to the native codeset
    virtual Boolean read_string(RDRInputStream&, Char *&) = 0;

    /// Read an array of characters from the stream, converting the
    /// characters from the stream codeset to the native codeset.
    virtual Boolean read_char_array(RDRInputStream&, Char*,
            UInt) = 0;

    /// Write a single character to the stream, converting from the
    /// native codeset to the stream codeset
    virtual Boolean write_char(RDROutputStream&, Char) = 0;

    /// Write a string to the stream, including the length, converting
    /// from the native codeset to the stream codeset
    virtual Boolean write_string(RDROutputStream&, UInt,
            const Char*) = 0;

    /// Write an array of characters to the stream, converting from the
    /// native codeset to the stream codeset
    virtual Boolean write_char_array(RDROutputStream&, const Char*,
            UInt) = 0;

    virtual UInt ncs() = 0;
    virtual UInt tcs() = 0;
protected:
    /// Children have access to low-level routines because they cannot
    /// use read_char or something similar (it would recurse).
    Boolean read_1(RDRInputStream& input, Octet *x);
    Boolean write_1(RDROutputStream& output, const Octet *x);

    /// Efficiently read @a length elements of size @a size each from
    /// <input> into <x>; the data must be aligned to <align>.
    Boolean read_array(RDRInputStream& input, void* x, size_t size,
            size_t align, UInt length);

    /**
     * Efficiently write @a length elements of size @a size from <x> into
     * <output>. Before inserting the elements enough padding is added
     * to ensure that the elements will be aligned to <align> in the
     * stream.
     */
    Boolean write_array(RDROutputStream& output, const void *x,
            size_t size, size_t align, UInt length);

    /**
     * Exposes the stream implementation of <adjust>, this is useful in
     * many cases to minimize memory allocations during marshaling.
     * On success @a buf will contain a contiguous area in the CDR stream
     * that can hold @a size bytes aligned to <align>.
     * Results
     */
    int adjust(RDROutputStream& out, size_t size, size_t align, char *&buf);

    /// Used by derived classes to set errors in the CDR stream.
    void good_bit(RDROutputStream& out, bool bit);

    /// Obtain the CDR Stream's major & minor version values.
    Octet major_version(RDRInputStream& input);
    Octet minor_version(RDRInputStream& input);
    Octet major_version(RDROutputStream& output);
    Octet minor_version(RDROutputStream& output);
};

// ****************************************************************

/**
 * @class RDR_WChar_Codeset_Translator
 *
 * @brief Codeset translation routines runtime to both Output and Input
 * CDR streams.
 *
 * This class is a base class for defining codeset translation
 * routines to handle the character set translations required by
 * both CDR Input streams and CDR Output streams.
 */
class Euryale_Export RDR_WChar_Codeset_Translator {
public:
    virtual ~RDR_WChar_Codeset_Translator();

    virtual Boolean read_wchar(RDRInputStream&, WChar&) = 0;
    virtual Boolean read_wstring(RDRInputStream&, WChar *&) = 0;
    virtual Boolean read_wchar_array(RDRInputStream&, WChar*,
            UInt) = 0;
    virtual Boolean write_wchar(RDROutputStream&, WChar) = 0;
    virtual Boolean write_wstring(RDROutputStream&, UInt,
            const WChar*) = 0;
    virtual Boolean write_wchar_array(RDROutputStream&, const WChar*,
            UInt) = 0;

    virtual UInt ncs() = 0;
    virtual UInt tcs() = 0;
protected:
    /// Children have access to low-level routines because they cannot
    /// use read_char or something similar (it would recurse).
    Boolean read_1(RDRInputStream& input, Octet *x);
    Boolean read_2(RDRInputStream& input, UShort *x);
    Boolean read_4(RDRInputStream& input, UInt *x);
    Boolean write_1(RDROutputStream& output, const Octet *x);
    Boolean write_2(RDROutputStream& output, const UShort *x);
    Boolean write_4(RDROutputStream& output, const UInt *x);

    /// Efficiently read @a length elements of size @a size each from
    /// <input> into <x>; the data must be aligned to <align>.
    Boolean read_array(RDRInputStream& input, void* x, size_t size,
            size_t align, UInt length);

    /**
     * Efficiently write @a length elements of size @a size from <x> into
     * <output>. Before inserting the elements enough padding is added
     * to ensure that the elements will be aligned to <align> in the
     * stream.
     */
    Boolean write_array(RDROutputStream& output, const void *x,
            size_t size, size_t align, UInt length);

    /**
     * Exposes the stream implementation of <adjust>, this is useful in
     * many cases to minimize memory allocations during marshaling.
     * On success @a buf will contain a contiguous area in the CDR stream
     * that can hold @a size bytes aligned to <align>.
     * Results
     */
    int adjust(RDROutputStream& out, size_t size, size_t align, char *&buf);

    /// Used by derived classes to set errors in the CDR stream.
    void good_bit(RDROutputStream& out, bool bit);

    /// Obtain the CDR Stream's major & minor version values.
    Octet major_version(RDRInputStream& input);
    Octet minor_version(RDRInputStream& input);
    Octet major_version(RDROutputStream& output);
    Octet minor_version(RDROutputStream& output);

};

// @@ These operators should not be inlined since they force SString.h
//    to be included in this header.
extern Euryale_Export Boolean operator<<(RDROutputStream &os,
        const ACE_CString &x);

extern Euryale_Export Boolean operator>>(RDRInputStream &is, ACE_CString &x);
/*class X{
public:
    X(){}

    ACE_Message_Block* m_mb;
};*/


#if defined (__ACE_INLINE__)
#include <ace/Message_Block.h>
#include "euryale/common/io/RDRStream_inl.h"
#else /* __ACE_INLINE__ */



// Not used by CORBA or TAO
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RToChar x);
// CDR output operators for primitive types

extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RToShort x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RToUShort x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RToLong x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RToULong x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RToLongLong x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RToULongLong x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream& os,
        LongDouble x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        Float x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        Double x);

// CDR output operator from helper classes

extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RDROutputStream::from_boolean x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RDROutputStream::from_char x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RDROutputStream::from_wchar x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RDROutputStream::from_octet x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RDROutputStream::from_string x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        RDROutputStream::from_wstring x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        const RToChar* x);
extern Euryale_Export RToBoolean operator<<(RDROutputStream &os,
        const WChar* x);

// Not used by CORBA or TAO
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RToChar &x);
// CDR input operators for primitive types

extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RToShort &x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RToUShort &x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RToLong &x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RToULong &x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RToLongLong &x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RToULongLong &x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        LongDouble &x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        Float &x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        Double &x);

// CDR input operator from helper classes

extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RDRInputStream::to_boolean x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RDRInputStream::to_char x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RDRInputStream::to_wchar x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RDRInputStream::to_octet x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RDRInputStream::to_string x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RDRInputStream::to_wstring x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        RToChar*& x);
extern Euryale_Export RToBoolean operator>>(RDRInputStream &is,
        WChar*& x);


#endif /* __ACE_INLINE__ */

#if defined (GEN_OSTREAM_OPS)

namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

// ostream insertion operators for debugging code generated from IDL. All
// but these below are either in generated code itself or are unambiguous
// primitive types.

Euryale_Export std::ostream & operator<<(std::ostream &os,
        RDROutputStream::from_boolean x);

Euryale_Export std::ostream & operator<<(std::ostream &os,
        RDROutputStream::from_char x);

Euryale_Export std::ostream & operator<<(std::ostream &os,
        RDROutputStream::from_wchar x);

Euryale_Export std::ostream & operator<<(std::ostream &os,
        RDROutputStream::from_octet x);

}

}

}

}

#endif /* GEN_OSTREAM_OPS */

#include /**/ <ace/post.h>

#endif /*RDR_H_*/


