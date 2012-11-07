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
 * File:   ByteOutputStream.h
 * Author: rmartins
 *
 * Created on January 12, 2010, 12:47 PM
 */

#ifndef _BYTEOUTPUTSTREAM_H
#define	_BYTEOUTPUTSTREAM_H


#include <euryale/Euryale_export.h>
#include <euryale/Types.h>
#include <euryale/common/io/ByteStream.h>
#include <euryale/common/io/OutputStream.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <ace/SStringfwd.h>
#include <ace/Message_Block.h>
#include <ace/OS_Memory.h>




class CharCodesetTranslator;
class WCharCodesetTranslator;

class ByteOutputStream : public OutputStream {
public:
    ByteOutputStream(size_t size = 0, int byte_order = RDR_BYTE_ORDER,
            ACE_Allocator* buffer_allocator = 0,
            ACE_Allocator* data_block_allocator = 0,
            ACE_Allocator* message_block_allocator = 0,
            size_t memcpy_tradeoff = ACE_DEFAULT_CDR_MEMCPY_TRADEOFF,
            Octet major_version = RDR_GIOP_MAJOR_VERSION,
            Octet minor_version = RDR_GIOP_MINOR_VERSION);

    ByteOutputStream(ACE_Data_Block *data_block,
            int byte_order = RDR_BYTE_ORDER,
            ACE_Allocator* message_block_allocator = 0,
            size_t memcpy_tradeoff = ACE_DEFAULT_CDR_MEMCPY_TRADEOFF,
            Octet giop_major_version = RDR_GIOP_MAJOR_VERSION,
            Octet giop_minor_version = RDR_GIOP_MINOR_VERSION);


    virtual ~ByteOutputStream();
    bool current_is_writable_;
    int current_alignment_;
    ACE_Message_Block* current_;
    ACE_Message_Block start_;
    bool good_bit_;
    bool do_byte_swap_;
    size_t memcpy_tradeoff_;
    Octet major_version_;
    Octet minor_version_;

    int grow_and_adjust(size_t size,
            size_t align,
            char*& buf) {
        if (!this->current_is_writable_
                || this->current_->cont() == 0
                || this->current_->cont()->size() < size + MAX_ALIGNMENT) {
            // Calculate the new buffer's length; if growing for encode, we
            // don't grow in "small" chunks because of the cost.
            size_t cursize = this->current_->size();
            if (this->current_->cont() != 0)
                cursize = this->current_->cont()->size();
            size_t minsize = size;

#if !defined (ACE_LACKS_CDR_ALIGNMENT)
            minsize += MAX_ALIGNMENT;
#endif /* ACE_LACKS_CDR_ALIGNMENT */

            // Make sure that there is enough room for <minsize> bytes, but
            // also make it bigger than whatever our current size is.
            if (minsize < cursize)
                minsize = cursize;

            const size_t newsize = RDR::next_size(minsize);

            this->good_bit_ = false;
            ACE_Message_Block* tmp = 0;
            ACE_NEW_RETURN(tmp,
                    ACE_Message_Block(newsize,
                    ACE_Message_Block::MB_DATA,
                    0,
                    0,
                    this->current_->data_block()->allocator_strategy(),
                    0,
                    0,
                    ACE_Time_Value::zero,
                    ACE_Time_Value::max_time,
                    this->current_->data_block()->data_block_allocator()),
                    -1);

            // Packet block initialization may fail while the construction
            // succeds.  Since as a matter of policy, ACE may throw no
            // exceptions, we have to do a separate check like this.
            if (tmp != 0 && tmp->size() < newsize) {
                delete tmp;
                errno = ENOMEM;
                return -1;
            }

            this->good_bit_ = true;

#if !defined (ACE_LACKS_CDR_ALIGNMENT)
            // The new block must start with the same alignment as the
            // previous block finished.
            ptrdiff_t const tmpalign =
                    reinterpret_cast<ptrdiff_t> (tmp->rd_ptr()) % MAX_ALIGNMENT;
            ptrdiff_t const curalign =
                    static_cast<ptrdiff_t> (this->current_alignment_) % MAX_ALIGNMENT;
            ptrdiff_t offset = curalign - tmpalign;
            if (offset < 0)
                offset += MAX_ALIGNMENT;
            tmp->rd_ptr(static_cast<size_t> (offset));
            tmp->wr_ptr(tmp->rd_ptr());
#endif /* ACE_LACKS_CDR_ALIGNMENT */

            // grow the chain and set the current block.
            tmp->cont(this->current_->cont());
            this->current_->cont(tmp);
        }
        this->current_ = this->current_->cont();
        this->current_is_writable_ = true;
        return this->adjust(size, align, buf);
    }

