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
 * File:   ByteInputStream.cpp
 * Author: rmartins
 * 
 * Created on January 12, 2010, 12:47 PM
 */

#include "ByteInputStream.h"
#include <euryale/common/Backtrace.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <euryale/common/io/CharCodesetTranslator.h>
#include <euryale/common/io/WCharCodesetTranslator.h>

ByteInputStream::ByteInputStream(const char *buf,
        size_t bufsiz,
        int byte_order,
        Octet major_version,
        Octet minor_version)
: start_(buf, bufsiz),
do_byte_swap_(byte_order != RDR_BYTE_ORDER),
good_bit_(true),
major_version_(major_version),
minor_version_(minor_version),
char_translator_(0),
wchar_translator_(0) {
    this->start_.wr_ptr(bufsiz);
}

ByteInputStream::ByteInputStream(size_t bufsiz,
        int byte_order,
        Octet major_version,
        Octet minor_version)
: start_(bufsiz),
do_byte_swap_(byte_order != RDR_BYTE_ORDER),
good_bit_(true),
major_version_(major_version),
minor_version_(minor_version),
char_translator_(0),
wchar_translator_(0) {
}

ByteInputStream::ByteInputStream(const ACE_Message_Block *data,
        int byte_order,
        Octet major_version,
        Octet minor_version,
        ACE_Lock* lock)
: start_(0, ACE_Message_Block::MB_DATA, 0, 0, 0, lock),
good_bit_(true),
major_version_(major_version),
minor_version_(minor_version),
char_translator_(0),
wchar_translator_(0) {
    this->reset(data, byte_order);
}

ByteInputStream::ByteInputStream(ACE_Data_Block *data,
        ACE_Message_Block::Message_Flags flag,
        int byte_order,
        Octet major_version,
        Octet minor_version)
: start_(data, flag),
do_byte_swap_(byte_order != RDR_BYTE_ORDER),
good_bit_(true),
major_version_(major_version),
minor_version_(minor_version),
char_translator_(0),
wchar_translator_(0) {
}

ByteInputStream::ByteInputStream(ACE_Data_Block *data,
        ACE_Message_Block::Message_Flags flag,
        size_t rd_pos,
        size_t wr_pos,
        int byte_order,
        Octet major_version,
        Octet minor_version)
: start_(data, flag),
do_byte_swap_(byte_order != RDR_BYTE_ORDER),
good_bit_(true),
major_version_(major_version),
minor_version_(minor_version),
char_translator_(0),
wchar_translator_(0) {
    // Set the read pointer
    this->start_.rd_ptr(rd_pos);

    // Set the write pointer after doing a sanity check.
    char* wrpos = this->start_.base() + wr_pos;

    if (this->start_.end() >= wrpos) {
        this->start_.wr_ptr(wr_pos);
    }
}

ByteInputStream::ByteInputStream(const ByteInputStream& rhs,
        size_t size,
        Int offset)
: start_(rhs.start_,
MAX_ALIGNMENT),
do_byte_swap_(rhs.do_byte_swap_),
good_bit_(true),
major_version_(rhs.major_version_),
minor_version_(rhs.minor_version_),
char_translator_(rhs.char_translator_),
wchar_translator_(rhs.wchar_translator_) {
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
    // Align the base pointer assuming that the incoming stream is also
    // aligned the way we are aligned
    char *incoming_start = ACE_ptr_align_binary(rhs.start_.base(),
            MAX_ALIGNMENT);
#else
    char *incoming_start = rhs.start_.base();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

    const size_t newpos =
            (rhs.start_.rd_ptr() - incoming_start) + offset;

    if (newpos <= this->start_.space()
            && newpos + size <= this->start_.space()) {
        this->start_.rd_ptr(newpos);
        this->start_.wr_ptr(newpos + size);
    } else
        this->good_bit_ = false;
}

ByteInputStream::ByteInputStream(const ByteInputStream& rhs,
        size_t size)
: start_(rhs.start_,
MAX_ALIGNMENT),
do_byte_swap_(rhs.do_byte_swap_),
good_bit_(true),
major_version_(rhs.major_version_),
minor_version_(rhs.minor_version_),
char_translator_(rhs.char_translator_),
wchar_translator_(rhs.wchar_translator_) {
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
    // Align the base pointer assuming that the incoming stream is also
    // aligned the way we are aligned
    char *incoming_start = ACE_ptr_align_binary(rhs.start_.base(),
            MAX_ALIGNMENT);
#else
    char *incoming_start = rhs.start_.base();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

    const size_t newpos =
            rhs.start_.rd_ptr() - incoming_start;

    if (newpos <= this->start_.space()
            && newpos + size <= this->start_.space()) {
        // Notice that ACE_Message_Block::duplicate may leave the
        // wr_ptr() with a higher value than what we actually want.
        this->start_.rd_ptr(newpos);
        this->start_.wr_ptr(newpos + size);

        Octet byte_order = 0;
        (void) this->read_octet(byte_order);
        this->do_byte_swap_ = (byte_order != RDR_BYTE_ORDER);
    } else
        this->good_bit_ = false;
}

