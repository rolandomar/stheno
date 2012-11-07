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
 * File:   WCharCodesetTranslator.cpp
 * Author: rmartins
 * 
 * Created on January 13, 2010, 1:16 PM
 */

#include "WCharCodesetTranslator.h"
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>

ACE_INLINE Boolean
WCharCodesetTranslator::read_1 (ByteInputStream& input,
                                      Octet *x)
{
  return input.read_1 (x);
}

ACE_INLINE Boolean
WCharCodesetTranslator::read_2 (ByteInputStream& input,
                                      UShort *x)
{
  return input.read_2 (x);
}

ACE_INLINE Boolean
WCharCodesetTranslator::read_4 (ByteInputStream& input,
                                      UInt *x)
{
  return input.read_4 (x);
}

ACE_INLINE Boolean
WCharCodesetTranslator::write_1 (ByteOutputStream& output,
                                       const Octet *x)
{
  return output.write_1 (x);
}

ACE_INLINE Boolean
WCharCodesetTranslator::write_2 (ByteOutputStream& output,
                                       const UShort *x)
{
  return output.write_2 (x);
}

ACE_INLINE Boolean
WCharCodesetTranslator::write_4 (ByteOutputStream& output,
                                       const UInt *x)
{
  return output.write_4 (x);
}

ACE_INLINE Boolean
WCharCodesetTranslator::read_array (ByteInputStream& in,
                                          void* x,
                                          size_t size,
                                          size_t align,
                                          UInt length)
{
  return in.read_array (x, size, align, length);
}

ACE_INLINE Boolean
WCharCodesetTranslator::write_array (ByteOutputStream& out,
                                           const void *x,
                                           size_t size,
                                           size_t align,
                                           UInt length)
{
  return out.write_array(x, size, align, length);
}

ACE_INLINE int
WCharCodesetTranslator::adjust (ByteOutputStream& out,
                                      size_t size,
                                      size_t align,
                                      char *&buf)
{
  return out.adjust(size, align, buf);
}

ACE_INLINE void
WCharCodesetTranslator::good_bit (ByteOutputStream& out, bool bit)
{
  out.good_bit_ = bit;
}

ACE_INLINE Octet
WCharCodesetTranslator::major_version (ByteInputStream& input)
{
  return input.major_version_;
}

ACE_INLINE Octet
WCharCodesetTranslator::minor_version (ByteInputStream& input)
{
  return input.minor_version_;
}

ACE_INLINE Octet
WCharCodesetTranslator::major_version (ByteOutputStream& output)
{
  return output.major_version_;
}

ACE_INLINE Octet
WCharCodesetTranslator::minor_version (ByteOutputStream& output)
{
  return output.minor_version_;
}


