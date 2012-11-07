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
//

#include "RDRStream.h"

/*
 *  Adapted from CDR_Stream.cpp by Rolando Martins <rolando.martins@gmail.com>
 */

//#include "ace/OS_NS_string.h"
//#include "ace/OS_Memory.h"
//#include <ace/Message_Block.h>


// ****************************************************************

// implementing the special types
ACE_INLINE
RDROutputStream::from_boolean::from_boolean (Boolean b)
  : val_ (b)
{
}

ACE_INLINE
RDRInputStream::to_boolean::to_boolean (Boolean &b)
  : ref_ (b)
{
}

ACE_INLINE
RDROutputStream::from_octet::from_octet (Octet o)
  : val_ (o)
{
}

ACE_INLINE
RDRInputStream::to_octet::to_octet (Octet &o)
  : ref_ (o)
{
}

ACE_INLINE
RDROutputStream::from_char::from_char (Char c)
  : val_ (c)
{
}

ACE_INLINE
RDRInputStream::to_char::to_char (Char &c)
  : ref_ (c)
{
}

ACE_INLINE
RDROutputStream::from_wchar::from_wchar (WChar wc)
  : val_ (wc)
{
}

ACE_INLINE
RDRInputStream::to_wchar::to_wchar (WChar &wc)
  : ref_ (wc)
{
}

ACE_INLINE
RDROutputStream::from_string::from_string (Char *s,
                                         UInt b,
                                         Boolean nocopy)
  : val_ (s),
    bound_ (b),
    nocopy_ (nocopy)
{
}

ACE_INLINE
RDROutputStream::from_string::from_string (const Char *s,
                                         UInt b,
                                         Boolean nocopy)
  : val_ (const_cast<Char *> (s)),
    bound_ (b),
    nocopy_ (nocopy)
{
}

ACE_INLINE
RDRInputStream::to_string::to_string (Char *&s,
                                    UInt b)
  : val_ (const_cast<const Char *&> (s)),
    bound_ (b)
{
}

ACE_INLINE
RDRInputStream::to_string::to_string (const Char *&s,
                                    UInt b)
  : val_ (s),
    bound_ (b)
{
}

ACE_INLINE
RDROutputStream::from_wstring::from_wstring (WChar *ws,
                                           UInt b,
                                           Boolean nocopy)
  : val_ (ws),
    bound_ (b),
    nocopy_ (nocopy)
{
}

ACE_INLINE
RDROutputStream::from_wstring::from_wstring (const WChar *ws,
                                           UInt b,
                                           Boolean nocopy)
  : val_ (const_cast<WChar *> (ws)),
    bound_ (b),
    nocopy_ (nocopy)
{
}

ACE_INLINE
RDRInputStream::to_wstring::to_wstring (WChar *&ws,
                                      UInt b)
  : val_ (const_cast<const WChar *&> (ws)),
    bound_ (b)
{
}

ACE_INLINE
RDRInputStream::to_wstring::to_wstring (const WChar *&ws,
                                      UInt b)
  : val_ (ws),
    bound_ (b)
{
}

ACE_INLINE
RDRInputStream::Transfer_Contents::Transfer_Contents (RDRInputStream &rhs)
  :  rhs_ (rhs)
{
}

// ****************************************************************

ACE_INLINE
RDROutputStream::~RDROutputStream (void)
{
  if (this->start_.cont () != 0)
    {
      ACE_Message_Block::release (this->start_.cont ());
      this->start_.cont (0);
    }
  this->current_ = 0;
}

ACE_INLINE void
RDROutputStream::reset (void)
{
  this->current_ = &this->start_;
  this->current_is_writable_ = true;
  RDR::mb_align (&this->start_);

#if !defined (ACE_LACKS_CDR_ALIGNMENT)
  this->current_alignment_ = 0;
#endif /* ACE_LACKS_CDR_ALIGNMENT */

  // It is tempting not to remove the memory, but we need to do so to
  // release any potential user buffers chained in the continuation
  // field.

  ACE_Message_Block * const cont = this->start_.cont ();
  if (cont)
    {
      ACE_Message_Block::release (cont);
      this->start_.cont (0);
    }
}

// Encode the CDR stream.

ACE_INLINE Boolean
RDROutputStream::write_octet (Octet x)
{
  return this->write_1 (&x);
}

