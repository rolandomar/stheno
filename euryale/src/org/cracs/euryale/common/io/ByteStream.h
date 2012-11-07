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
 * File:   ByteStream.h
 * Author: rmartins
 *
 * Created on January 13, 2010, 2:50 PM
 */

#ifndef _BYTESTREAM_H
#define	_BYTESTREAM_H
#include <ace/Basic_Types.h>
#define ACE_LACKS_CDR_ALIGNMENT

#if defined (GEN_OSTREAM_OPS)
#include <ace/streams.h>
#endif /* GEN_OSTREAM_OPS */

// Stuff used by the ACE CDR classes.
#if defined ACE_LITTLE_ENDIAN
#define RDR_BYTE_ORDER 1
// little endian encapsulation byte order has value = 1
#else  /* ! ACE_LITTLE_ENDIAN */
#define RDR_BYTE_ORDER 0
// big endian encapsulation byte order has value = 0
#endif /* ! ACE_LITTLE_ENDIAN */

#define ACE_DISABLE_SWAP_ON_READ

#endif	/* _BYTESTREAM_H */

