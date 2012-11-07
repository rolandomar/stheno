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
#ifndef OUTPUTSTREAM_H_
#define OUTPUTSTREAM_H_

#include <euryale/common/Object.h>
#include <euryale/Types.h>

//#include <euryale/serialization/Serializable.h
class Serializable;
#include <list>
using std::list;

class OutputStream : public Object {
public:

    OutputStream() {
    }

    virtual ~OutputStream() {
    }

    virtual const String& toString() {
        if (m_toString == 0) {
            char* buffer;
            ACE_NEW_NORETURN(buffer, char[30]);
            ACE_OS::sprintf(buffer, "OutputStream(%p)", this);
            m_toString = new String(buffer);
        }
        return *m_toString;
    }

    virtual const char* objectID() const {
        return "e6d71b09086d65624669159c92b29e24";
    }

    // Return 0 on failure and 1 on success.
    //@{ @name Write operations
    virtual Boolean write_boolean(Boolean x) = 0;
    virtual Boolean write_char(Char x) = 0;
    virtual Boolean write_wchar(WChar x) = 0;
    virtual Boolean write_octet(Octet x) = 0;
    virtual Boolean write_short(Short x) = 0;
    virtual Boolean write_ushort(UShort x) = 0;
    virtual Boolean write_long(Long x) = 0;
    virtual Boolean write_ulong(ULong x) = 0;
    virtual Boolean write_longlong(const LongLong &x) = 0;
    virtual Boolean write_ulonglong(const ULongLong &x) = 0;
    virtual Boolean write_float(Float x) = 0;
    virtual Boolean write_double(const Double &x) = 0;
    virtual Boolean write_longdouble(const LongDouble &x) = 0;

    /// For string we offer methods that accept a precomputed length.
    virtual Boolean write_string(const Char *x) = 0;
    virtual Boolean write_string(UInt len, const Char *x) = 0;
    virtual Boolean write_string(const ACE_CString &x) = 0;
    virtual Boolean write_wstring(const WChar *x) = 0;
    virtual Boolean write_wstring(UInt length, const WChar *x) = 0;
    //@}

    /// @note the portion written starts at <x> and ends
    ///    at <x + length>.
    /// The length is *NOT* stored into the CDR stream.
    //@{ @name Array write operations
    virtual Boolean write_boolean_array(const Boolean *x,
            UInt length) = 0;
    virtual Boolean
    write_char_array(const Char *x, UInt length) = 0;
    virtual Boolean write_wchar_array(const WChar* x,
            UInt length) = 0;
    virtual Boolean write_octet_array(const Octet* x,
            UInt length) = 0;
    virtual Boolean write_short_array(const Short *x,
            UInt length) = 0;
    virtual Boolean write_ushort_array(const UShort *x,
            UInt length) = 0;
    virtual Boolean
    write_long_array(const Int *x, UInt length) = 0;
    virtual Boolean write_ulong_array(const UInt *x,
            UInt length) = 0;
    virtual Boolean write_longlong_array(const LongLong* x,
            UInt length) = 0;
    virtual Boolean write_ulonglong_array(const ULongLong *x,
            UInt length) = 0;
    virtual Boolean write_float_array(const Float *x,
            UInt length) = 0;
    virtual Boolean write_double_array(const Double *x,
            UInt length) = 0;
    virtual Boolean write_longdouble_array(const LongDouble* x,
            UInt length) = 0;

    template <typename X> Boolean write_list(list<X*>& l) {
        typename list<X*>::const_iterator iter = l.begin();
        write_ulong(l.size());
        while (iter != l.end()) {
            (*iter)->serialize(*this);
            iter++;
        }
        return true;
    }

    virtual Boolean write_boolean_list(list<Boolean> x) {
        list<Boolean>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_boolean(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_char_list(list<Char> x) {
        list<Char>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_char(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_wchar_list(list<WChar> x) {
        list<WChar>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_wchar(*iter);
            iter++;
        }
        return true;

    }

    virtual Boolean write_octet_list(list<Octet> x) {
        list<Octet>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_octet(*iter);
            iter++;
        }
        return true;

    }

    virtual Boolean write_short_list(list<Short> x) {
        list<Short>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_short(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_ushort_list(list<UShort> x) {
        list<UShort>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_ushort(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_long_list(list<Int> x) {
        list<Int>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_long(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_ulong_list(list<UInt> x) {
        list<UInt>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_ulong(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_longlong_list(list<LongLong> &x) {
        list<LongLong>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_longlong(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_ulonglong_list(list<ULong> &x) {
        list<ULong>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_ulonglong(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_float_list(list<Float> x) {
        list<Float>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_float(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_double_list(list<Double> &x) {
        list<Double>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_double(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_longdouble_list(list<LongDouble> &x) {
        list<LongDouble>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_longdouble(*iter);
            iter++;
        }
        return true;
    }

    /// For string we offer methods that accept a precomputed length.

    virtual Boolean write_string_list(list<Char*> &x) {
        list<Char*>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_string(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_string_list(list<ACE_CString> &x) {
        list<ACE_CString>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_string(*iter);
            iter++;
        }
        return true;
    }

    virtual Boolean write_wstring_list(list<WChar*> &x) {
        list<WChar*>::const_iterator iter = x.begin();
        write_ulong(x.size());
        while (iter != x.end()) {
            write_wstring((const WChar*) (*iter));
            iter++;
        }
        return true;
    }



    virtual Boolean write_serializable_list(const list<Serializable*>& l) = 0;

    /// Write an octet array contained inside a MB, this can be optimized
    /// to minimize copies.
    virtual Boolean write_octet_array_mb(const ACE_Message_Block* mb) = 0;
    //@}


    /*virtual int consolidate() = 0;

    virtual const ACE_Message_Block *begin(void) const = 0;

    virtual const ACE_Message_Block *current(void) const = 0;

    virtual ACE_Message_Block* release(void);

    virtual Boolean replace(Int x, char* loc) = 0;*/

    virtual size_t total_length(void) const = 0;
};


#endif /*OUTPUTSTREAM_H_*/
