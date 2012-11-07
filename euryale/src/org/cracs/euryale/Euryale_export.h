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
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl RTo
// ------------------------------
#ifndef Euryale_Export_H
#define Euryale_Export_H

#include <ace/config-all.h>

#if defined (ACE_AS_STATIC_LIBS) && !defined (EURYALE_HAS_DLL)
#  define EURYALE_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && RTO_HAS_DLL */

#if !defined (EURYALE_HAS_DLL)
#  define EURYALE_HAS_DLL 1
#endif /* ! RTO_HAS_DLL */

#if defined (EURYALE_HAS_DLL) && (EURYALE_HAS_DLL == 1)
#  if defined (EURYALE_HAS_DLL)
#    define Euryale_Export ACE_Proper_Export_Flag
#    define EURYALE_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define EURYALE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* EURYALE_HAS_DLL */
#    define Euryale_Export ACE_Proper_Import_Flag
#    define EURYALE(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define EURYALE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* EURYALE_HAS_DLL */
#else /* EURYALE_HAS_DLL == 1 */
#  define Euryale_Export
#  define EURYALE_SINGLETON_DECLARATION(T)
#  define EURYALE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* EURYALE_HAS_DLL == 1 */

// Set EURYALE_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (EURYALE_NTRACE)
#  if (ACE_NTRACE == 1)
#    define EURYALE_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define EURYALE_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !EURYALE_NTRACE */

#if (EURYALE_NTRACE == 1)
#  define EURYALE_TRACE(X)
#else /* (EURYALE_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define EURYALE_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (EURYALE_NTRACE == 1) */

#endif /* Euryale_Export_H */

