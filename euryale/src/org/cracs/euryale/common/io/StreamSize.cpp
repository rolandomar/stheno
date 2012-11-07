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

#include <ace/SString.h>
#include <ace/OS_Memory.h>
#include <ace/Truncate.h>
#include <euryale/common/io/StreamSize.h>
#include <euryale/serialization/Serializable.h>

#if !defined (__ACE_INLINE__)
#include <ace/StreamSize.inl>
#endif /* ! __ACE_INLINE__ */
//    virtual Boolean write_serializable_list(const list<Serializable*>& l);

Boolean StreamSize::write_serializable_list(const list<Serializable *>& l) {
    list<Serializable*>::const_iterator iter = l.begin();
    while (iter != l.end()) {
        StreamSize sz;
        size_ += (*iter)->getSerializationSize(sz);
        iter++;
    }
    return true;
}

Boolean
StreamSize::write_wchar(WChar x) {
    // Note: translator framework is not supported.
    //
    if (RDR::wchar_maxbytes() == 0) {
        errno = EACCES;
        return (this->good_bit_ = false);
    }

    if (static_cast<Short> (major_version_) == 1
            && static_cast<Short> (minor_version_) == 2) {
        Octet len =
                static_cast<Octet> (RDR::wchar_maxbytes());

        if (this->write_1(&len)) {
            if (RDR::wchar_maxbytes() == sizeof (WChar)) {
                return
                this->write_octet_array(
                        reinterpret_cast<const Octet*> (&x),
                        static_cast<ULong> (len));
            } else {
                if (RDR::wchar_maxbytes() == 2) {
                    Short sx = static_cast<Short> (x);
                    return
                    this->write_octet_array(
                            reinterpret_cast<const Octet*> (&sx),
                            static_cast<ULong> (len));
                } else {
                    Octet ox = static_cast<Octet> (x);
                    return
                    this->write_octet_array(
                            reinterpret_cast<const Octet*> (&ox),
                            static_cast<ULong> (len));
                }
            }
        }
    } else if (static_cast<Short> (minor_version_) == 0) { // wchar is not allowed with GIOP 1.0.
        errno = EINVAL;
        return (this->good_bit_ = false);
    }

    if (RDR::wchar_maxbytes() == sizeof (WChar)) {
        const void *temp = &x;
        return this->write_4(reinterpret_cast<const ULong *> (temp));
    } else if (RDR::wchar_maxbytes() == 2) {
        Short sx = static_cast<Short> (x);
        return this->write_2(reinterpret_cast<const UShort *> (&sx));
    }

    Octet ox = static_cast<Octet> (x);
    return this->write_1(reinterpret_cast<const Octet *> (&ox));
}

Boolean
StreamSize::write_string(ULong len,
        const Char *x) {
    // Note: translator framework is not supported.
    //
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
StreamSize::write_string(const ACE_CString &x) {
    // @@ Leave this method in here, not the `.i' file so that we don't
    //    have to unnecessarily pull in the `ace/SString.h' header.
    return this->write_string(static_cast<ULong> (x.length()),
            x.c_str());
}

Boolean
StreamSize::write_wstring(ULong len,
        const WChar *x) {
    // Note: translator framework is not supported.
    //
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
            Boolean good_ulong =
                    this->write_ulong(
                    ACE_Utils::truncate_cast<ULong > (
                    RDR::wchar_maxbytes() * len));

            if (good_ulong) {
                return this->write_wchar_array(x, len);
            }
        } else {
            //In GIOP 1.2 zero length wstrings are legal
            return this->write_ulong(0);
        }
    }
    else
        if (x != 0) {
        if (this->write_ulong(len + 1))
            return this->write_wchar_array(x, len + 1);
    } else if (this->write_ulong(1))
        return this->write_wchar(0);
    return (this->good_bit_ = false);
}

Boolean
StreamSize::write_1(const Octet *) {
    this->adjust(1);
    return true;
}

Boolean
StreamSize::write_2(const UShort *) {
    this->adjust(SHORT_SIZE);
    return true;
}

Boolean
StreamSize::write_4(const ULong *) {
    this->adjust(LONG_SIZE);
    return true;
}

Boolean
StreamSize::write_8(const ULongLong *) {
    this->adjust(LONGLONG_SIZE);
    return true;
}

Boolean
StreamSize::write_16(const LongDouble *) {
    this->adjust(LONGDOUBLE_SIZE,
            LONGDOUBLE_ALIGN);
    return true;
}

Boolean
StreamSize::write_wchar_array_i(const WChar *,
        ULong length) {
    if (length == 0)
        return true;

    size_t const align = (RDR::wchar_maxbytes() == 2) ?
            SHORT_ALIGN :
            OCTET_ALIGN;

    this->adjust(RDR::wchar_maxbytes() * length, align);
    return true;
}

Boolean
StreamSize::write_array(const void *,
        size_t size,
        size_t align,
        ULong length) {
    if (length == 0)
        return true;

    this->adjust(size * length, align);
    return true;
}

Boolean
StreamSize::write_boolean_array(const Boolean*,
        ULong length) {
    this->adjust(length, 1);
    return true;
}

void
StreamSize::adjust(size_t size) {
    adjust(size, size);
}

void
StreamSize::adjust(size_t size,
        size_t align) {
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
    const size_t offset = ACE_align_binary(size_, align) - size_;
    size_ += offset;
#endif /* ACE_LACKS_CDR_ALIGNMENT */
    size_ += size;
}

Boolean
operator<<(StreamSize &ss, const ACE_CString &x) {
    ss.write_string(x);
    return ss.good_bit();
}

//ACE_END_VERSIONED_NAMESPACE_DECL
