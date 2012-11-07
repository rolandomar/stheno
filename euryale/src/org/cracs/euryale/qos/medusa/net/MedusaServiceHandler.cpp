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
 * File:   MedusaServiceHandler.cpp
 * Author: rmartins
 * 
 * Created on December 16, 2009, 3:28 PM
 */

#include "MedusaServiceHandler.h"
#include "euryale/Types.h"
#include "euryale/qos/net/packet/MedusaPacket.h"
#include "euryale/qos/medusa/MedusadCore.h"
#include <euryale/net/packet/Packet.h>
#include <ace/Reactor.h>
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <euryale/qos/net/packet/MedusaPacketFactory.h>

int
MedusaServiceHandler::open(void*) {
    /*ACE_TCHAR peer_name[MAXHOSTNAMELEN];
    ACE_INET_Addr peer_addr;
    if (this->sock_.get_remote_addr(peer_addr) == 0 &&
            peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN) == 0)
        ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T) Connection from %s\n"),
            peer_name));*/
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ClientService:open:ok\n")));
    m_begin = 0;
    m_header = 0;
    m_partialMessage = 0;
    this->notifier_.reactor(ACE_Reactor::instance()); //this->reactor ());
    this->output_queue_.notification_strategy(&this->notifier_);
    int ret = this->reactor()->register_handler
            (this, ACE_Event_Handler::READ_MASK);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ClientService:open: register=%d\n"), ret));
    return ret;
}

int
MedusaServiceHandler::handle_input(ACE_HANDLE) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Handle_input\n")));
    ssize_t recv_cnt;
    ssize_t readSize = 0;
    //new message
    ACE_Message_Block* buffer = 0;
    if (m_header == 0) {
        if (m_partialMessage == 0) {
            readSize = MedusaHeader::getSize();
            buffer = new ACE_Message_Block(readSize);
        } else {
            /*readSize = MedusaHeader::getSize() - m_partialMessage->length();
            buffer = new ACE_Message_Block(MedusaHeader::getSize());
            ACE_OS::memcpy(buffer->rd_ptr(), m_partialMessage->rd_ptr(), m_partialMessage->length());
            buffer->rd_ptr(m_partialMessage->length());
            delete m_partialMessage;
            m_partialMessage = 0;*/
            readSize = MedusaHeader::getSize() - m_partialMessage->length();
            buffer = new ACE_Message_Block(MedusaHeader::getSize());
            size_t total_size = m_partialMessage->wr_ptr() - m_partialMessage->base();
            readSize -= total_size;
            ACE_OS::memcpy(buffer->base(), m_partialMessage->base(), total_size);
            buffer->wr_ptr(total_size);
            ACE_Message_Block::release(m_partialMessage);
            m_partialMessage = 0;
        }
    } else {
        if (m_partialMessage == 0) {
            readSize = m_header->getBodySize();
            buffer = new ACE_Message_Block(readSize);
        } else {
            /*readSize = m_header->getBodySize();
            readSize -= m_partialMessage->length();
            buffer = new ACE_Message_Block(m_header->getBodySize());
            ACE_OS::memcpy(buffer->rd_ptr(), m_partialMessage->rd_ptr(), m_partialMessage->length());
            buffer->rd_ptr(m_partialMessage->length());
            delete m_partialMessage;
            m_partialMessage = 0;*/
            readSize = m_header->getBodySize();
            buffer = new ACE_Message_Block(m_header->getBodySize());
            size_t total_size = m_partialMessage->wr_ptr() - m_partialMessage->base();
            readSize -= total_size;
            ACE_OS::memcpy(buffer->base(), m_partialMessage->base(), total_size);
            buffer->wr_ptr(total_size);
            ACE_Message_Block::release(m_partialMessage);
            m_partialMessage = 0;
        }
    }

    /* if ((recv_cnt = this->sock_.recv(buffer->wr_ptr(), buffer->space())) <= 0) {
         ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("(%T) Connection closed\n")));
         return -1;
     }
     buffer->wr_ptr(recv_cnt);*/
    if ((recv_cnt = peer().recv(buffer->wr_ptr(), readSize)) <= 0) {
        return -1;
    }
    buffer->wr_ptr(recv_cnt);
    if (m_header == 0) {
        if (recv_cnt < readSize) {
            /*m_partialMessage = buffer;
            m_partialMessage->wr_ptr(recv_cnt);*/
            m_partialMessage = buffer;
            buffer = 0;
        } else {//complete
            /*ByteInputStream bis(buffer);
            m_header = new MedusaHeader(bis);
            if (!bis.good_bit()) {
                delete m_header;
                return -1;
            }*/
            ByteInputStream bis(buffer);
            ACE_Message_Block::release(buffer);
            buffer = 0;
            m_header = new MedusaHeader(bis);
            if (!bis.good_bit()) {
                delete m_header;
                return -1;
            }
            if (m_header->getBodySize() == 0) {
                //complete
                ByteInputStream bis(buffer);
                MedusaPacket* packet = MedusaPacketFactory::createPacket(m_header, bis);
                if (packet->getPacketHeader()->getType() == CLIENT_INIT_ID) {
                    m_uuid = static_cast<InitMedusaClientPacket*> (packet)->getClientUUID();
                    delete packet;
                } else {
                    m_core->processPacket(packet, this);
                }
                m_header = 0;
            }
        }
        return 0;
    } else {
        if (recv_cnt < readSize) {
            /*m_partialMessage = buffer;
            m_partialMessage->wr_ptr(recv_cnt);*/
            m_partialMessage = buffer;
            buffer = 0;
        } else {
            //complete
            /*ByteInputStream bis(buffer);*/
            ByteInputStream bis(buffer);
            ACE_Message_Block::release(buffer);
            buffer = 0;
            MedusaPacket* packet = MedusaPacketFactory::createPacket(m_header, bis);
            if (packet->getPacketHeader()->getType() == CLIENT_INIT_ID) {
                m_uuid = static_cast<InitMedusaClientPacket*> (packet)->getClientUUID();
                delete packet;
            } else {
                m_core->processPacket(packet, this);
            }
            m_header = 0;
        }
        return 0;
    }
}

