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
#include <ace/svc_export.h>
#include <stheno/core/p2p/p3/leaf/LeafPeer.h>

#include <stheno/core/p2p/p3/common/P3PeerInfo.h>
// Returns the pointer to the Today class.
// The ACE_BUILD_SVC_DLL and ACE_Svc_Export directives are necessary to
// take care of exporting the function for Win32 platforms.
extern "C" ACE_Svc_Export Overlay *create_overlay (void);

Overlay *create_overlay (void)
{
  LeafPeer *overlay = 0;
  //ACE_NEW_RETURN (overlay, SuperPeer, 0);
  overlay = new LeafPeer(P3PeerInfo::SENSOR);
  return overlay;
}
