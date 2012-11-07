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
 * File:   MissingFramePacket.h
 * Author: rmartins
 *
 * Created on November 18, 2010, 3:27 PM
 */

#ifndef MISSINGFRAMEPACKET_H
#define	MISSINGFRAMEPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>

class MissingFramePacket : public SthenoStreamPacket {
public:    
    static const UShort MISSING_FRAME_PACKET_OP = 0x8323;
    
    MissingFramePacket();
    MissingFramePacket(InputStream& inputStream);
    MissingFramePacket(ULong requestID,ULong missingFrameID);
    virtual ~MissingFramePacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);

    int getMissingFrameID();    

protected:
    int m_missingFrameID;    
    
    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};



#endif	/* MISSINGFRAMEPACKET_H */