ByteInputStream::ByteInputStream(const ByteInputStream& rhs)
: start_(rhs.start_,
MAX_ALIGNMENT),
do_byte_swap_(rhs.do_byte_swap_),
good_bit_(true),
major_version_(rhs.major_version_),
minor_version_(rhs.minor_version_),
char_translator_(rhs.char_translator_),
wchar_translator_(rhs.wchar_translator_) {
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
    char *buf = ACE_ptr_align_binary(rhs.start_.base(),
            MAX_ALIGNMENT);
#else
    char *buf = rhs.start_.base();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

    size_t rd_offset = rhs.start_.rd_ptr() - buf;
    size_t wr_offset = rhs.start_.wr_ptr() - buf;
    this->start_.rd_ptr(rd_offset);
    this->start_.wr_ptr(wr_offset);
}

ByteInputStream::ByteInputStream(ByteInputStream::Transfer_Contents x)
: start_(x.rhs_.start_.data_block()),
do_byte_swap_(x.rhs_.do_byte_swap_),
good_bit_(true),
major_version_(x.rhs_.major_version_),
minor_version_(x.rhs_.minor_version_),
char_translator_(x.rhs_.char_translator_),
wchar_translator_(x.rhs_.wchar_translator_) {

    this->start_.rd_ptr(x.rhs_.start_.rd_ptr());
    this->start_.wr_ptr(x.rhs_.start_.wr_ptr());

    ACE_Data_Block* db = this->start_.data_block()->clone_nocopy();
    (void) x.rhs_.start_.replace_data_block(db);
}

ByteInputStream&
        ByteInputStream::operator=(const ByteInputStream& rhs) {
    if (this != &rhs) {
        this->start_.data_block(rhs.start_.data_block()->duplicate());
        this->start_.rd_ptr(rhs.start_.rd_ptr());
        this->start_.wr_ptr(rhs.start_.wr_ptr());
        this->do_byte_swap_ = rhs.do_byte_swap_;
        this->good_bit_ = true;
        this->char_translator_ = rhs.char_translator_;
        this->major_version_ = rhs.major_version_;
        this->minor_version_ = rhs.minor_version_;
    }
    return *this;
}

ByteInputStream::ByteInputStream(const ByteOutputStream& rhs,
        ACE_Allocator* buffer_allocator,
        ACE_Allocator* data_block_allocator,
        ACE_Allocator* message_block_allocator)
: start_(rhs.total_length() + MAX_ALIGNMENT,
ACE_Message_Block::MB_DATA,
0,
0,
buffer_allocator,
0,
0,
ACE_Time_Value::zero,
ACE_Time_Value::max_time,
data_block_allocator,
message_block_allocator),
do_byte_swap_(rhs.do_byte_swap_),
good_bit_(true),
major_version_(rhs.major_version_),
minor_version_(rhs.minor_version_),
char_translator_(rhs.char_translator_),
wchar_translator_(rhs.wchar_translator_) {
    RDR::mb_align(&this->start_);
    for (const ACE_Message_Block *i = rhs.begin();
            i != rhs.end();
            i = i->cont())
        this->start_.copy(i->rd_ptr(), i->length());
}

Boolean
ByteInputStream::skip_wchar(void) {
    if (static_cast<Short> (major_version_) == 1
            && static_cast<Short> (minor_version_) == 2) {
        Octet len;
        if (this->read_1(&len))
            return this->skip_bytes(static_cast<size_t> (len));
    } else {
        WChar x;
        void * const temp = &x;
        if (RDR::wchar_maxbytes() == 2)
            return this->read_2(reinterpret_cast<UShort *> (temp));
        else
            return this->read_4(reinterpret_cast<UInt *> (temp));
    }

    return (this->good_bit_ = false);
}

Boolean
ByteInputStream::read_wchar(WChar& x) {
    if (this->wchar_translator_ != 0) {
        this->good_bit_ = this->wchar_translator_->read_wchar(*this, x);
        return this->good_bit_;
    }
    if (RDR::wchar_maxbytes() == 0) {
        errno = EACCES;
        return (this->good_bit_ = false);
    }

    if (RDR::wchar_maxbytes() == sizeof (WChar)) {
        if (static_cast<Short> (major_version_) == 1
                && static_cast<Short> (minor_version_) == 2) {
            Octet len;

            if (this->read_1(&len))
                return this->read_array
                    (reinterpret_cast<Octet*> (&x),
                    static_cast<UInt> (len),
                    OCTET_ALIGN,
                    1);

            else
                return (this->good_bit_ = false);
        }

        void * const temp = &x;
        if (sizeof (WChar) == 2)
            return this->read_2(reinterpret_cast<UShort *> (temp));
        else
            return this->read_4(reinterpret_cast<UInt *> (temp));
    }

    if (static_cast<Short> (major_version_) == 1
            && static_cast<Short> (minor_version_) == 2) {
        Octet len;

        if (this->read_1(&len)) {
            if (len == 2) {
                Short sx;
                if (this->read_array
                        (reinterpret_cast<Octet*> (&sx),
                        static_cast<UInt> (len),
                        OCTET_ALIGN,
                        1)) {
                    x = static_cast<WChar> (sx);
                    return true;
                }
            } else {
                Octet ox;
                if (this->read_array
                        (reinterpret_cast<Octet*> (&ox),
                        static_cast<UInt> (len),
                        OCTET_ALIGN,
                        1)) {
                    x = static_cast<WChar> (ox);
                    return true;
                }
            }
        }
    } else {
        if (RDR::wchar_maxbytes() == 2) {
            UShort sx;
            if (this->read_2(reinterpret_cast<UShort *> (&sx))) {
                x = static_cast<WChar> (sx);
                return true;
            }
        } else {
            Octet ox;
            if (this->read_1(&ox)) {
                x = static_cast<WChar> (ox);
                return true;
            }

        }
    }
    return (this->good_bit_ = false);
}

