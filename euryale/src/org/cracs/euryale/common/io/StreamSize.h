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
// -*- C++ -*-

//=============================================================================
/**
 *  @file   CDR_Size.h
 *
 *  $Id: CDR_Size.h 80826 2008-03-04 14:51:23Z wotte $
 *
 *
 * ACE Common Data Representation (CDR) size-calculating stream.
 *
 *
 * The current implementation assumes that the host has 1-byte,
 * 2-byte and 4-byte integral types, and that it has single
 * precision and double precision IEEE floats.
 * Those assumptions are pretty good these days, with Crays beign
 * the only known exception.
 *
 *
 *  @author Boris Kolpackov <boris@kolpackov.net>
 *
 */
//=============================================================================

#ifndef ACE_CDR_SIZE_H
#define ACE_CDR_SIZE_H

#include /**/ <ace/pre.h>

#define ACE_LACKS_CDR_ALIGNMENT

//#include "ace/CDR_Base.h"
//#include "ace/CDR_Stream.h" // for ACE_OutputCDR::from_*

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <ace/SStringfwd.h>
#include <euryale/Types.h>
#include <euryale/common/io/Alignment.h>
#include <ace/Message_Block.h>
#include <euryale/common/io/OutputStream.h>

/**
 * @class ACE_SizeCDR
 *
 * @brief A CDR stream for calculating size of the representation.
 *
 */

class Serializable;
class StreamSize : public OutputStream, public Alignment {
public:
    /// Default constructor.
    StreamSize(Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    /// Returns @c false if an error has ocurred.
    bool good_bit(void) const;


    /// Reset current size.
    void reset(void);


    /// Return current size.
    size_t total_length(void) const;


    // Return 0 on failure and 1 on success.
    //@{ @name Size-calculating pseudo-write operations
    Boolean write_boolean(Boolean x);
    Boolean write_char(Char x);
    Boolean write_wchar(WChar x);
    Boolean write_octet(Octet x);
    Boolean write_short(Short x);
    Boolean write_ushort(UShort x);
    Boolean write_long(Long x);
    Boolean write_ulong(ULong x);
    Boolean write_longlong(const LongLong &x);
    Boolean write_ulonglong(const ULongLong &x);
    Boolean write_float(Float x);
    Boolean write_double(const Double &x);
    Boolean write_longdouble(const LongDouble &x);

    /// For string we offer methods that accept a precomputed length.
    Boolean write_string(const Char *x);
    Boolean write_string(ULong len,
            const Char *x);
    Boolean write_string(const ACE_CString &x);
    Boolean write_wstring(const WChar *x);
    Boolean write_wstring(ULong length,
            const WChar *x);
    //@}

    /// @note the portion written starts at <x> and ends
    ///    at <x + length>.
    /// The length is *NOT* stored into the CDR stream.
    //@{ @name Array write operations
    Boolean write_boolean_array(const Boolean *x,
            ULong length);
    Boolean write_char_array(const Char *x,
            ULong length);
    Boolean write_wchar_array(const WChar* x,
            ULong length);
    Boolean write_octet_array(const Octet* x,
            ULong length);
    Boolean write_short_array(const Short *x,
            ULong length);
    Boolean write_ushort_array(const UShort *x,
            ULong length);
    Boolean write_long_array(const Long *x,
            ULong length);
    Boolean write_ulong_array(const ULong *x,
            ULong length);
    Boolean write_longlong_array(const LongLong* x,
            ULong length);
    Boolean write_ulonglong_array(const ULongLong *x,
            ULong length);
    Boolean write_float_array(const Float *x,
            ULong length);
    Boolean write_double_array(const Double *x,
            ULong length);
    Boolean write_longdouble_array(const LongDouble* x,
            ULong length);

    ///
    /// Adjust to @a size and count <size> octets.
    void adjust(size_t size);

    /// As above, but now the size and alignment requirements may be
    /// different.
    void adjust(size_t size,
            size_t align);

    virtual Boolean write_serializable_list(const list<Serializable*>& l);

private:
    /// disallow copying...
    StreamSize(const StreamSize& rhs);
    StreamSize & operator=(const StreamSize& rhs);

    Boolean write_1(const Octet *x);
    Boolean write_2(const UShort *x);
    Boolean write_4(const ULong *x);
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
    Boolean write_array(const void *x,
            size_t size,
            size_t align,
            ULong length);


    Boolean write_wchar_array_i(const WChar* x,
            ULong length);

    virtual Boolean write_octet_array_mb(const ACE_Message_Block* mb)
    {
        // If the buffer is small and it fits in the current packet
        // block it is be cheaper just to copy the buffer.
        for (const ACE_Message_Block* i = mb;
                i != 0;
                i = i->cont()) {
            this->size_ += mb->length();
        }

        return true;
    }
    //@}

    

    virtual int consolidate()
    {
        return 0;
    }

    virtual const ACE_Message_Block *begin(void) const
    {
        return 0;
    }

    virtual const ACE_Message_Block *current(void) const
    {
        return 0;
    }

    virtual ACE_Message_Block* release(void)
    {
        return 0;
    }

    virtual Boolean replace(Int x, char* loc)
    {
        return false;
    }


private:
    /// Set to false when an error ocurrs.
    bool good_bit_;

    /// Current size.
    size_t size_;

protected:
    /// GIOP version information
    Octet major_version_;
    Octet minor_version_;
};


#if defined (__ACE_INLINE__)
#include <euryale/common/io/StreamSize.inl>
#else /* __ACE_INLINE__ */
#endif /* __ACE_INLINE__ */


#include /**/ <ace/post.h>

#endif /* ACE_CDR_SIZE_H */
