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
 * File:   ByteOutputStream.cpp
 * Author: rmartins
 * 
 * Created on January 12, 2010, 12:47 PM
 */

#include "ByteOutputStream.h"
#include <euryale/common/io/CharCodesetTranslator.h>
#include <euryale/common/io/WCharCodesetTranslator.h>
#include <euryale/serialization/Serializable.h>

ByteOutputStream::~ByteOutputStream() {
    if (this->start_.cont() != 0) {
        ACE_Message_Block::release(this->start_.cont());
        this->start_.cont(0);
    }
    this->current_ = 0;
}

ByteOutputStream::ByteOutputStream(size_t size,
        int byte_order,
        ACE_Allocator *buffer_allocator,
        ACE_Allocator *data_block_allocator,
        ACE_Allocator *message_block_allocator,
        size_t memcpy_tradeoff,
        Octet major_version,
        Octet minor_version)
: start_((size ? size : (size_t) DEFAULT_BUFSIZE) + MAX_ALIGNMENT,
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
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
current_alignment_(0),
#endif /* ACE_LACKS_CDR_ALIGNMENT */
current_is_writable_(true),
do_byte_swap_(byte_order != RDR_BYTE_ORDER),
good_bit_(true),
memcpy_tradeoff_(memcpy_tradeoff),
major_version_(major_version),
minor_version_(minor_version),
char_translator_(0),
wchar_translator_(0)
 {
    RDR::mb_align(&this->start_);
    this->current_ = &this->start_;
}

ByteOutputStream::ByteOutputStream(ACE_Data_Block *data_block,
        int byte_order,
        ACE_Allocator *message_block_allocator,
        size_t memcpy_tradeoff,
        Octet major_version,
        Octet minor_version)
: start_(data_block,
ACE_Message_Block::DONT_DELETE,
message_block_allocator),
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
current_alignment_(0),
#endif /* ACE_LACKS_CDR_ALIGNMENT */
current_is_writable_(true),
do_byte_swap_(byte_order != RDR_BYTE_ORDER),
good_bit_(true),
memcpy_tradeoff_(memcpy_tradeoff),
major_version_(major_version),
minor_version_(minor_version),
char_translator_(0),
wchar_translator_(0) {
    // We cannot trust the buffer to be properly aligned
    RDR::mb_align(&this->start_);
    this->current_ = &this->start_;
}

Boolean
ByteOutputStream::write_wchar_array_i(const WChar *x, UInt length) {
    if (length == 0)
        return true;
    char* buf = 0;
    size_t const align = (RDR::wchar_maxbytes_ == 2) ?
            SHORT_ALIGN :
            OCTET_ALIGN;

    if (this->adjust(RDR::wchar_maxbytes_ * length, align, buf) == 0) {
        if (RDR::wchar_maxbytes_ == 2) {
            UShort *sb = reinterpret_cast<UShort *> (buf);
            for (size_t i = 0; i < length; ++i)
#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
                sb[i] = static_cast<UShort> (x[i]);
#else
                if (!this->do_byte_swap_)
                    sb[i] = static_cast<RToUShort> (x[i]);
                else {
                    RToUShort sx = static_cast<RToUShort> (x[i]);
                    swap_2(reinterpret_cast<char *> (&sx), &buf[i * 2]);
                }
#endif /* ACE_ENABLE_SWAP_ON_WRITE */
        } else {
            for (size_t i = 0; i < length; ++i)
                buf[i] = static_cast<char> (x[i]);
        }
        return this->good_bit_;
    }
    return false;
}

Boolean
ByteOutputStream::write_array(const void *x,
        size_t size,
        size_t align,
        UInt length) {
    if (length == 0)
        return true;
    char *buf = 0;
    if (this->adjust(size * length, align, buf) == 0) {
#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
        ACE_OS::memcpy(buf, x, size * length);
        return true;
#else
        if (!this->do_byte_swap_ || size == 1) {
            ACE_OS::memcpy(buf, x, size * length);
            return true;
        } else {
            const char *source = reinterpret_cast<const char *> (x);
            switch (size) {
                case 2:
                    swap_2_array(source, buf, length);
                    return true;
                case 4:
                    swap_4_array(source, buf, length);
                    return true;
                case 8:
                    swap_8_array(source, buf, length);
                    return true;
                case 16:
                    swap_16_array(source, buf, length);
                    return true;
                default:
                    // TODO: print something?
                    this->good_bit_ = false;
                    return false;
            }
        }
#endif /* ACE_ENABLE_SWAP_ON_WRITE */
    }
    this->good_bit_ = false;
    return false;
}

Boolean
ByteOutputStream::write_octet(Octet x) {
    return this->write_1(&x);
}

Boolean
ByteOutputStream::write_boolean(Boolean x) {
    return
    static_cast<Boolean> (
            this->write_octet(
            x
            ? static_cast<Octet> (1)
            : static_cast<Octet> (0)));
}

Boolean
ByteOutputStream::write_char(Char x) {
    if (this->char_translator_ == 0) {
        Octet temp = static_cast<Octet> (x);
        return this->write_1(&temp);
    }
    return this->char_translator_->write_char(*this, x);
}

Boolean
ByteOutputStream::write_short(Short x) {
    UShort temp = static_cast<UShort> (x);
    return this->write_2(&temp);
}

Boolean
ByteOutputStream::write_ushort(UShort x) {
    return this->write_2(&x);
}

Boolean
ByteOutputStream::write_long(Int x) {
    UInt temp = static_cast<UInt> (x);
    return this->write_4(&temp);
}

Boolean
ByteOutputStream::write_ulong(UInt x) {
    return this->write_4(&x);
}

Boolean
ByteOutputStream::write_longlong(const LongLong &x) {
    void const * const temp = &x;
    return this->write_8(reinterpret_cast<ULongLong const *> (temp));
}

Boolean
ByteOutputStream::write_ulonglong(const ULongLong &x) {
    return this->write_8(&x);
}

Boolean
ByteOutputStream::write_float(Float x) {
    void const * const temp = &x;
    return this->write_4(reinterpret_cast<UInt const *> (temp));
}

Boolean
ByteOutputStream::write_double(const Double &x) {
    void const * const temp = &x;
    return this->write_8(reinterpret_cast<ULongLong const *> (temp));
}

Boolean
ByteOutputStream::write_longdouble(const LongDouble &x) {
    return this->write_16(&x);
}

Boolean
ByteOutputStream::write_wchar(WChar x) {
    if (this->wchar_translator_ != 0)
        return (this->good_bit_ = this->wchar_translator_->write_wchar(*this, x));
    if (RDR::wchar_maxbytes() == 0) {
        errno = EACCES;
        return (this->good_bit_ = false);
    }
    if (static_cast<Short> (major_version_) == 1
            && static_cast<Short> (minor_version_) == 2) {
        Octet len =
                static_cast<Octet> (RDR::wchar_maxbytes());
        if (this->write_1(&len)) {
            if (RDR::wchar_maxbytes() == sizeof (WChar))
                return
                this->write_octet_array(
                    reinterpret_cast<const Octet*> (&x),
                    static_cast<UInt> (len));
            else
                if (RDR::wchar_maxbytes() == 2) {
                Short sx = static_cast<Short> (x);
                return
                this->write_octet_array(
                        reinterpret_cast<const Octet*> (&sx),
                        static_cast<UInt> (len));
            } else {
                Octet ox = static_cast<Octet> (x);
                return
                this->write_octet_array(
                        reinterpret_cast<const Octet*> (&ox),
                        static_cast<UInt> (len));
            }
        }
    } else if (static_cast<Short> (minor_version_) == 0) { // wchar is not allowed with GIOP 1.0.
        errno = EINVAL;
        return (this->good_bit_ = false);
    }
    if (RDR::wchar_maxbytes() == sizeof (WChar)) {
        void const * const temp = &x;
        return
        this->write_4(reinterpret_cast<const UInt *> (temp));
    } else if (RDR::wchar_maxbytes() == 2) {
        Short sx = static_cast<Short> (x);
        return this->write_2(reinterpret_cast<const UShort *> (&sx));
    }
    Octet ox = static_cast<Octet> (x);
    return this->write_1(reinterpret_cast<const Octet *> (&ox));
}

Boolean
ByteOutputStream::write_string(const ACE_CString &x) {
    // @@ Leave this method in here, not the `.i' file so that we don't
    //    have to unnecessarily pull in the `ace/SString.h' header.
    return this->write_string(static_cast<UInt> (x.length()),
            x.c_str());
}

Boolean
ByteOutputStream::write_string(UInt len,
        const Char *x) {
    // @@ This is a slight violation of "Optimize for the runtime case",
    // i.e. normally the translator will be 0, but OTOH the code is
    // smaller and should be better for the cache ;-) ;-)
    if (this->char_translator_ != 0)
        return this->char_translator_->write_string(*this, len, x);

    if (len != 0) {
        if (this->write_ulong(len + 1))
            return this->write_char_array(x, len + 1);
    } else {
        // Be nice to programmers: treat nulls as empty strings not
        // errors. (OMG-IDL supports languages that don't use the C/C++
        // notion of null v. empty strings; nulls aren't part of the OMG-IDL
        // string model.)
        if (this->write_ulong(1))
            return this->write_char(0);
    }

    return (this->good_bit_ = false);
}

Boolean
ByteOutputStream::write_string(const Char *x) {
    if (x) {
        const UInt len =
                static_cast<UInt> (ACE_OS::strlen(x));
        return this->write_string(len, x);
    }

    return this->write_string(0, 0);
}

Boolean
ByteOutputStream::write_wstring(const WChar *x) {
    if (x) {
        UInt len =
                static_cast<UInt> (ACE_OS::strlen(x));
        return this->write_wstring(len, x);
    }

    return this->write_wstring(0, 0);
}

Boolean
ByteOutputStream::write_wstring(UInt len,
        const WChar *x) {
    // @@ This is a slight violation of "Optimize for the runtime case",
    // i.e. normally the translator will be 0, but OTOH the code is
    // smaller and should be better for the cache ;-) ;-)
    // What do we do for GIOP 1.2???
    if (this->wchar_translator_ != 0)
        return this->wchar_translator_->write_wstring(*this, len, x);
    if (RDR::wchar_maxbytes() == 0) {
        errno = EACCES;
        return (this->good_bit_ = false);
    }

    if (static_cast<Short> (this->major_version_) == 1
            && static_cast<Short> (this->minor_version_) == 2) {
        if (x != 0) {
            //In GIOP 1.2 the length field contains the number of bytes
            //the wstring occupies rather than number of wchars
            //Taking sizeof might not be a good way! This is a temporary fix.
            if (this->write_ulong(RDR::wchar_maxbytes() * len))
                return this->write_wchar_array(x, len);
        } else
            //In GIOP 1.2 zero length wstrings are legal
            return this->write_ulong(0);
    } else
        if (x != 0) {
        if (this->write_ulong(len + 1))
            return this->write_wchar_array(x, len + 1);
    } else if (this->write_ulong(1))
        return this->write_wchar(0);
    return (this->good_bit_ = false);
}

Boolean
ByteOutputStream::write_char_array(const Char *x,
        UInt length) {
    if (this->char_translator_ == 0)
        return this->write_array(x,
            OCTET_SIZE,
            OCTET_ALIGN,
            length);
    return this->char_translator_->write_char_array(*this, x, length);
}

Boolean
ByteOutputStream::write_wchar_array(const WChar* x,
        UInt length) {
    if (this->wchar_translator_)
        return this->wchar_translator_->write_wchar_array(*this, x, length);

    if (RDR::wchar_maxbytes_ == 0) {
        errno = EACCES;
        return (Boolean) (this->good_bit_ = false);
    }

    if (RDR::wchar_maxbytes_ == sizeof (WChar))
        return this->write_array(x,
            sizeof (WChar),
            sizeof (WChar) == 2
            ? SHORT_ALIGN
            : LONG_ALIGN,
            length);
    return this->write_wchar_array_i(x, length);
}

Boolean
ByteOutputStream::write_octet_array(const Octet* x,
        UInt length) {
    return this->write_array(x,
            OCTET_SIZE,
            OCTET_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_short_array(const Short *x,
        UInt length) {
    return this->write_array(x,
            SHORT_SIZE,
            SHORT_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_ushort_array(const UShort *x,
        UInt length) {
    return this->write_array(x,
            SHORT_SIZE,
            SHORT_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_long_array(const Int *x,
        UInt length) {
    return this->write_array(x,
            LONG_SIZE,
            LONG_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_ulong_array(const UInt *x,
        UInt length) {
    return this->write_array(x,
            LONG_SIZE,
            LONG_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_longlong_array(const LongLong *x,
        UInt length) {
    return this->write_array(x,
            LONGLONG_SIZE,
            LONGLONG_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_ulonglong_array(const ULongLong *x,
        UInt length) {
    return this->write_array(x,
            LONGLONG_SIZE,
            LONGLONG_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_float_array(const Float *x,
        UInt length) {
    return this->write_array(x,
            LONG_SIZE,
            LONG_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_double_array(const Double *x,
        UInt length) {
    return this->write_array(x,
            LONGLONG_SIZE,
            LONGLONG_ALIGN,
            length);
}

Boolean
ByteOutputStream::write_longdouble_array(const LongDouble* x,
        UInt length) {
    return this->write_array(x,
            LONGDOUBLE_SIZE,
            LONGDOUBLE_ALIGN,
            length);
}

Boolean ByteOutputStream::write_serializable_list(const list<Serializable*>& l) {
    list<Serializable*>::const_iterator iter = l.begin();
    while (iter != l.end()) {
        (*iter)->serialize(*this);
        iter++;
    }
    return true;
}

Boolean
ByteOutputStream::write_octet_array_mb(const ACE_Message_Block* mb) {
    // If the buffer is small and it fits in the current packet
    // block it is be cheaper just to copy the buffer.    
    for (const ACE_Message_Block* i = mb;
            i != 0;
            i = i->cont()) {       
        const size_t length = i->length();

        // If the mb does not own its data we are forced to make a copy.
        if (ACE_BIT_ENABLED(i->flags(),
                ACE_Message_Block::DONT_DELETE)) {
            if (!this->write_array(i->rd_ptr(),
                    OCTET_SIZE,
                    OCTET_ALIGN,
                    static_cast<UInt> (length))) {               
                return (this->good_bit_ = false);
            }            
            continue;
        }        
        if (length < this->memcpy_tradeoff_
                && this->current_->wr_ptr() + length < this->current_->end()) {
            if (!this->write_array(i->rd_ptr(),
                    OCTET_SIZE,
                    OCTET_ALIGN,
                    static_cast<UInt> (length))) {         
                return (this->good_bit_ = false);
            }

            continue;
        }        
        ACE_Message_Block* cont = 0;
        this->good_bit_ = false;
        ACE_NEW_RETURN(cont,
                ACE_Message_Block(i->data_block()->duplicate()),
                false);
        this->good_bit_ = true;

        if (this->current_->cont() != 0) {         
            ACE_Message_Block::release(this->current_->cont());
        }
        cont->rd_ptr(i->rd_ptr());
        cont->wr_ptr(i->wr_ptr());

        this->current_->cont(cont);
        this->current_ = cont;
        this->current_is_writable_ = false;        
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
        this->current_alignment_ =
                (this->current_alignment_ + cont->length()) % MAX_ALIGNMENT;
#endif /* ACE_LACKS_CDR_ALIGNMENT */
    }    
    return true;
}

Boolean
ByteOutputStream::write_boolean_array(const Boolean* x,
        UInt length) {
    // It is hard to optimize this, the spec requires that on the wire
    // booleans be represented as a byte with value 0 or 1, but in
    // memory it is possible (though very unlikely) that a boolean has
    // a non-zero value (different from 1).
    // We resort to a simple loop.
    Boolean const * const end = x + length;

    for (Boolean const * i = x;
            i != end && this->good_bit();
            ++i)
        (void) this->write_boolean(*i);

    return this->good_bit();
}

void
ByteOutputStream::reset(void) {
    this->current_ = &this->start_;
    this->current_is_writable_ = true;
    RDR::mb_align(&this->start_);

#if !defined (ACE_LACKS_CDR_ALIGNMENT)
    this->current_alignment_ = 0;
#endif /* ACE_LACKS_CDR_ALIGNMENT */

    // It is tempting not to remove the memory, but we need to do so to
    // release any potential user buffers chained in the continuation
    // field.

    ACE_Message_Block * const cont = this->start_.cont();
    if (cont) {
        ACE_Message_Block::release(cont);
        this->start_.cont(0);
    }
}

size_t
ByteOutputStream::total_length(void) const {
    return RDR::total_length(this->begin(), this->end());
}