Boolean
ByteInputStream::read_string(Char *&x) {
    // @@ This is a slight violation of "Optimize for the runtime case",
    // i.e. normally the translator will be 0, but OTOH the code is
    // smaller and should be better for the cache ;-) ;-)
    if (this->char_translator_ != 0) {
        this->good_bit_ = this->char_translator_->read_string(*this, x);
        return this->good_bit_;
    }

    UInt len = 0;

    if (!this->read_ulong(len))
        return false;

    // A check for the length being too great is done later in the
    // call to read_char_array but we want to have it done before
    // the memory is allocated.
    if (len > 0 && len <= this->length()) {
        ACE_NEW_RETURN(x,
                Char[len],
                0);

        ACE_Auto_Basic_Array_Ptr<Char> safe_data(x);

        if (this->read_char_array(x, len)) {
            (void) safe_data.release();
            return true;
        }
    } else if (len == 0) {
        // Convert any null strings to empty strings since empty
        // strings can cause crashes. (See bug 58.)
        ACE_NEW_RETURN(x,
                Char[1],
                0);
        ACE_OS::strcpy(const_cast<char *&> (x), "");
        return true;
    }

    x = 0;
    return (this->good_bit_ = false);
}

Boolean
ByteInputStream::read_string(ACE_CString &x) {
    Char * data = 0;
    if (this->read_string(data)) {
        ACE_Auto_Basic_Array_Ptr<Char> safe_data(data);
        x = data;
        return true;
    }

    x = "";
    return (this->good_bit_ = false);
}

Boolean
ByteInputStream::read_wstring(WChar*& x) {
    // @@ This is a slight violation of "Optimize for the runtime case",
    // i.e. normally the translator will be 0, but OTOH the code is
    // smaller and should be better for the cache ;-) ;-)
    if (this->wchar_translator_ != 0) {
        this->good_bit_ = this->wchar_translator_->read_wstring(*this, x);
        return this->good_bit_;
    }
    if (RDR::wchar_maxbytes() == 0) {
        errno = EACCES;
        return (this->good_bit_ = false);
    }

    UInt len = 0;
    if (!this->read_ulong(len))
        return false;

    // A check for the length being too great is done later in the
    // call to read_char_array but we want to have it done before
    // the memory is allocated.
    if (len > 0 && len <= this->length()) {
        ACE_Auto_Basic_Array_Ptr<WChar> safe_data;

        if (static_cast<Short> (this->major_version_) == 1
                && static_cast<Short> (this->minor_version_) == 2) {
            len /= RDR::wchar_maxbytes();

            //allocating one extra for the null character needed by applications
            ACE_NEW_RETURN(x,
                    WChar [len + 1],
                    false);

            ACE_auto_ptr_reset(safe_data, x);

            if (this->read_wchar_array(x, len)) {

                //Null character used by applications to find the end of
                //the wstring
                //Is this okay with the GIOP 1.2 spec??
                x[len] = '\x00';

                (void) safe_data.release();

                return true;
            }
        } else {
            ACE_NEW_RETURN(x,
                    WChar [len],
                    false);

            ACE_auto_ptr_reset(safe_data, x);

            if (this->read_wchar_array(x, len)) {
                (void) safe_data.release();

                return true;
            }
        }
    } else if (len == 0) {
        // Convert any null strings to empty strings since empty
        // strings can cause crashes. (See bug 58.)
        ACE_NEW_RETURN(x,
                WChar[1],
                false);
        x[0] = '\x00';
        return true;
    }

    this->good_bit_ = false;
    x = 0;
    return false;
}

Boolean
ByteInputStream::read_array(void* x,
        size_t size,
        size_t align,
        UInt length) {
    if (length == 0)
        return true;
    char* buf = 0;

    if (this->adjust(size * length, align, buf) == 0) {
#if defined (ACE_DISABLE_SWAP_ON_READ)
        ACE_OS::memcpy(x, buf, size * length);
#else
        if (!this->do_byte_swap_ || size == 1)
            ACE_OS::memcpy(x, buf, size * length);
        else {
            char *target = reinterpret_cast<char*> (x);
            switch (size) {
                case 2:
                    swap_2_array(buf, target, length);
                    break;
                case 4:
                    swap_4_array(buf, target, length);
                    break;
                case 8:
                    swap_8_array(buf, target, length);
                    break;
                case 16:
                    swap_16_array(buf, target, length);
                    break;
                default:
                    // TODO: print something?
                    this->good_bit_ = false;
                    return false;
            }
        }
#endif /* ACE_DISABLE_SWAP_ON_READ */
        return this->good_bit_;
    }
    return false;
}

