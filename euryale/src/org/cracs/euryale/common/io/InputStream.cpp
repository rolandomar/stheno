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

#include <euryale/common/io/InputStream.h>
#include <euryale/common/Object.h>

InputStream::InputStream() {
}

InputStream::~InputStream() {
}

const String& InputStream::toString() {
    if (m_toString == 0) {
        char* buffer;
        ACE_NEW_NORETURN(buffer, char[30]);
        ACE_OS::sprintf(buffer, "InputStream(%p)", this);
        m_toString = new String(buffer);
    }
    return *m_toString;
}

const char* InputStream::objectID() const {
    return "f32c2d0ebea180f16f4dc52f669b6aa7";
}

Boolean InputStream::read_boolean_list(list<Boolean>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        Boolean x;
        read_boolean(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_char_list(list<Char>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        Char x;
        read_char(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_wchar_list(list<WChar>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        WChar x;
        read_wchar(x);
        l.push_back(x);
    }
    return true;

}

Boolean InputStream::read_octet_list(list<Octet>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        Octet x;
        read_octet(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_short_list(list<Short>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        Short x;
        read_short(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_ushort_list(list<UShort>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        UShort x;
        read_ushort(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_long_list(list<Int>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        Int x;
        read_long(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_ulong_list(list<UInt>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        UInt x;
        read_ulong(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_longlong_list(list<LongLong>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        LongLong x;
        read_longlong(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_ulonglong_list(list<ULong> &l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        ULong x;
        read_ulong(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_float_list(list<Float>& l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        Float x;
        read_float(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_double_list(list<Double> &l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        Double x;
        read_double(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_longdouble_list(list<LongDouble> &l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        LongDouble x;
        read_longdouble(x);
        l.push_back(x);
    }
    return true;
}

/// For string we offer methods that accept a precomputed length.

Boolean InputStream::read_string_list(list<Char*> &l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        Char* x;
        read_string(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_string_list(list<ACE_CString> &l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        ACE_CString x;
        read_string(x);
        l.push_back(x);
    }
    return true;
}

Boolean InputStream::read_wstring_list(list<WChar*> &l) {
    UInt size = 0;
    read_ulong(size);
    for (int i = 0; i < size; i++) {
        WChar* x;
        read_wstring(x);
        l.push_back(x);
    }
    return true;
}