ACE_INLINE Boolean
RDROutputStream::write_boolean (Boolean x)
{
  return
    static_cast<Boolean> (
      this->write_octet (
        x
        ? static_cast<Octet> (1)
        : static_cast<Octet> (0)));
}

ACE_INLINE Boolean
RDROutputStream::write_char (Char x)
{
  if (this->char_translator_ == 0)
    {
      Octet temp = static_cast<Octet> (x);
      return this->write_1 (&temp);
    }
  return this->char_translator_->write_char (*this, x);
}

ACE_INLINE Boolean
RDROutputStream::write_short (Short x)
{
  UShort temp = static_cast<UShort> (x);
  return this->write_2 (&temp);
}

ACE_INLINE Boolean
RDROutputStream::write_ushort (UShort x)
{
  return this->write_2 (&x);
}

ACE_INLINE Boolean
RDROutputStream::write_long (Int x)
{
  UInt temp = static_cast<UInt> (x);
  return this->write_4 (&temp);
}

ACE_INLINE Boolean
RDROutputStream::write_ulong (UInt x)
{
  return this->write_4 (&x);
}

ACE_INLINE Boolean
RDROutputStream::write_longlong (const LongLong &x)
{
  void const * const temp = &x;
  return this->write_8 (reinterpret_cast<ULongLong const *> (temp));
}

ACE_INLINE Boolean
RDROutputStream::write_ulonglong (const ULongLong &x)
{
  return this->write_8 (&x);
}

ACE_INLINE Boolean
RDROutputStream::write_float (Float x)
{
  void const * const temp = &x;
  return this->write_4 (reinterpret_cast<UInt const *> (temp));
}

ACE_INLINE Boolean
RDROutputStream::write_double (const Double &x)
{
  void const * const temp = &x;
  return this->write_8 (reinterpret_cast<ULongLong const *> (temp));
}

ACE_INLINE Boolean
RDROutputStream::write_longdouble (const LongDouble &x)
{
  return this->write_16 (&x);
}

ACE_INLINE Boolean
RDROutputStream::write_string (const Char *x)
{
  if (x)
    {
      const UInt len =
        static_cast<UInt> (ACE_OS::strlen (x));
      return this->write_string (len, x);
    }

  return this->write_string (0, 0);
}

ACE_INLINE Boolean
RDROutputStream::write_wstring (const WChar *x)
{
  if (x)
    {
      UInt len =
        static_cast<UInt> (ACE_OS::strlen (x));
      return this->write_wstring (len, x);
    }

  return this->write_wstring (0, 0);
}

ACE_INLINE Boolean
RDROutputStream::write_char_array (const Char *x,
                                 UInt length)
{
  if (this->char_translator_ == 0)
    return this->write_array (x,
                              OCTET_SIZE,
                              OCTET_ALIGN,
                              length);
  return this->char_translator_->write_char_array (*this, x, length);
}

ACE_INLINE Boolean
RDROutputStream::write_wchar_array (const WChar* x,
                                  UInt length)
{
  if (this->wchar_translator_)
    return this->wchar_translator_->write_wchar_array (*this, x, length);

  if (RDROutputStream::wchar_maxbytes_ == 0)
    {
      errno = EACCES;
      return (Boolean) (this->good_bit_ = false);
    }

  if (RDROutputStream::wchar_maxbytes_ == sizeof (WChar))
    return this->write_array (x,
                              sizeof (WChar),
                              sizeof (WChar) == 2
                              ? SHORT_ALIGN
                              : LONG_ALIGN,
                              length);
  return this->write_wchar_array_i (x,length);
}

ACE_INLINE Boolean
RDROutputStream::write_octet_array (const Octet* x,
                                  UInt length)
{
  return this->write_array (x,
                            OCTET_SIZE,
                            OCTET_ALIGN,
                            length);
}

ACE_INLINE Boolean
RDROutputStream::write_short_array (const Short *x,
                                  UInt length)
{
  return this->write_array (x,
                            SHORT_SIZE,
                            SHORT_ALIGN,
                            length);
}

ACE_INLINE Boolean
RDROutputStream::write_ushort_array (const UShort *x,
                                   UInt length)
{
  return this->write_array (x,
                            SHORT_SIZE,
                            SHORT_ALIGN,
                            length);
}