Boolean
ByteInputStream::read_wchar_array_i(WChar* x,
        UInt length) {
    if (length == 0)
        return true;
    char* buf = 0;
    size_t const align = (RDR::wchar_maxbytes() == 2) ?
            SHORT_ALIGN :
            OCTET_ALIGN;

    if (this->adjust(RDR::wchar_maxbytes() * length, align, buf) == 0) {
        if (RDR::wchar_maxbytes() == 2) {
            UShort *sb = reinterpret_cast<UShort *> (buf);
            for (size_t i = 0; i < length; ++i)
#if defined (ACE_DISABLE_SWAP_ON_READ)
                x[i] = static_cast<WChar> (sb[i]);
#else
                if (!this->do_byte_swap_)
                    x[i] = static_cast<WChar> (sb[i]);
                else {
                    RToUShort sx;
                    swap_2(&buf[i * 2], reinterpret_cast<char *> (&sx));
                    x[i] = static_cast<WChar> (sx);
                }
#endif /* ACE_DISABLE_SWAP_ON_READ */
        } else {
            for (size_t i = 0; i < length; ++i)
                x[i] = static_cast<Octet> (buf[i]);
        }
        return this->good_bit_;
    }
    return false;
}

Boolean
ByteInputStream::read_boolean_array(Boolean *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    // It is hard to optimize this, the spec requires that on the wire
    // booleans be represented as a byte with value 0 or 1, but in
    // memory it is possible (though very unlikely) that a boolean has
    // a non-zero value (different from 1).
    // We resort to a simple loop.
    for (UInt i = 0; i != length && this->good_bit_; ++i)
        (void) this->read_boolean(x[i]);

    return this->good_bit_;
}

Boolean
ByteInputStream::read_1(Octet *x) {
    if (this->rd_ptr() < this->wr_ptr()) {
        *x = *reinterpret_cast<Octet*> (this->rd_ptr());
        this->start_.rd_ptr(1);
        return true;
    }

    this->good_bit_ = false;
    return false;
}

Boolean
ByteInputStream::read_2(UShort *x) {
    char *buf = 0;
    if (this->adjust(SHORT_SIZE, buf) == 0) {
#if !defined (ACE_DISABLE_SWAP_ON_READ)
        if (!this->do_byte_swap_)
            *x = *reinterpret_cast<RToUShort*> (buf);
        else
            swap_2(buf, reinterpret_cast<char*> (x));
#else
        *x = *reinterpret_cast<UShort*> (buf);
#endif /* ACE_DISABLE_SWAP_ON_READ */
        return true;
    }
    this->good_bit_ = false;
    return false;
}

Boolean
ByteInputStream::read_4(UInt *x) {
    char *buf = 0;
    if (this->adjust(LONG_SIZE, buf) == 0) {
#if !defined (ACE_DISABLE_SWAP_ON_READ)
        if (!this->do_byte_swap_)
            *x = *reinterpret_cast<RToULong*> (buf);
        else
            swap_4(buf, reinterpret_cast<char*> (x));
#else
        *x = *reinterpret_cast<UInt*> (buf);
#endif /* ACE_DISABLE_SWAP_ON_READ */
        return true;
    }
    this->good_bit_ = false;
    return false;
}

Boolean
ByteInputStream::read_8(ULongLong *x) {
    char *buf = 0;

    if (this->adjust(LONGLONG_SIZE, buf) == 0) {
#if !defined (ACE_DISABLE_SWAP_ON_READ)
#if defined (__arm__)
        if (!this->do_byte_swap_) {
            // Convert from Intel format (12345678 => 56781234)
            const char *orig = buf;
            char *target = reinterpret_cast<char *> (x);
            register ACE_UINT32 x =
                    *reinterpret_cast<const ACE_UINT32 *> (orig);
            register ACE_UINT32 y =
                    *reinterpret_cast<const ACE_UINT32 *> (orig + 4);
            *reinterpret_cast<ACE_UINT32 *> (target) = y;
            *reinterpret_cast<ACE_UINT32 *> (target + 4) = x;
        } else {
            // Convert from Sparc format (12345678 => 43218765)
            const char *orig = buf;
            char *target = reinterpret_cast<char *> (x);
            register ACE_UINT32 x =
                    *reinterpret_cast<const ACE_UINT32 *> (orig);
            register ACE_UINT32 y =
                    *reinterpret_cast<const ACE_UINT32 *> (orig + 4);
            x = (x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24);
            y = (y << 24) | ((y & 0xff00) << 8) | ((y & 0xff0000) >> 8) | (y >> 24);
            *reinterpret_cast<ACE_UINT32 *> (target) = x;
            *reinterpret_cast<ACE_UINT32 *> (target + 4) = y;
        }
#else
        if (!this->do_byte_swap_)
            *x = *reinterpret_cast<RToULongLong *> (buf);
        else
            swap_8(buf, reinterpret_cast<char *> (x));
#endif /* !__arm__ */
#else
        *x = *reinterpret_cast<ULong *> (buf);
#endif /* ACE_DISABLE_SWAP_ON_READ */
        return true;
    }

    this->good_bit_ = false;
    return false;
}