    int adjust(size_t size,
            size_t align,
            char*& buf) {
        if (!this->current_is_writable_)
            return this->grow_and_adjust(size, align, buf);

#if !defined (ACE_LACKS_CDR_ALIGNMENT)
        size_t const offset =
                ACE_align_binary(this->current_alignment_, align)
                - this->current_alignment_;

        buf = this->current_->wr_ptr() + offset;
#else
        buf = this->current_->wr_ptr();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

        char * const end = buf + size;

        if (end <= this->current_->end() &&
                end >= buf) {
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
            this->current_alignment_ += offset + size;
#endif /* ACE_LACKS_CDR_ALIGNMENT */
            this->current_->wr_ptr(end);
            return 0;
        }

        return this->grow_and_adjust(size, align, buf);
    }

    int
    adjust(size_t size, char*& buf) {
        return this->adjust(size, size, buf);
    }

    Boolean
    write_1(const Octet *x) {
        char *buf = 0;
        if (this->adjust(1, buf) == 0) {
            *reinterpret_cast<Octet*> (buf) = *x;
            return true;
        }

        return false;
    }

    Boolean
    write_2(const UShort *x) {
        char *buf = 0;
        if (this->adjust(SHORT_SIZE, buf) == 0) {
#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
            *reinterpret_cast<UShort*> (buf) = *x;
            return true;
#else
            if (!this->do_byte_swap_) {
                *reinterpret_cast<RToUShort *> (buf) = *x;
                return true;
            } else {
                swap_2(reinterpret_cast<const char*> (x), buf);
                return true;
            }
#endif /* ACE_ENABLE_SWAP_ON_WRITE */
        }

        return false;
    }

    Boolean
    write_4(const UInt *x) {
        char *buf = 0;
        if (this->adjust(LONG_SIZE, buf) == 0) {
#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
            *reinterpret_cast<UInt*> (buf) = *x;
            return true;
#else
            if (!this->do_byte_swap_) {
                *reinterpret_cast<RToULong *> (buf) = *x;
                return true;
            } else {
                swap_4(reinterpret_cast<const char*> (x), buf);
                return true;
            }
#endif /* ACE_ENABLE_SWAP_ON_WRITE */
        }

        return false;
    }

    Boolean
    write_8(const ULongLong *x) {
        char *buf = 0;

        if (this->adjust(LONGLONG_SIZE, buf) == 0) {
#if defined (__arm__)
            // Convert to Intel format (12345678 => 56781234)
            const char *orig = reinterpret_cast<const char *> (x);
            char *target = buf;
            register ACE_UINT32 x =
                    *reinterpret_cast<const ACE_UINT32 *> (orig);
            register ACE_UINT32 y =
                    *reinterpret_cast<const ACE_UINT32 *> (orig + 4);
            *reinterpret_cast<ACE_UINT32 *> (target) = y;
            *reinterpret_cast<ACE_UINT32 *> (target + 4) = x;
            return true;
#else
#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
            *reinterpret_cast<ULong *> (buf) = *x;
            return true;
#else
            if (!this->do_byte_swap_) {
                *reinterpret_cast<RToULongLong *> (buf) = *x;
                return true;
            } else {
                swap_8(reinterpret_cast<const char*> (x), buf);
                return true;
            }
#endif /* ACE_ENABLE_SWAP_ON_WRITE */
#endif /* !__arm__ */
        }

        return false;
    }

    Boolean
    write_16(const LongDouble *x) {
        char* buf = 0;
        if (this->adjust(LONGDOUBLE_SIZE,
                LONGDOUBLE_ALIGN,
                buf) == 0) {
#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
            *reinterpret_cast<LongDouble*> (buf) = *x;
            return 1;
#else
            if (!this->do_byte_swap_) {
                *reinterpret_cast<LongDouble *> (buf) = *x;
                return true;
            } else {
                swap_16(reinterpret_cast<const char*> (x), buf);
                return true;
            }
#endif /* ACE_ENABLE_SWAP_ON_WRITE */
        }

        return false;
    }

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
    Boolean write_longlong_array(const LongLong* x, UInt length);
    Boolean write_ulonglong_array(const ULongLong *x,
            UInt length);
    Boolean write_float_array(const Float *x, UInt length);
    Boolean write_double_array(const Double *x, UInt length);
    Boolean write_longdouble_array(const LongDouble* x,
            UInt length);

    Boolean write_array(const void *x, size_t size, size_t align,
            UInt length);

    Boolean write_wchar_array_i(const WChar* x, UInt length);

    Boolean write_octet_array_mb(const ACE_Message_Block* mb);
    Boolean write_serializable_list(const list<Serializable*>& l);

    //static size_t wchar_maxbytes_;

    const ACE_Message_Block *begin(void) const {
        return &start_;
    }

    const ACE_Message_Block *current(void) const {
        return current_;
    }

    ACE_Message_Block* end(void) const {
        return this->current_->cont();
    }

    bool good_bit(void) const {
        return this->good_bit_;
    }

    size_t total_length(void) const;

    void reset(void);
    

    void
    reset_byte_order(int byte_order) {
        this->do_byte_swap_ = (byte_order != RDR_BYTE_ORDER);
    }

    CharCodesetTranslator *char_translator_;
    WCharCodesetTranslator *wchar_translator_;
private:

};

#endif	/* _BYTEOUTPUTSTREAM_H */

