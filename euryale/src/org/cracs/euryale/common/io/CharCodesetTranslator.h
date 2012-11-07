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
 * File:   CharCodesetTranslator.h
 * Author: rmartins
 *
 * Created on January 13, 2010, 1:15 PM
 */

#ifndef _CHARCODESETTRANSLATOR_H
#define	_CHARCODESETTRANSLATOR_H

#include <euryale/Euryale_export.h>
#include <euryale/Types.h>
#include <euryale/common/io/ByteStream.h>
class ByteInputStream;
class ByteOutputStream;
class Euryale_Export CharCodesetTranslator {
public:
    virtual ~CharCodesetTranslator();

    /// Read a single character from the stream, converting from the
    /// stream codeset to the native codeset
    virtual Boolean read_char(ByteInputStream&, Char&) = 0;

    /// Read a string from the stream, including the length, converting
    /// the characters from the stream codeset to the native codeset
    virtual Boolean read_string(ByteInputStream&, Char *&) = 0;

    /// Read an array of characters from the stream, converting the
    /// characters from the stream codeset to the native codeset.
    virtual Boolean read_char_array(ByteInputStream&, Char*,
            UInt) = 0;

    /// Write a single character to the stream, converting from the
    /// native codeset to the stream codeset
    virtual Boolean write_char(ByteOutputStream&, Char) = 0;

    /// Write a string to the stream, including the length, converting
    /// from the native codeset to the stream codeset
    virtual Boolean write_string(ByteOutputStream&, UInt,
            const Char*) = 0;

    /// Write an array of characters to the stream, converting from the
    /// native codeset to the stream codeset
    virtual Boolean write_char_array(ByteOutputStream&, const Char*,
            UInt) = 0;

    virtual UInt ncs() = 0;
    virtual UInt tcs() = 0;
protected:
    /// Children have access to low-level routines because they cannot
    /// use read_char or something similar (it would recurse).
    Boolean read_1(ByteInputStream& input, Octet *x);
    Boolean write_1(ByteOutputStream& output, const Octet *x);

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

#endif	/* _CHARCODESETTRANSLATOR_H */