Boolean
ByteInputStream::read_16(LongDouble *x) {
    char *buf = 0;
    if (this->adjust(LONGDOUBLE_SIZE,
            LONGDOUBLE_ALIGN,
            buf) == 0) {
#if !defined (ACE_DISABLE_SWAP_ON_READ)
        if (!this->do_byte_swap_)
            *x = *reinterpret_cast<LongDouble *> (buf);
        else
            swap_16(buf, reinterpret_cast<char*> (x));
#else
        *x = *reinterpret_cast<LongDouble*> (buf);
#endif /* ACE_DISABLE_SWAP_ON_READ */
        return true;
    }

    this->good_bit_ = false;
    return false;
}

Boolean
ByteInputStream::skip_string(void) {
    UInt len = 0;
    if (this->read_ulong(len)) {
        if (this->rd_ptr() + len <= this->wr_ptr()) {
            this->rd_ptr(len);
            return true;
        }
        this->good_bit_ = false;
    }
    return false;
}

Boolean
ByteInputStream::skip_wstring(void) {
    Boolean continue_skipping = true;
    UInt len = 0;

    continue_skipping = read_ulong(len);

    if (continue_skipping && len != 0) {
        if (static_cast<Short> (this->major_version_) == 1
                && static_cast<Short> (this->minor_version_) == 2)
            continue_skipping = this->skip_bytes((size_t) len);
        else
            while (continue_skipping && len--)
                continue_skipping = this->skip_wchar();
    }
    return continue_skipping;
}

Boolean
ByteInputStream::skip_bytes(size_t len) {
    if (this->rd_ptr() + len <= this->wr_ptr()) {
        this->rd_ptr(len);
        return true;
    }
    this->good_bit_ = false;
    return false;
}

int
ByteInputStream::grow(size_t newsize) {
    if (RDR::grow(&this->start_, newsize) == -1)
        return -1;

    RDR::mb_align(&this->start_);
    this->start_.wr_ptr(newsize);
    return 0;
}

void
ByteInputStream::reset(const ACE_Message_Block* data,
        int byte_order) {
    this->reset_byte_order(byte_order);
    RDR::consolidate(&this->start_, data);
}

void
ByteInputStream::steal_from(ByteInputStream &cdr) {
    this->do_byte_swap_ = cdr.do_byte_swap_;
    this->start_.data_block(cdr.start_.data_block()->duplicate());

    // If the packet block had a DONT_DELETE flags, just clear it off..
    this->start_.clr_self_flags(ACE_Message_Block::DONT_DELETE);
    this->start_.rd_ptr(cdr.start_.rd_ptr());

    this->start_.wr_ptr(cdr.start_.wr_ptr());
    this->major_version_ = cdr.major_version_;
    this->minor_version_ = cdr.minor_version_;
    cdr.reset_contents();
}

void
ByteInputStream::exchange_data_blocks(ByteInputStream &cdr) {
    // Exchange byte orders
    int byte_order = cdr.do_byte_swap_;
    cdr.do_byte_swap_ = this->do_byte_swap_;
    this->do_byte_swap_ = byte_order;

    // Get the destination read and write pointers
    size_t drd_pos =
            cdr.start_.rd_ptr() - cdr.start_.base();
    size_t dwr_pos =
            cdr.start_.wr_ptr() - cdr.start_.base();

    // Get the source read & write pointers
    size_t srd_pos =
            this->start_.rd_ptr() - this->start_.base();
    size_t swr_pos =
            this->start_.wr_ptr() - this->start_.base();

    // Exchange data_blocks. Dont release any of the data blocks.
    ACE_Data_Block *dnb =
            this->start_.replace_data_block(cdr.start_.data_block());
    cdr.start_.replace_data_block(dnb);

    // Exchange the flags information..
    ACE_Message_Block::Message_Flags df = cdr.start_.self_flags();
    ACE_Message_Block::Message_Flags sf = this->start_.self_flags();

    cdr.start_.clr_self_flags(df);
    this->start_.clr_self_flags(sf);

    cdr.start_.set_self_flags(sf);
    this->start_.set_self_flags(df);

    // Reset the <cdr> pointers to zero before it is set again.
    cdr.start_.reset();
    this->start_.reset();

    // Set the read and write pointers.
    if (cdr.start_.size() >= srd_pos)
        cdr.start_.rd_ptr(srd_pos);

    if (cdr.start_.size() >= swr_pos)
        cdr.start_.wr_ptr(swr_pos);

    if (this->start_.size() >= drd_pos)
        this->start_.rd_ptr(drd_pos);

    if (this->start_.size() >= dwr_pos)
        this->start_.wr_ptr(dwr_pos);

    Octet dmajor = cdr.major_version_;
    Octet dminor = cdr.minor_version_;

    // Exchange the GIOP version info
    cdr.major_version_ = this->major_version_;
    cdr.minor_version_ = this->minor_version_;

    this->major_version_ = dmajor;
    this->minor_version_ = dminor;
}

