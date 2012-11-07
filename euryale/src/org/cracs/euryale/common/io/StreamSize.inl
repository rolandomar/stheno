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
// -*- C++ -*-
//
// $Id: CDR_Size.inl 80826 2008-03-04 14:51:23Z wotte $

#include <ace/OS_NS_string.h>
//ACE_BEGIN_VERSIONED_NAMESPACE_DECL

ACE_INLINE
StreamSize::StreamSize (Octet major_version,
                          Octet minor_version)
    : good_bit_ (true),
      size_ (0),
      major_version_ (major_version),
      minor_version_ (minor_version)
{
}

ACE_INLINE bool
StreamSize::good_bit (void) const
{
  return this->good_bit_;
}

ACE_INLINE void
StreamSize::reset (void)
{
  this->size_ = 0;
}

ACE_INLINE size_t
StreamSize::total_length (void) const
{
  return this->size_;
}


// Encode the CDR stream.

ACE_INLINE Boolean
StreamSize::write_octet (Octet x)
{
  return this->write_1 (reinterpret_cast<const Octet *> (&x));
}

ACE_INLINE Boolean
StreamSize::write_boolean (Boolean x)
{
  return (Boolean) this->write_octet (x ? (Octet) 1 : (Octet) 0);
}

ACE_INLINE Boolean
StreamSize::write_char (Char x)
{
  // Note: translator framework is not supported.
  //
  return this->write_1 (reinterpret_cast<const Octet*> (&x));
}

ACE_INLINE Boolean
StreamSize::write_short (Short x)
{
  return this->write_2 (reinterpret_cast<const UShort*> (&x));
}

ACE_INLINE Boolean
StreamSize::write_ushort (UShort x)
{
  return this->write_2 (reinterpret_cast<const UShort*> (&x));
}

ACE_INLINE Boolean
StreamSize::write_long (Long x)
{
  return this->write_4 (reinterpret_cast<const ULong*> (&x));
}

ACE_INLINE Boolean
StreamSize::write_ulong (ULong x)
{
  return this->write_4 (reinterpret_cast<const ULong*> (&x));
}

ACE_INLINE Boolean
StreamSize::write_longlong (const LongLong &x)
{
  return this->write_8 (reinterpret_cast<const ULongLong*> (&x));
}

ACE_INLINE Boolean
StreamSize::write_ulonglong (const ULongLong &x)
{
  const void *temp = &x;
  return this->write_8 (reinterpret_cast<const ULongLong *> (temp));
}

ACE_INLINE Boolean
StreamSize::write_float (Float x)
{
  const void *temp = &x;
  return this->write_4 (reinterpret_cast<const ULong *> (temp));
}

ACE_INLINE Boolean
StreamSize::write_double (const Double &x)
{
  const void *temp = &x;
  return this->write_8 (reinterpret_cast<const ULongLong*> (temp));
}

ACE_INLINE Boolean
StreamSize::write_longdouble (const LongDouble &x)
{
  const void *temp = &x;
  return this->write_16 (reinterpret_cast<const LongDouble*> (temp));
}

ACE_INLINE Boolean
StreamSize::write_string (const Char *x)
{
  if (x != 0)
    {
      const ULong len =
        static_cast<ULong> (ACE_OS::strlen (x));
      return this->write_string (len, x);
    }
  return this->write_string (0, 0);
}

ACE_INLINE Boolean
StreamSize::write_wstring (const WChar *x)
{
  if (x != 0)
    {
      ULong len =
        static_cast<ULong> (ACE_OS::strlen (x));
      return this->write_wstring (len, x);
    }
  return this->write_wstring (0, 0);
}

ACE_INLINE Boolean
StreamSize::write_char_array (const Char *x,
                                 ULong length)
{
  // Note: translator framework is not supported.
  //
  return this->write_array (x,
                            OCTET_SIZE,
                            OCTET_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_wchar_array (const WChar* x,
                                  ULong length)
{
  // Note: translator framework is not supported.
  //
  if (RDR::wchar_maxbytes () == 0)
    {
      errno = EACCES;
      return (Boolean) (this->good_bit_ = false);
    }
  if (RDR::wchar_maxbytes () == sizeof (WChar))
    return this->write_array (x,
                              sizeof (WChar),
                              sizeof (WChar) == 2
                              ? SHORT_ALIGN
                              : LONG_ALIGN,
                              length);
  return this->write_wchar_array_i (x,length);
}

ACE_INLINE Boolean
StreamSize::write_octet_array (const Octet* x,
                                  ULong length)
{
  return this->write_array (x,
                            OCTET_SIZE,
                            OCTET_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_short_array (const Short *x,
                                  ULong length)
{
  return this->write_array (x,
                            SHORT_SIZE,
                            SHORT_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_ushort_array (const UShort *x,
                                 ULong length)
{
  return this->write_array (x,
                            SHORT_SIZE,
                            SHORT_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_long_array (const Long *x,
                               ULong length)
{
  return this->write_array (x,
                            LONG_SIZE,
                            LONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_ulong_array (const ULong *x,
                                ULong length)
{
  return this->write_array (x,
                            LONG_SIZE,
                            LONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_longlong_array (const LongLong *x,
                                     ULong length)
{
  return this->write_array (x,
                            LONGLONG_SIZE,
                            LONGLONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_ulonglong_array (const ULongLong *x,
                                      ULong length)
{
  return this->write_array (x,
                            LONGLONG_SIZE,
                            LONGLONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_float_array (const Float *x,
                                  ULong length)
{
  return this->write_array (x,
                            LONG_SIZE,
                            LONG_ALIGN,
                            length);
}


ACE_INLINE Boolean
StreamSize::write_double_array (const Double *x,
                                   ULong length)
{
  return this->write_array (x,
                            LONGLONG_SIZE,
                            LONGLONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
StreamSize::write_longdouble_array (const LongDouble* x,
                                       ULong length)
{
  return this->write_array (x,
                            LONGDOUBLE_SIZE,
                            LONGDOUBLE_ALIGN,
                            length);
}


// ****************************************************************



//ACE_END_VERSIONED_NAMESPACE_DECL
