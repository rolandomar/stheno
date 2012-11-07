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
 * File:   WCharCodesetTranslator.h
 * Author: rmartins
 *
 * Created on January 13, 2010, 1:16 PM
 */

#ifndef _WCHARCODESETTRANSLATOR_H
#define	_WCHARCODESETTRANSLATOR_H

#include <euryale/Euryale_export.h>
#include <euryale/Types.h>
#include <euryale/common/io/ByteStream.h>

class ByteInputStream;
class ByteOutputStream;
class Euryale_Export WCharCodesetTranslator {
public:
    virtual ~WCharCodesetTranslator();

    virtual Boolean read_wchar(ByteInputStream&, WChar&) = 0;
    virtual Boolean read_wstring(ByteInputStream&, WChar *&) = 0;
    virtual Boolean read_wchar_array(ByteInputStream&, WChar*,
            UInt) = 0;
    virtual Boolean write_wchar(ByteOutputStream&, WChar) = 0;
    virtual Boolean write_wstring(ByteOutputStream&, UInt,
            const WChar*) = 0;
    virtual Boolean write_wchar_array(ByteOutputStream&, const WChar*,
            UInt) = 0;

    virtual UInt ncs() = 0;
    virtual UInt tcs() = 0;
protected:
    /// Children have access to low-level routines because they cannot
    /// use read_char or something similar (it would recurse).
    Boolean read_1(ByteInputStream& input, Octet *x);
    Boolean read_2(ByteInputStream& input, UShort *x);
    Boolean read_4(ByteInputStream& input, UInt *x);
    Boolean write_1(ByteOutputStream& output, const Octet *x);
    Boolean write_2(ByteOutputStream& output, const UShort *x);
    Boolean write_4(ByteOutputStream& output, const UInt *x);

    /// Efficiently read @a length elements of size @a size each from
    /// <input> into <x>; the data must be aligned to <align>.
    Boolean read_array(ByteInputStream& input, void* x, size_t size,
            size_t align, UInt length);

    /**
     * Efficiently write @a length elements of size @a size from <x> into
     * <output>. Before inserting the elements enough padding is added
     * to ensure that the elements will be aligned to <align> in the
     * stream.
     */
    Boolean write_array(ByteOutputStream& output, const void *x,
            size_t size, size_t align, UInt length);

    /**
     * Exposes the stream implementation of <adjust>, this is useful in
     * many cases to minimize memory allocations during marshaling.
     * On success @a buf will contain a contiguous area in the CDR stream
     * that can hold @a size bytes aligned to <align>.
     * Results
     */
    int adjust(ByteOutputStream& out, size_t size, size_t align, char *&buf);

    /// Used by derived classes to set errors in the CDR stream.
    void good_bit(ByteOutputStream& out, bool bit);

    /// Obtain the CDR Stream's major & minor version values.
    Octet major_version(ByteInputStream& input);
    Octet minor_version(ByteInputStream& input);
    Octet major_version(ByteOutputStream& output);
    Octet minor_version(ByteOutputStream& output);

};


#endif	/* _WCHARCODESETTRANSLATOR_H */