ACE_Data_Block *
ByteInputStream::clone_from(ByteInputStream &cdr) {
    this->do_byte_swap_ = cdr.do_byte_swap_;

    // Get the read & write pointer positions in the incoming CDR
    // streams
    char *rd_ptr = cdr.start_.rd_ptr();
    char *wr_ptr = cdr.start_.wr_ptr();

    // Now reset the incoming CDR stream
    cdr.start_.reset();

    // As we have reset the stream, try to align the underlying packet
    // block in the incoming stream
    RDR::mb_align(&cdr.start_);

    // Get the read & write pointer positions again
    char *nrd_ptr = cdr.start_.rd_ptr();
    char *nwr_ptr = cdr.start_.wr_ptr();

    // Actual length of the stream is..
    // @todo: This will look idiotic, but we dont seem to have much of a
    // choice. How do we calculate the length of the incoming stream?
    // Calling the method before calling reset () would give us the
    // wrong length of the stream that needs copying.  So we do the
    // calulation like this
    // (1) We get the <rd_ptr> and <wr_ptr> positions of the incoming
    // stream.
    // (2) Then we reset the <incoming> stream and then align it.
    // (3) We get the <rd_ptr> and <wr_ptr> positions again. (Points #1
    // thru #3 has been done already)
    // (4) The difference in the <rd_ptr> and <wr_ptr> positions gives
    // us the following, the actual bytes traversed by the <rd_ptr> and
    // <wr_ptr>.
    // (5) The bytes traversed by the <wr_ptr> is the actual length of
    // the stream.

    // Actual bytes traversed
    size_t rd_bytes = rd_ptr - nrd_ptr;
    size_t wr_bytes = wr_ptr - nwr_ptr;

    RDR::mb_align(&this->start_);

    ACE_Data_Block *db =
            this->start_.data_block();

    // If the size of the data that needs to be copied are higher than
    // what is available, then do a reallocation.
    if (wr_bytes > (this->start_.size() - MAX_ALIGNMENT)) {
        // @@NOTE: We need to probably add another method to the packet
        // block interface to simplify this
        db =
                cdr.start_.data_block()->clone_nocopy();

        if (db == 0 || db->size((wr_bytes) +
                MAX_ALIGNMENT) == -1)
            return 0;

        // Replace our data block by using the incoming CDR stream.
        db = this->start_.replace_data_block(db);

        // Align the start_ packet block.
        RDR::mb_align(&this->start_);

        // Clear the DONT_DELETE flag if it has been set
        this->start_.clr_self_flags(ACE_Message_Block::DONT_DELETE);
    }

    // Now do the copy
    (void) ACE_OS::memcpy(this->start_.wr_ptr(),
            cdr.start_.rd_ptr(),
            wr_bytes);

    // Set the read pointer position to the same point as that was in
    // <incoming> cdr.
    this->start_.rd_ptr(rd_bytes);
    this->start_.wr_ptr(wr_bytes);

    // We have changed the read & write pointers for the incoming
    // stream. Set them back to the positions that they were before..
    cdr.start_.rd_ptr(rd_bytes);
    cdr.start_.wr_ptr(wr_bytes);

    this->major_version_ = cdr.major_version_;
    this->minor_version_ = cdr.minor_version_;

    // Copy the char/wchar translators
    this->char_translator_ = cdr.char_translator_;
    this->wchar_translator_ = cdr.wchar_translator_;

    return db;
}

ACE_Message_Block*
ByteInputStream::steal_contents(void) {
    ACE_Message_Block* block =
            this->start_.clone();
    this->start_.data_block(block->data_block()->clone());

    // If at all our packet had a DONT_DELETE flag set, just clear it
    // off.
    this->start_.clr_self_flags(ACE_Message_Block::DONT_DELETE);

    RDR::mb_align(&this->start_);

    return block;
}

void
ByteInputStream::reset_contents(void) {
    this->start_.data_block(this->start_.data_block()->clone_nocopy
            ());

    // Reset the flags...
    this->start_.clr_self_flags(ACE_Message_Block::DONT_DELETE);
}

void
ByteInputStream::reset_byte_order(int byte_order) {
    this->do_byte_swap_ = (byte_order != RDR_BYTE_ORDER);
}

bool
ByteInputStream::do_byte_swap(void) const {
    return this->do_byte_swap_;
}

int
ByteInputStream::byte_order(void) const {
    return this->do_byte_swap() ? !RDR_BYTE_ORDER : RDR_BYTE_ORDER;
}

int
ByteInputStream::align_read_ptr(size_t alignment) {
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
    char *buf = ACE_ptr_align_binary(this->rd_ptr(),
            alignment);
#else
    char *buf = this->rd_ptr();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

    if (buf <= this->wr_ptr()) {
        this->start_.rd_ptr(buf);
        return 0;
    }

    this->good_bit_ = false;
    return -1;
}

void
ByteInputStream::set_version(Octet major, Octet minor) {
    this->major_version_ = major;
    this->minor_version_ = minor;
}

