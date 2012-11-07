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
 * File:   PacketChannel.h
 * Author: rmartins
 *
 * Created on April 18, 2010, 11:30 PM
 */

#ifndef _PACKETDATAGRAMCHANNEL_H
#define	_PACKETDATAGRAMCHANNEL_H

#include <euryale/net/AbstractDatagramChannel.h>
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <euryale/net/packet/Packet.h>

template <class PACKET, class HEADER, class PACKET_FACTORY, class SOCK_TYPE, class SYNC_TYPE>
class PacketDatagramChannel;

/*template <class PACKET, class HEADER, class PACKET_FACTORY, class SOCK_TYPE, class SYNC_TYPE>
class IncomingDatagramListener {
public:
    virtual void onPacket(PacketDatagramChannel<PACKET, HEADER, PACKET_FACTORY, SOCK_TYPE, SYNC_TYPE>* channel, PACKET* packet) = 0;
};*/

template <class HEADER, class PACKET>
class WriteDatagramBlockPacket : public WriteDatagramBlock {
public:

    WriteDatagramBlockPacket(PACKET* packet, ACE_INET_Addr& addr, ACE_Time_Value* timeout) :
    WriteDatagramBlock(timeout) {
        HEADER *header = static_cast<HEADER*> (packet->getPacketHeader());
        StreamSize ss;
        packet->serialize(ss);
        ByteOutputStream os(ss.total_length());
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)serializePacket: before ser PACKET ID=%d\n", header->m_requestID);
        packet->serialize(os);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)serializePacket: after\n");
        ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
        RDR::consolidate(mb, &os.start_);
        m_mb = mb;
        m_addr = addr;
    }

    ~WriteDatagramBlockPacket() {
        if (m_mb != 0) {
            delete m_mb;
        }
    }
};

template <class PACKET, class HEADER, class PACKET_FACTORY, class SOCK_TYPE, class SYNC_TYPE>
class PacketDatagramChannel : public AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE> {
public:

    static const int MAX_DATAGRAM_SIZE = 102400;
    
    PacketDatagramChannel(bool asyncRead, bool asyncWrite,
            bool inputQueue = false,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseDatagramListener<SOCK_TYPE, SYNC_TYPE>* closeListener = 0) :
    AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>(asyncRead, asyncWrite, inputQueue, ec, reserve, qos, closeListener),
    m_begin(0), m_current(0), m_header(0), m_partialMessage(0){//, m_packetHandler(0) {
    }
  

    /*PacketDatagramChannel(bool asyncWrite,
            IncomingDatagramListener<PACKET, HEADER, PACKET_FACTORY, SOCK_TYPE, SYNC_TYPE> *packetHandler,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseDatagramListener<SOCK_TYPE, SYNC_TYPE>* closeListener = 0) :
    AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>(false, asyncWrite, false, ec, reserve, qos, closeListener),
    m_begin(0), m_current(0), m_header(0), m_partialMessage(0), m_packetHandler(packetHandler) {
    }

    virtual ~PacketDatagramChannel() {
        //~AbstractChannel<SOCK_TYPE, SYNC_TYPE>();
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)PacketChannel::~PacketChannel()\n");
    }*/

    /*int openMulticast(const ACE_INET_Addr &mcast_addr,        // Bound & sendto address.
            const ACE_TCHAR *net_if = 0,            // Net interface for sends.
            int reuse_addr = 1){
        return AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::open(mcast_addr,net_if,reuse_addr);
    }*/

    virtual int send(PACKET* packet, ACE_Time_Value* timeout = 0) {        
        ACE_Message_Block* mb = serializePacket(packet);
        int ret =  AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::send(mb, 0, timeout);
        ACE_Message_Block::release(mb);
        return ret;
    }

    virtual int send(PACKET* packet, ACE_INET_Addr* addr, ACE_Time_Value* timeout = 0) {
        ACE_Message_Block* mb = serializePacket(packet);
        int ret = AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::send(mb, addr, timeout);
        ACE_Message_Block::release(mb);
        return ret;
    }

    //virtual int sendAsync(WriteDatagramBlockPacket<HEADER, PACKET>* wb) {
    virtual int sendAsync(WriteDatagramBlockPtr* wb) {
        return AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::sendAsync(wb);
    }

    virtual int receivePacket(PACKET*& packet, ACE_Time_Value* timeout) {        
        if (AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::hasAsyncRead()) {
            //async socket, sync read
            /*if (m_packetHandler != 0) {
                m_packetHandler->onPacket(this, packet);
                return 0;
            }*/

            if (AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::m_hasInputQueue) {
                ACE_Message_Block* mb = 0;
                int ret = AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::m_inputQueue.dequeue(mb, timeout);
                if (mb == 0 || ret == -1) {
                    return -1;
                }
                ACE_OS::memcpy(&packet, mb->base(), sizeof (PACKET*));
                return 0;
            }
            return -1;
        } else {
            ACE_Time_Value absTimeout = ACE_OS::gettimeofday();
            if(timeout != 0){
                absTimeout += *timeout;
            }
            while (packet == 0) {
                if (timeout != 0) {
                    ACE_Time_Value now = ACE_OS::gettimeofday();
                    if (now > absTimeout) {
                        //printf("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL22222222222222L\n");
                        //AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::close();
                        //AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::close();
                        //return -1;
                        return 0;
                    }
                }
                ACE_INET_Addr addr;               
                int ret = handleInputParsePacket(packet, addr, timeout);               
                if (ret == -1) {
                    //printf("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n");
                    AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::close();
                    return -1;
                }

            }
            return 0;
        }
    }

protected:
    virtual int handlePacket(PACKET* packet) = 0;