/*int
MedusaServiceHandler::handle_input(ACE_HANDLE)
{
    const size_t INPUT_SIZE = 4096;
    char buffer[INPUT_SIZE];
    ssize_t recv_cnt, send_cnt;

    if ((recv_cnt = this->sock_.recv(buffer, sizeof (buffer))) <= 0) {
        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%T) Connection closed\n")));
        return -1;
    }

    send_cnt =
            this->sock_.send(buffer, static_cast<size_t> (recv_cnt));
    if (send_cnt == recv_cnt)
        return 0;
    if (send_cnt == -1 && ACE_OS::last_error() != EWOULDBLOCK)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T) %@\n"),
            ACE_TEXT("(%T)send")),
            0);
    if (send_cnt == -1)
        send_cnt = 0;
    ACE_Message_Block *mb;
    size_t remaining =
            static_cast<size_t> ((recv_cnt - send_cnt));
    ACE_NEW_RETURN(mb, ACE_Message_Block(remaining), -1);
    mb->copy(&buffer[send_cnt], remaining);
    int output_off = this->output_queue_.is_empty();
    ACE_Time_Value nowait(ACE_OS::gettimeofday());
    if (this->output_queue_.enqueue_tail(mb, &nowait) == -1) {
        ACE_ERROR((LM_ERROR,
                ACE_TEXT("(%T) %@; discarding data\n"),
                ACE_TEXT("(%T)enqueue failed")));
        mb->release();
        return 0;
    }
    if (output_off)
        return this->reactor()->register_handler
            (this, ACE_Event_Handler::WRITE_MASK);
    return 0;
}*/
// Listing 8

// Listing 9 code/ch07

int MedusaServiceHandler::send(MedusaPacket* packet) {
    //OutputStream* os = ObjectStreamFactory::createOutputStream(packet->getSerializationSize());
    //ByteOutputStream os(packet->getSerializationSize());
    MedusaHeader *header = static_cast<MedusaHeader*> (packet->getPacketHeader());
    if (header->getPacketID() == 0) {
        header->m_requestID = ++m_sequenceID;
    }
    StreamSize ss;
    packet->serialize(ss);
    ByteOutputStream os(ss.total_length());
    packet->serialize(os);

    ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
    RDR::consolidate(mb, &os.start_);
    this->output_queue_.enqueue(mb);
    //this->reactor()->register_handler
    //      (this, ACE_Event_Handler::WRITE_MASK);
    return 0;
}

int
MedusaServiceHandler::handle_output(ACE_HANDLE) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Handle output\n")));
    ACE_Message_Block *mb;
    ACE_Time_Value nowait(ACE_OS::gettimeofday());
    while (0 <= this->output_queue_.dequeue_head
            (mb, &nowait)) {
        ssize_t send_cnt =
                this->sock_.send(mb->rd_ptr(), mb->length());
        if (send_cnt == -1)
            ACE_ERROR((LM_ERROR,
                ACE_TEXT("(%T) %@\n"),
                ACE_TEXT("(%T)send")));
        else
            mb->rd_ptr(static_cast<size_t> (send_cnt));
        if (mb->length() > 0) {
            this->output_queue_.enqueue_head(mb);
            break;
        }
        mb->release();
    }
    return 0;
    //return (this->output_queue_.is_empty()) ? -1 : 0;
}

int
MedusaServiceHandler::handle_close(ACE_HANDLE, ACE_Reactor_Mask mask) {
    if (mask == ACE_Event_Handler::WRITE_MASK)
        return 0;
    mask = ACE_Event_Handler::ALL_EVENTS_MASK |
            ACE_Event_Handler::DONT_CALL;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ClientService:handle_close\n")));
    this->reactor()->remove_handler(this, mask);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ClientService:handle_close: after remove_handler\n")));
    this->sock_.close();
    this->output_queue_.flush();
    //
    m_core->onCloseHandle(this);
    delete this;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ClientService:handle_close:after delete\n")));
    return 0;
}

UUIDPtr& MedusaServiceHandler::getUUID() {
    return m_uuid;
}