void
ByteInputStream::get_version(Octet &major, Octet &minor) {
    major = this->major_version_;
    minor = this->minor_version_;
}

CharCodesetTranslator *
ByteInputStream::char_translator(void) const {
    return this->char_translator_;
}

WCharCodesetTranslator *
ByteInputStream::wchar_translator(void) const {
    return this->wchar_translator_;
}

void
ByteInputStream::char_translator(CharCodesetTranslator * ctran) {
    this->char_translator_ = ctran;
}

void
ByteInputStream::wchar_translator(WCharCodesetTranslator * wctran) {
    this->wchar_translator_ = wctran;
}

ByteInputStream::~ByteInputStream(void) {
     if (this->start_.cont () != 0)
    {
      ACE_Message_Block::release (this->start_.cont ());
      this->start_.cont (0);
    }    
}

Boolean
ByteInputStream::read_octet(Octet& x) {
    return this->read_1(&x);
}

Boolean
ByteInputStream::read_boolean(Boolean& x) {
    Octet tmp = 0;
    (void) this->read_octet(tmp);
    x = tmp ? true : false;
    return (Boolean) this->good_bit_;
}

Boolean
ByteInputStream::read_char(Char &x) {
    if (this->char_translator_ == 0) {
        void *temp = &x;
        return this->read_1(reinterpret_cast<Octet*> (temp));
    }
    return this->char_translator_->read_char(*this, x);
}

Boolean
ByteInputStream::read_short(Short &x) {
    void *temp = &x;
    return this->read_2(reinterpret_cast<UShort*> (temp));
}

Boolean
ByteInputStream::read_ushort(UShort &x) {
    return this->read_2(&x);
}

Boolean
ByteInputStream::read_long(Int &x) {
    void *temp = &x;
    return this->read_4(reinterpret_cast<UInt*> (temp));
}

Boolean
ByteInputStream::read_ulong(UInt &x) {
    return this->read_4(&x);
}

Boolean
ByteInputStream::read_longlong(LongLong &x) {
    void *temp = &x;
    return this->read_8(reinterpret_cast<ULongLong*> (temp));
}

Boolean
ByteInputStream::read_ulonglong(ULongLong &x) {
    return this->read_8(&x);
}

Boolean
ByteInputStream::read_float(Float &x) {
    void *temp = &x;
    return this->read_4(reinterpret_cast<UInt*> (temp));
}

Boolean
ByteInputStream::read_double(Double &x) {
    void *temp = &x;
    return this->read_8(reinterpret_cast<ULongLong*> (temp));
}

Boolean
ByteInputStream::read_longdouble(LongDouble &x) {
    return this->read_16(&x);
}

size_t
ByteInputStream::length(void) const {
    return this->start_.length();
}

Boolean
ByteInputStream::read_char_array(Char* x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    if (this->char_translator_ == 0)
        return this->read_array(x,
            OCTET_SIZE,
            OCTET_ALIGN,
            length);
    return this->char_translator_->read_char_array(*this, x, length);
}

Boolean
ByteInputStream::read_wchar_array(WChar* x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * RDR::wchar_maxbytes() > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    if (this->wchar_translator_ != 0)
        return this->wchar_translator_->read_wchar_array(*this, x, length);
    if (RDR::wchar_maxbytes() != sizeof (WChar))
        return this->read_wchar_array_i(x, length);
    return this->read_array(x,
            sizeof (WChar),
            sizeof (WChar) == 2
            ? SHORT_ALIGN
            : LONG_ALIGN,
            length);
}

Boolean
ByteInputStream::read_octet_array(Octet* x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ByteInputStream::read_octet_array: read_len(%u) buffer_len(%lu)\n",
    //        length,this->length())
    if (length * OCTET_SIZE > this->length()) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ByteInputStream::read_octet_array: Bad bit %u %u\n"),
                length * OCTET_SIZE, this->length()));        
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            OCTET_SIZE,
            OCTET_ALIGN,
            length);
}

Boolean
ByteInputStream::read_short_array(Short *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * SHORT_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            SHORT_SIZE,
            SHORT_ALIGN,
            length);
}

Boolean
ByteInputStream::read_ushort_array(UShort *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * SHORT_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            SHORT_SIZE,
            SHORT_ALIGN,
            length);
}

Boolean
ByteInputStream::read_long_array(Int *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * LONG_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            LONG_SIZE,
            LONG_ALIGN,
            length);
}

Boolean
ByteInputStream::read_ulong_array(UInt *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * LONG_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            LONG_SIZE,
            LONG_ALIGN,
            length);
}

Boolean
ByteInputStream::read_longlong_array(LongLong *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * LONGLONG_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            LONGLONG_SIZE,
            LONGLONG_ALIGN,
            length);
}

Boolean
ByteInputStream::read_ulonglong_array(ULong *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * LONGLONG_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            LONGLONG_SIZE,
            LONGLONG_ALIGN,
            length);
}

Boolean
ByteInputStream::read_float_array(Float *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * LONG_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            LONG_SIZE,
            LONG_ALIGN,
            length);
}