    virtual int handleInput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int handleInputParsePacket(PACKET*& packet, ACE_INET_Addr& addr, ACE_Time_Value* timeout);

    virtual int handleInputQueue_i(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        return 0;
    }

    virtual ACE_Message_Block* serializePacket(PACKET* packet) {
        HEADER *header = static_cast<HEADER*> (packet->getPacketHeader());
        if (header->getPacketID() == 0) {
            header->m_requestID = ++m_sequenceID;
        }
        StreamSize ss;
        packet->serialize(ss);
        ByteOutputStream os(ss.total_length());
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)serializePacket: before ser PACKET ID=%d\n", header->m_requestID);
        packet->serialize(os);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)serializePacket: after\n");
        ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(os.current_, 0));
        RDR::consolidate(mb, os.current_);
        return mb;
    }

    ACE_Message_Block* m_begin;
    ACE_Message_Block* m_current;
    HEADER* m_header;
    ACE_Message_Block* m_partialMessage;
    ACE_Atomic_Op<ACE_Thread_Mutex, UInt> m_sequenceID;
    //IncomingDatagramListener<PACKET, HEADER, PACKET_FACTORY, SOCK_TYPE, SYNC_TYPE> *m_packetHandler;
};

template <class PACKET, class HEADER, class PACKET_FACTORY, class SOCK_TYPE, class SYNC_TYPE>
int PacketDatagramChannel<PACKET, HEADER, PACKET_FACTORY, SOCK_TYPE, SYNC_TYPE>::handleInput_i(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: PacketChannel: handle_input()\n")));
    PACKET* packet = 0;
    ACE_INET_Addr addr;
    int ret = handleInputParsePacket(packet, addr, 0);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleInputParsePacket ret=%d\n",ret);
    if (ret == -1) {
        return -1;
    }    
    if (packet == 0) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)no packet\n")));
        return ret;
    } else {
        if (AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::m_hasInputQueue) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)queue packet\n");
            ACE_Message_Block* mbMeta = new ACE_Message_Block(sizeof (PACKET*));
            ACE_OS::memcpy(mbMeta->base(), &packet, sizeof (PACKET*));
            ACE_Time_Value now = ACE_OS::gettimeofday();
            return AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::m_inputQueue.enqueue_tail(mbMeta, &now);
        } else {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handle packet\n");
            if (handlePacket(packet) == -1) {
                return -1;
            }
            return 0;
        }

    }

}

template <class PACKET, class HEADER, class PACKET_FACTORY, class SOCK_TYPE, class SYNC_TYPE>
int PacketDatagramChannel<PACKET, HEADER, PACKET_FACTORY, SOCK_TYPE, SYNC_TYPE>::
handleInputParsePacket(PACKET*& inputPacket, ACE_INET_Addr& addr, ACE_Time_Value* timeout) {

    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)PacketChannel::Handle_input\n");
    ssize_t recv_cnt = 0;
    ssize_t readSize = 0;
    inputPacket = 0;
    //new message
    ACE_Message_Block* buffer = 0;


    readSize = MAX_DATAGRAM_SIZE; //HEADER::getSize();
    buffer = new ACE_Message_Block(readSize);
    int flags = 0;    
    if ((recv_cnt = AbstractDatagramChannel<SOCK_TYPE, SYNC_TYPE>::peer().recv(buffer->wr_ptr(), buffer->space(), addr, flags, timeout)) <= 0) {
        if(errno == ETIME){
            ACE_Message_Block::release(buffer);
            return 0;
        }
        ACE_Message_Block::release(buffer);
        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%T) Connection closed\n")));
        return -1;
    }
    if(recv_cnt < HEADER::getSize()){
        ACE_Message_Block::release(buffer);
        return 0;
    }

    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleInputParsePacket after %d %d\n",ACE_OS::gettimeofday().msec(),timeout->msec());
    buffer->wr_ptr(recv_cnt);
    /*ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T) PacketChannel::handle_input RECEIVED(%d)\n"), recv_cnt));*/
    ByteInputStream bis(buffer);
    ACE_Message_Block::release(buffer);
    m_header = new HEADER(bis);
    /*ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T) Got Header!\n")));*/
    if (!bis.good_bit()) {
        ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T) Got Header error!\n")));
        delete m_header;
        m_header = 0;
        return -1;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)BodySize=%d\n",m_header->getBodySize());
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)BodySize2=%d\n",m_header->getType());
    if (m_header->getBodySize() == 0 || m_header->getBodySize() == bis.length()) {
        //complete
        /*ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T) Body!\n")));*/
        //ByteInputStream bis(buffer);
        /*ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T) Body after buffer buffer_size=%lu!\n"),bis.length()));*/
        PACKET* packet = PACKET_FACTORY::createPacket(m_header, bis);
        /*ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)PacketDatagramChannel::handleInputParsePacket type=%d\n",
        m_header->getType());*/
        /*ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%T) Got Packet! Body size=%d left_on_buffer=%lu\n"),
                packet->getPacketHeader()->getBodySize(),bis.length()));*/
        //if (handlePacket(packet) == -1) {
        //    return -1;
        //}
        inputPacket = packet;
        m_header = 0;
    }else{
        delete m_header;
        m_header = 0;
    }

    return 0;

}

#endif	/* _PACKETCHANNEL_H */