ACE_INLINE Boolean
RDROutputStream::write_long_array (const Int *x,
                                 UInt length)
{
  return this->write_array (x,
                            LONG_SIZE,
                            LONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
RDROutputStream::write_ulong_array (const UInt *x,
                                  UInt length)
{
  return this->write_array (x,
                            LONG_SIZE,
                            LONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
RDROutputStream::write_longlong_array (const LongLong *x,
                                     UInt length)
{
  return this->write_array (x,
                            LONGLONG_SIZE,
                            LONGLONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
RDROutputStream::write_ulonglong_array (const ULong *x,
                                      UInt length)
{
  return this->write_array (x,
                            LONGLONG_SIZE,
                            LONGLONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
RDROutputStream::write_float_array (const Float *x,
                                  UInt length)
{
  return this->write_array (x,
                            LONG_SIZE,
                            LONG_ALIGN,
                            length);
}


ACE_INLINE Boolean
RDROutputStream::write_double_array (const Double *x,
                                   UInt length)
{
  return this->write_array (x,
                            LONGLONG_SIZE,
                            LONGLONG_ALIGN,
                            length);
}

ACE_INLINE Boolean
RDROutputStream::write_longdouble_array (const LongDouble* x,
                                       UInt length)
{
  return this->write_array (x,
                            LONGDOUBLE_SIZE,
                            LONGDOUBLE_ALIGN,
                            length);
}

ACE_INLINE bool
RDROutputStream::good_bit (void) const
{
  return this->good_bit_;
}

ACE_INLINE int
RDROutputStream::adjust (size_t size,
                       size_t align,
                       char*& buf)
{
  if (!this->current_is_writable_)
    return this->grow_and_adjust (size, align, buf);

#if !defined (ACE_LACKS_CDR_ALIGNMENT)
  size_t const offset =
    ACE_align_binary (this->current_alignment_, align)
    - this->current_alignment_;

  buf = this->current_->wr_ptr () + offset;
#else
  buf = this->current_->wr_ptr ();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

  char * const end = buf + size;

  if (end <= this->current_->end () &&
      end >= buf)
    {
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
      this->current_alignment_ += offset + size;
#endif /* ACE_LACKS_CDR_ALIGNMENT */
      this->current_->wr_ptr (end);
      return 0;
    }

  return this->grow_and_adjust (size, align, buf);
}

ACE_INLINE int
RDROutputStream::adjust (size_t size, char*& buf)
{
  return this->adjust (size, size, buf);
}

ACE_INLINE void
RDROutputStream::set_version (Octet major, Octet minor)
{
  this->major_version_ = major;
  this->minor_version_ = minor;
}

ACE_INLINE void
RDROutputStream::get_version (Octet &major, Octet &minor)
{
  major = this->major_version_;
  minor = this->minor_version_;
}


ACE_INLINE const ACE_Message_Block*
RDROutputStream::begin (void) const
{
  return &this->start_;
}

ACE_INLINE const ACE_Message_Block*
RDROutputStream::end (void) const
{
  return this->current_->cont ();
}

ACE_INLINE const ACE_Message_Block*
RDROutputStream::current (void) const
{
  return this->current_;
}

ACE_INLINE size_t
RDROutputStream::total_length (void) const
{
  return RDR::total_length (this->begin (), this->end ());
}

ACE_INLINE const char*
RDROutputStream::buffer (void) const
{
  return this->start_.rd_ptr ();
}

ACE_INLINE size_t
RDROutputStream::length (void) const
{
  return this->start_.length ();
}

ACE_INLINE bool
RDROutputStream::do_byte_swap (void) const
{
  return this->do_byte_swap_;
}

ACE_INLINE int
RDROutputStream::byte_order (void) const
{
  if (this->do_byte_swap ())
    return !RDR_BYTE_ORDER;
  else
    return RDR_BYTE_ORDER;
}

ACE_INLINE void
RDROutputStream::reset_byte_order (int byte_order)
{
  this->do_byte_swap_ = (byte_order != RDR_BYTE_ORDER);
}

ACE_INLINE size_t
RDROutputStream::current_alignment (void) const
{
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
  return this->current_alignment_;
#else
  // Default value set to 0
  return 0;
#endif /* ACE_LACKS_CDR_ALIGNMENT */
}

ACE_INLINE int
RDROutputStream::align_write_ptr (size_t alignment)
{
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
  char *dummy;
  return this->adjust (0, alignment, dummy);
#else
  ACE_UNUSED_ARG (alignment);
  // A return value of -1 from this function is used
  // to indicate failure, returning 0
  return 0;
#endif /* ACE_LACKS_CDR_ALIGNMENT */
}

ACE_INLINE RDR_Char_Codeset_Translator *
RDROutputStream::char_translator (void) const
{
  return this->char_translator_;
}

ACE_INLINE RDR_WChar_Codeset_Translator *
RDROutputStream::wchar_translator (void) const
{
  return this->wchar_translator_;
}

ACE_INLINE void
RDROutputStream::char_translator (RDR_Char_Codeset_Translator * ctran)
{
  this->char_translator_ = ctran;
}

ACE_INLINE void
RDROutputStream::wchar_translator (RDR_WChar_Codeset_Translator * wctran)
{
  this->wchar_translator_ = wctran;
}

// ****************************************************************

ACE_INLINE
RDRInputStream::~RDRInputStream (void)
{
}

ACE_INLINE Boolean
RDRInputStream::read_octet (Octet& x)
{
  return this->read_1 (&x);
}

ACE_INLINE Boolean
RDRInputStream::read_boolean (Boolean& x)
{
  Octet tmp = 0;
  (void) this->read_octet (tmp);
  x = tmp ? true : false;
  return (Boolean) this->good_bit_;
}

ACE_INLINE Boolean
RDRInputStream::read_char (Char &x)
{
  if (this->char_translator_ == 0)
    {
      void *temp = &x;
      return this->read_1 (reinterpret_cast<Octet*> (temp));
    }
  return this->char_translator_->read_char (*this, x);
}


ACE_INLINE Boolean
RDRInputStream::read_short (Short &x)
{
  void *temp = &x;
  return this->read_2 (reinterpret_cast<UShort*> (temp));
}

ACE_INLINE Boolean
RDRInputStream::read_ushort (UShort &x)
{
  return this->read_2 (&x);
}


ACE_INLINE Boolean
RDRInputStream::read_long (Int &x)
{
  void *temp = &x;
  return this->read_4 (reinterpret_cast<UInt*> (temp));
}


ACE_INLINE Boolean
RDRInputStream::read_ulong (UInt &x)
{
  return this->read_4 (&x);
}


ACE_INLINE Boolean
RDRInputStream::read_longlong (LongLong &x)
{
  void *temp = &x;
  return this->read_8 (reinterpret_cast<ULong*> (temp));
}

ACE_INLINE Boolean
RDRInputStream::read_ulonglong (ULong &x)
{
  return this->read_8 (&x);
}

ACE_INLINE Boolean
RDRInputStream::read_float (Float &x)
{
  void *temp = &x;
  return this->read_4 (reinterpret_cast<UInt*> (temp));
}

ACE_INLINE Boolean
RDRInputStream::read_double (Double &x)
{
  void *temp = &x;
  return this->read_8 (reinterpret_cast<ULong*> (temp));
}

ACE_INLINE Boolean
RDRInputStream::read_longdouble (LongDouble &x)
{
  return this->read_16 (&x);
}

ACE_INLINE size_t
RDRInputStream::length (void) const
{
  return this->start_.length ();
}

ACE_INLINE Boolean
RDRInputStream::read_char_array (Char* x,
                               UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  if (this->char_translator_ == 0)
    return this->read_array (x,
                             OCTET_SIZE,
                             OCTET_ALIGN,
                             length);
  return this->char_translator_->read_char_array (*this, x, length);
}

ACE_INLINE Boolean
RDRInputStream::read_wchar_array (WChar* x,
                                UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * RDROutputStream::wchar_maxbytes_ > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  if (this->wchar_translator_ != 0)
    return this->wchar_translator_->read_wchar_array (*this, x, length);
  if (RDROutputStream::wchar_maxbytes_ != sizeof (WChar))
    return this->read_wchar_array_i (x, length);
  return this->read_array (x,
                           sizeof (WChar),
                           sizeof (WChar) == 2
                           ? SHORT_ALIGN
                           : LONG_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_octet_array (Octet* x,
                                UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * OCTET_SIZE > this->length ())
    {	  
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           OCTET_SIZE,
                           OCTET_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_short_array (Short *x,
                                UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * SHORT_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           SHORT_SIZE,
                           SHORT_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_ushort_array (UShort *x,
                                 UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * SHORT_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           SHORT_SIZE,
                           SHORT_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_long_array (Int *x,
                               UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * LONG_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           LONG_SIZE,
                           LONG_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_ulong_array (UInt *x,
                                UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * LONG_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           LONG_SIZE,
                           LONG_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_longlong_array (LongLong *x,
                                   UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * LONGLONG_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           LONGLONG_SIZE,
                           LONGLONG_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_ulonglong_array (ULong *x,
                                    UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * LONGLONG_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           LONGLONG_SIZE,
                           LONGLONG_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_float_array (Float *x,
                                UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * LONG_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           LONG_SIZE,
                           LONG_ALIGN,
                           length);
}


ACE_INLINE Boolean
RDRInputStream::read_double_array (Double *x,
                                 UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * LONGLONG_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }

  return this->read_array (x,
                           LONGLONG_SIZE,
                           LONGLONG_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::read_longdouble_array (LongDouble* x,
                                     UInt length)
{
  // Make sure the length of the array isn't greater than the length of
  // the stream.
  if (length * LONGDOUBLE_SIZE > this->length ())
    {
      this->good_bit_ = false;
      return false;
    }
  return this->read_array (x,
                           LONGDOUBLE_SIZE,
                           LONGDOUBLE_ALIGN,
                           length);
}

ACE_INLINE Boolean
RDRInputStream::skip_octet (void)
{
  Octet x;
  return this->read_1 (&x);
}

ACE_INLINE Boolean
RDRInputStream::skip_char (void)
{
  return this->skip_octet ();  // sizeof (Char) == sizeof (Octet)
}

ACE_INLINE Boolean
RDRInputStream::skip_boolean (void)
{
  return this->skip_octet () && this->good_bit_;
}

ACE_INLINE Boolean
RDRInputStream::skip_ushort (void)
{
  UShort x;
  return this->read_2 (&x);
}

ACE_INLINE Boolean
RDRInputStream::skip_short (void)
{
  return this->skip_ushort ();
}

ACE_INLINE Boolean
RDRInputStream::skip_ulong (void)
{
  UInt x;
  return this->read_4 (&x);
}

ACE_INLINE Boolean
RDRInputStream::skip_long (void)
{
  return this->skip_ulong ();  // sizeof (Long) == sizeof (RTo_ULong)
}

ACE_INLINE Boolean
RDRInputStream::skip_ulonglong (void)
{
  ULong x;
  return this->read_8 (&x);
}

ACE_INLINE Boolean
RDRInputStream::skip_longlong (void)
{
  return this->skip_ulonglong ();  // sizeof (RToLongLong) == sizeof (RToULongLong)
}

ACE_INLINE Boolean
RDRInputStream::skip_float (void)
{
  return this->skip_ulong ();  // sizeof(Float) == sizeof (RTo_ULong)
}

ACE_INLINE Boolean
RDRInputStream::skip_double (void)
{
  return this->skip_ulonglong ();  // sizeof(Double) == sizeof (RToULongLong)
}

ACE_INLINE Boolean
RDRInputStream::skip_longdouble (void)
{
  LongDouble x;
  return this->read_16 (&x);
}

ACE_INLINE char*
RDRInputStream::end (void)
{
  return this->start_.end ();
}

ACE_INLINE void
RDRInputStream::rd_ptr (size_t offset)
{
  this->start_.rd_ptr (offset);
}

ACE_INLINE char*
RDRInputStream::rd_ptr (void)
{
  return this->start_.rd_ptr ();
}

ACE_INLINE char*
RDRInputStream::wr_ptr (void)
{
  return this->start_.wr_ptr ();
}

ACE_INLINE int
RDRInputStream::adjust (size_t size,
                      size_t align,
                      char*& buf)
{
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
  buf = ACE_ptr_align_binary (this->rd_ptr (), align);
#else
  buf = this->rd_ptr ();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

  char * const end = buf + size;
  if (end <= this->wr_ptr ())
    {
      this->start_.rd_ptr (end);
      return 0;
    }

  this->good_bit_ = false;
  return -1;
#if defined (ACE_LACKS_CDR_ALIGNMENT)
  ACE_UNUSED_ARG (align);
#endif /* ACE_LACKS_CDR_ALIGNMENT */
}

ACE_INLINE int
RDRInputStream::adjust (size_t size,
                      char*& buf)
{
  return this->adjust (size, size, buf);
}

ACE_INLINE const ACE_Message_Block*
RDRInputStream::start (void) const
{
  return &this->start_;
}

ACE_INLINE bool
RDRInputStream::good_bit (void) const
{
  return this->good_bit_;
}

// ****************************************************************

ACE_INLINE Boolean
operator<< (RDROutputStream &os, Char x)
{
  os.write_char (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, Short x)
{
  os.write_short (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, UShort x)
{
  os.write_ushort (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, Int x)
{
  os.write_long (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, UInt x)
{
  os.write_ulong (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, LongLong x)
{
  os.write_longlong (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, LongDouble x)
{
  os.write_longdouble (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, Float x)
{
  os.write_float (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, Double x)
{
  os.write_double (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, const Char *x)
{
  os.write_string (x);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, const WChar *x)
{
  os.write_wstring (x);
  return (Boolean) os.good_bit ();
}

// The following use the helper classes
ACE_INLINE Boolean
operator<< (RDROutputStream &os, RDROutputStream::from_boolean x)
{
  (void) os.write_boolean (x.val_);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, RDROutputStream::from_char x)
{
  os.write_char (x.val_);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, RDROutputStream::from_wchar x)
{
  os.write_wchar (x.val_);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, RDROutputStream::from_octet x)
{
  os.write_octet (x.val_);
  return (Boolean) os.good_bit ();
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, RDROutputStream::from_string x)
{
  UInt len = 0;

  if (x.val_ != 0)
    {
      len = static_cast<UInt> (ACE_OS::strlen (x.val_));
    }

  os.write_string (len, x.val_);
  return
    (Boolean) (os.good_bit () && (!x.bound_ || len <= x.bound_));
}

ACE_INLINE Boolean
operator<< (RDROutputStream &os, RDROutputStream::from_wstring x)
{
  UInt len = 0;

  if (x.val_ != 0)
    {
      len = static_cast<UInt> (ACE_OS::strlen (x.val_));
    }

  os.write_wstring (len, x.val_);
  return
    (Boolean) (os.good_bit () && (!x.bound_ || len <= x.bound_));
}

// ****************************************************************

ACE_INLINE Boolean
operator>> (RDRInputStream &is, Char &x)
{
  return is.read_char (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, Short &x)
{
  return is.read_short (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, UShort &x)
{
  return is.read_ushort (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>>(RDRInputStream &is, Int &x)
{
  return is.read_long (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, UInt &x)
{
  return is.read_ulong (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream& is, LongLong &x)
{
  return is.read_longlong (x) && is.good_bit ();
}


ACE_INLINE Boolean
operator>> (RDRInputStream& is, LongDouble &x)
{
  return is.read_longdouble (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, Float &x)
{
  return is.read_float (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, Double &x)
{
  return is.read_double (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, Char *&x)
{
  return is.read_string (x) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, WChar *&x)
{
  return is.read_wstring (x) && is.good_bit ();
}

// The following use the helper classes
ACE_INLINE Boolean
operator>> (RDRInputStream &is, RDRInputStream::to_boolean x)
{
  return is.read_boolean (x.ref_);
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, RDRInputStream::to_char x)
{
  return is.read_char (x.ref_) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, RDRInputStream::to_wchar x)
{
  return is.read_wchar (x.ref_) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, RDRInputStream::to_octet x)
{
  return is.read_octet (x.ref_) && is.good_bit ();
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, RDRInputStream::to_string x)
{
  // check if the bounds are satisfied
  return
    (is.read_string (const_cast<char *&> (x.val_))
     && is.good_bit ()
     && (!x.bound_
         || ACE_OS::strlen (x.val_) <= x.bound_));
}

ACE_INLINE Boolean
operator>> (RDRInputStream &is, RDRInputStream::to_wstring x)
{
  // check if the bounds are satisfied
  return
    (is.read_wstring (const_cast<WChar *&> (x.val_))
     && is.good_bit ()
     && (!x.bound_
         || ACE_OS::strlen (x.val_) <= x.bound_));
}

// ***************************************************************************
// We must define these methods here because they use the "read_*" inlined
// methods of the RDRInputStream class
// ***************************************************************************

ACE_INLINE Boolean
RDROutputStream::append_boolean (RDRInputStream &stream)
{
  Boolean x;
  return stream.read_boolean (x) ? this->write_boolean (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_char (RDRInputStream &stream)
{
  Char x;
  return stream.read_char (x) ? this->write_char (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_wchar (RDRInputStream &stream)
{
  WChar x;
  return stream.read_wchar (x) ? this->write_wchar (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_octet (RDRInputStream &stream)
{
  Octet x;
  return stream.read_octet (x) ? this->write_octet (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_short (RDRInputStream &stream)
{
  Short x;
  return stream.read_short (x) ? this->write_short (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_ushort (RDRInputStream &stream)
{
  UShort x;
  return stream.read_ushort (x) ? this->write_ushort (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_long (RDRInputStream &stream)
{
  Int x;
  return stream.read_long (x) ? this->write_long (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_ulong (RDRInputStream &stream)
{
  UInt x;
  return stream.read_ulong (x) ? this->write_ulong (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_longlong (RDRInputStream &stream)
{
  LongLong x;
  return stream.read_longlong (x) ? this->write_longlong (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_ulonglong (RDRInputStream &stream)
{
  ULong x;
  return stream.read_ulonglong (x) ? this->write_ulonglong (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_float (RDRInputStream &stream)
{
  Float x;
  return stream.read_float (x) ? this->write_float (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_double (RDRInputStream &stream)
{
  Double x;
  return stream.read_double (x) ? this->write_double (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_longdouble (RDRInputStream &stream)
{
  LongDouble x;
  return stream.read_longdouble (x) ? this->write_longdouble (x) : false;
}

ACE_INLINE Boolean
RDROutputStream::append_string (RDRInputStream &stream)
{
  Char *x;
  const Boolean flag =
    (stream.read_string (x) ? this->write_string (x) : false);
  delete [] x;
  return flag;
}

ACE_INLINE Boolean
RDROutputStream::append_wstring (RDRInputStream &stream)
{
  WChar *x;
  const Boolean flag =
    (stream.read_wstring (x) ? this->write_wstring (x) : false);
  delete [] x;
  return flag;
}

ACE_INLINE void
RDRInputStream::reset_byte_order (int byte_order)
{
  this->do_byte_swap_ = (byte_order != RDR_BYTE_ORDER);
}

ACE_INLINE bool
RDRInputStream::do_byte_swap (void) const
{
  return this->do_byte_swap_;
}

ACE_INLINE int
RDRInputStream::byte_order (void) const
{
  return this->do_byte_swap () ? !RDR_BYTE_ORDER : RDR_BYTE_ORDER;
}

ACE_INLINE int
RDRInputStream::align_read_ptr (size_t alignment)
{
#if !defined (ACE_LACKS_CDR_ALIGNMENT)
  char *buf = ACE_ptr_align_binary (this->rd_ptr (),
                                    alignment);
#else
  char *buf = this->rd_ptr ();
#endif /* ACE_LACKS_CDR_ALIGNMENT */

  if (buf <= this->wr_ptr ())
    {
      this->start_.rd_ptr (buf);
      return 0;
    }

  this->good_bit_ = false;
  return -1;
}

ACE_INLINE void
RDRInputStream::set_version (Octet major, Octet minor)
{
  this->major_version_ = major;
  this->minor_version_ = minor;
}

ACE_INLINE void
RDRInputStream::get_version (Octet &major, Octet &minor)
{
  major = this->major_version_;
  minor = this->minor_version_;
}

ACE_INLINE RDR_Char_Codeset_Translator *
RDRInputStream::char_translator (void) const
{
  return this->char_translator_;
}

ACE_INLINE RDR_WChar_Codeset_Translator *
RDRInputStream::wchar_translator (void) const
{
  return this->wchar_translator_;
}


ACE_INLINE void
RDRInputStream::char_translator (RDR_Char_Codeset_Translator * ctran)
{
  this->char_translator_ = ctran;
}

ACE_INLINE void
RDRInputStream::wchar_translator (RDR_WChar_Codeset_Translator * wctran)
{
  this->wchar_translator_ = wctran;
}

// ****************************************************************

ACE_INLINE Boolean
RDR_Char_Codeset_Translator::read_1 (RDRInputStream& input,
                                     Octet *x)
{
  return input.read_1 (x);
}

ACE_INLINE Boolean
RDR_Char_Codeset_Translator::write_1 (RDROutputStream& output,
                                      const Octet *x)
{
  return output.write_1 (x);
}

ACE_INLINE Boolean
RDR_Char_Codeset_Translator::read_array (RDRInputStream& in,
                                         void* x,
                                         size_t size,
                                         size_t align,
                                         UInt length)
{
  return in.read_array (x, size, align, length);
}

ACE_INLINE Boolean
RDR_Char_Codeset_Translator::write_array (RDROutputStream& out,
                                          const void *x,
                                          size_t size,
                                          size_t align,
                                          UInt length)
{
  return out.write_array(x, size, align, length);
}

ACE_INLINE int
RDR_Char_Codeset_Translator::adjust (RDROutputStream& out,
                                     size_t size,
                                     size_t align,
                                     char *&buf)
{
  return out.adjust(size, align, buf);
}

ACE_INLINE void
RDR_Char_Codeset_Translator::good_bit (RDROutputStream& out, bool bit)
{
  out.good_bit_ = bit;
}

ACE_INLINE Octet
RDR_Char_Codeset_Translator::major_version (RDRInputStream& input)
{
  return input.major_version_;
}

ACE_INLINE Octet
RDR_Char_Codeset_Translator::minor_version (RDRInputStream& input)
{
  return input.minor_version_;
}

ACE_INLINE Octet
RDR_Char_Codeset_Translator::major_version (RDROutputStream& output)
{
  return output.major_version_;
}

ACE_INLINE Octet
RDR_Char_Codeset_Translator::minor_version (RDROutputStream& output)
{
  return output.minor_version_;
}

// ****************************************************************

ACE_INLINE Boolean
RDR_WChar_Codeset_Translator::read_1 (RDRInputStream& input,
                                      Octet *x)
{
  return input.read_1 (x);
}

ACE_INLINE Boolean
RDR_WChar_Codeset_Translator::read_2 (RDRInputStream& input,
                                      UShort *x)
{
  return input.read_2 (x);
}

ACE_INLINE Boolean
RDR_WChar_Codeset_Translator::read_4 (RDRInputStream& input,
                                      UInt *x)
{
  return input.read_4 (x);
}

ACE_INLINE Boolean
RDR_WChar_Codeset_Translator::write_1 (RDROutputStream& output,
                                       const Octet *x)
{
  return output.write_1 (x);
}

ACE_INLINE Boolean
RDR_WChar_Codeset_Translator::write_2 (RDROutputStream& output,
                                       const UShort *x)
{
  return output.write_2 (x);
}

ACE_INLINE Boolean
RDR_WChar_Codeset_Translator::write_4 (RDROutputStream& output,
                                       const UInt *x)
{
  return output.write_4 (x);
}

ACE_INLINE Boolean
RDR_WChar_Codeset_Translator::read_array (RDRInputStream& in,
                                          void* x,
                                          size_t size,
                                          size_t align,
                                          UInt length)
{
  return in.read_array (x, size, align, length);
}

ACE_INLINE Boolean
RDR_WChar_Codeset_Translator::write_array (RDROutputStream& out,
                                           const void *x,
                                           size_t size,
                                           size_t align,
                                           UInt length)
{
  return out.write_array(x, size, align, length);
}

ACE_INLINE int
RDR_WChar_Codeset_Translator::adjust (RDROutputStream& out,
                                      size_t size,
                                      size_t align,
                                      char *&buf)
{
  return out.adjust(size, align, buf);
}

ACE_INLINE void
RDR_WChar_Codeset_Translator::good_bit (RDROutputStream& out, bool bit)
{
  out.good_bit_ = bit;
}

ACE_INLINE Octet
RDR_WChar_Codeset_Translator::major_version (RDRInputStream& input)
{
  return input.major_version_;
}

ACE_INLINE Octet
RDR_WChar_Codeset_Translator::minor_version (RDRInputStream& input)
{
  return input.minor_version_;
}

ACE_INLINE Octet
RDR_WChar_Codeset_Translator::major_version (RDROutputStream& output)
{
  return output.major_version_;
}

ACE_INLINE Octet
RDR_WChar_Codeset_Translator::minor_version (RDROutputStream& output)
{
  return output.minor_version_;
}