Boolean
ByteInputStream::read_double_array(Double *x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * LONGLONG_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }

    return this->read_array(x,
            LONGLONG_SIZE,
            LONGLONG_ALIGN,
            length);
}

Boolean
ByteInputStream::read_longdouble_array(LongDouble* x,
        UInt length) {
    // Make sure the length of the array isn't greater than the length of
    // the stream.
    if (length * LONGDOUBLE_SIZE > this->length()) {
        this->good_bit_ = false;
        return false;
    }
    return this->read_array(x,
            LONGDOUBLE_SIZE,
            LONGDOUBLE_ALIGN,
            length);
}

Boolean
ByteInputStream::skip_octet(void) {
    Octet x;
    return this->read_1(&x);
}

Boolean
ByteInputStream::skip_char(void) {
    return this->skip_octet(); // sizeof (Char) == sizeof (Octet)
}

Boolean
ByteInputStream::skip_boolean(void) {
    return this->skip_octet() && this->good_bit_;
}

Boolean
ByteInputStream::skip_ushort(void) {
    UShort x;
    return this->read_2(&x);
}

Boolean
ByteInputStream::skip_short(void) {
    return this->skip_ushort();
}

Boolean
ByteInputStream::skip_ulong(void) {
    UInt x;
    return this->read_4(&x);
}

Boolean
ByteInputStream::skip_long(void) {
    return this->skip_ulong(); // sizeof (Long) == sizeof (RTo_ULong)
}

Boolean
ByteInputStream::skip_ulonglong(void) {
    ULongLong x;
    return this->read_8(&x);
}

Boolean
ByteInputStream::skip_longlong(void) {
    return this->skip_ulonglong(); // sizeof (RToLongLong) == sizeof (RToULongLong)
}

Boolean
ByteInputStream::skip_float(void) {
    return this->skip_ulong(); // sizeof(Float) == sizeof (RTo_ULong)
}

Boolean
ByteInputStream::skip_double(void) {
    return this->skip_ulonglong(); // sizeof(Double) == sizeof (RToULongLong)
}

Boolean
ByteInputStream::skip_longdouble(void) {
    LongDouble x;
    return this->read_16(&x);
}

char*
ByteInputStream::end(void) {
    return this->start_.end();
}

void
ByteInputStream::rd_ptr(size_t offset) {
    this->start_.rd_ptr(offset);
}

char*
ByteInputStream::rd_ptr(void) {
    return this->start_.rd_ptr();
}

char*
ByteInputStream::wr_ptr(void) {
    return this->start_.wr_ptr();
}

int
ByteInputStream::adjust(size_t size,
        size_t align,
        char*& buf) {
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
    buf = ACE_ptr_align_binary(this->rd_ptr(), align);
#else
    buf = this->rd_ptr();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

    char * const end = buf + size;
    if (end <= this->wr_ptr()) {
        this->start_.rd_ptr(end);
        return 0;
    }

    this->good_bit_ = false;
    return -1;
#if defined (ACE_LACKS_CDR_ALIGNMENT)
    ACE_UNUSED_ARG(align);
#endif /* ACE_LACKS_CDR_ALIGNMENT */
}

int
ByteInputStream::adjust(size_t size,
        char*& buf) {
    return this->adjust(size, size, buf);
}

const ACE_Message_Block*
ByteInputStream::start(void) const {
    return &this->start_;
}

bool
ByteInputStream::good_bit(void) const {
    return this->good_bit_;
}

Boolean
operator>>(ByteInputStream &is, Char &x) {
    return is.read_char(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, Short &x) {
    return is.read_short(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, UShort &x) {
    return is.read_ushort(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, Int &x) {
    return is.read_long(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, UInt &x) {
    return is.read_ulong(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream& is, LongLong &x) {
    return is.read_longlong(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream& is, LongDouble &x) {
    return is.read_longdouble(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, Float &x) {
    return is.read_float(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, Double &x) {
    return is.read_double(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, Char *&x) {
    return is.read_string(x) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, WChar *&x) {
    return is.read_wstring(x) && is.good_bit();
}

// The following use the helper classes

Boolean
operator>>(ByteInputStream &is, ByteInputStream::to_boolean x) {
    return is.read_boolean(x.ref_);
}

Boolean
operator>>(ByteInputStream &is, ByteInputStream::to_char x) {
    return is.read_char(x.ref_) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, ByteInputStream::to_wchar x) {
    return is.read_wchar(x.ref_) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, ByteInputStream::to_octet x) {
    return is.read_octet(x.ref_) && is.good_bit();
}

Boolean
operator>>(ByteInputStream &is, ByteInputStream::to_string x) {
    // check if the bounds are satisfied
    return
    (is.read_string(const_cast<char *&> (x.val_))
            && is.good_bit()
            && (!x.bound_
            || ACE_OS::strlen(x.val_) <= x.bound_));
}

Boolean
operator>>(ByteInputStream &is, ByteInputStream::to_wstring x) {
    // check if the bounds are satisfied
    return
    (is.read_wstring(const_cast<WChar *&> (x.val_))
            && is.good_bit()
            && (!x.bound_
            || ACE_OS::strlen(x.val_) <= x.bound_));
}
