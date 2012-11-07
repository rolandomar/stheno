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
#ifndef INPUTSTREAM_H_
#define INPUTSTREAM_H_
#include <ace/Log_Msg.h>
#include <euryale/common/Object.h>
#include <euryale/Types.h>
#include <list>
using std::list;


//class Object;

class InputStream : public Object {
public:

    InputStream();
    virtual ~InputStream();
    
    virtual const String& toString();

    virtual const char* objectID() const;

    virtual Boolean read_boolean(Boolean& x) = 0;
    virtual Boolean read_char(Char &x) = 0;
    virtual Boolean read_wchar(WChar& x) = 0;
    virtual Boolean read_octet(Octet& x) = 0;
    virtual Boolean read_short(Short &x) = 0;
    virtual Boolean read_ushort(UShort &x) = 0;
    virtual Boolean read_long(Int &x) = 0;
    virtual Boolean read_ulong(UInt &x) = 0;
    virtual Boolean read_longlong(LongLong& x) = 0;
    virtual Boolean read_ulonglong(ULongLong& x) = 0;
    virtual Boolean read_float(Float &x) = 0;
    virtual Boolean read_double(Double &x) = 0;
    virtual Boolean read_longdouble(LongDouble &x) = 0;
    virtual Boolean read_string(Char *&x) = 0;
    virtual Boolean read_string(ACE_CString &x) = 0;
    virtual Boolean read_wstring(WChar*& x) = 0;

    virtual Boolean
    read_boolean_array(Boolean* x, UInt length) = 0;
    virtual Boolean read_char_array(Char *x, UInt length) = 0;
    virtual Boolean read_wchar_array(WChar* x, UInt length) = 0;
    virtual Boolean read_octet_array(Octet* x, UInt length) = 0;
    virtual Boolean read_short_array(Short *x, UInt length) = 0;
    virtual Boolean
    read_ushort_array(UShort *x, UInt length) = 0;
    virtual Boolean read_long_array(Int *x, UInt length) = 0;
    virtual Boolean read_ulong_array(UInt *x, UInt length) = 0;
    virtual Boolean
    read_longlong_array(LongLong* x, UInt length) = 0;
    virtual Boolean read_ulonglong_array(ULong* x,
            UInt length) = 0;
    virtual Boolean read_float_array(Float *x, UInt length) = 0;
    virtual Boolean
    read_double_array(Double *x, UInt length) = 0;
    virtual Boolean read_longdouble_array(LongDouble* x,
            UInt length) = 0;

    virtual size_t length(void) const = 0;

    virtual const ACE_Message_Block* start(void) const = 0;
    

    virtual Boolean read_boolean_list(list<Boolean>& l);

    virtual Boolean read_char_list(list<Char>& l);

    virtual Boolean read_wchar_list(list<WChar>& l);

    virtual Boolean read_octet_list(list<Octet>& l);

    virtual Boolean read_short_list(list<Short>& l);

    virtual Boolean read_ushort_list(list<UShort>& l);

    virtual Boolean read_long_list(list<Int>& l);

    virtual Boolean read_ulong_list(list<UInt>& l);

    virtual Boolean read_longlong_list(list<LongLong>& l);

    virtual Boolean read_ulonglong_list(list<ULong> &l);

    virtual Boolean read_float_list(list<Float>& l);

    virtual Boolean read_double_list(list<Double> &l);

    virtual Boolean read_longdouble_list(list<LongDouble> &l);
    /// For string we offer methods that accept a precomputed length.

    virtual Boolean read_string_list(list<Char*> &l);

    virtual Boolean read_string_list(list<ACE_CString> &l);

    virtual Boolean read_wstring_list(list<WChar*> &l);
    
    template <typename X> Boolean read_list(list<X*>& l) {
    UInt size = 0;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)InputStream::read_list size\n")));
    read_ulong(size);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)InputStream::read_list size=%d\n"), size));
    for (int i = 0; i < size; i++) {
        X* x = new X();
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)InputStream::read_list item...\n")));
        x->deserialize(*this);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)InputStream::read_list after item...\n")));
        l.push_back(x);
    }
    return true;
}

};


#endif /*INPUTSTREAM_H_*/
