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
#ifndef EURYALECONF_H_
#define EURYALECONF_H_

#include /**/ "ace/pre.h"

#include <ace/Basic_Types.h>
#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"
#include <euryale/Types.h>


#if !defined (EURYALE_SYNCH_MUTEX)
#define EURYALE_SYNCH_MUTEX ACE_SYNCH_MUTEX
#endif /* EURYALE_SYNCH_MUTEX */

#if !defined (EURYALE_SYNCH_CONDITION)
#define EURYALE_SYNCH_CONDITION ACE_SYNCH_CONDITION
#endif /* RTO_SYNCH_CONDITION */

#ifdef RTM_LOCKING_NAME
#if !defined (RTM_REACTOR)
#define RTM_REACTOR ACE_Select_Reactor
#endif /* RTM_REACTOR */

#if !defined (RTM_NULL_LOCK_REACTOR)
#define RTM_NULL_LOCK_REACTOR ACE_Select_Reactor_T< ACE_Reactor_Token_T<ACE_Noop_Token> >
#endif /* RTM_NULL_LOCK_REACTOR */

// Define this to modify the default mutex type used throughout RTM.
#if !defined (RTM_SYNCH_MUTEX)
#define RTM_SYNCH_MUTEX ACE_SYNCH_MUTEX
#endif /* RTM_SYNCH_MUTEX */

#if !defined (RTM_SYNCH_RECURSIVE_MUTEX)
#define RTM_SYNCH_RECURSIVE_MUTEX ACE_SYNCH_RECURSIVE_MUTEX
#endif /* RTM_SYNCH_RECURSIVE_MUTEX */

#if !defined (RTM_SYNCH_RW_MUTEX)
#define RTM_SYNCH_RW_MUTEX ACE_SYNCH_RW_MUTEX
#endif /* RTM_SYNCH_RW_MUTEX */

#if !defined (RTM_SYNCH_CONDITION)
#define RTM_SYNCH_CONDITION ACE_SYNCH_CONDITION
#endif /* RTM_SYNCH_CONDITION */
#endif

#if !defined (RTM_DEFAULT_NETWORK_SERVICE)
#define RTM_DEFAULT_NETWORK_SERVICE P3Network
#endif

typedef Short Priority;
#define MINPRIORITY 0;
#define MAXPRIORITY 32767;




#include /**/ "ace/post.h"

#endif /*RTMCONF_H_*/
