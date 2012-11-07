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
 * File:   MedusaBaseHandler.cpp
 * Author: rmartins
 * 
 * Created on February 13, 2010, 6:35 PM
 */

#include "MedusaBaseHandler.h"

#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_errno.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_sys_time.h>
//#include <euryale/common/io/RDRStream.h>
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <euryale/qos/net/packet/MedusaPacketFactory.h>

int MedusaBaseHandler::send(MedusaPacket* packet)
{
    //OutputStream* os = ObjectStreamFactory::createOutputStream(packet->getSerializationSize());
    StreamSize ss;
    packet->serialize(ss);
    ByteOutputStream os(ss.total_length());
    packet->serialize(os);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaClientHandler: after\n")));
    //ACE_Message_Block mb(RDR::total_length(os.current_, 0));
    //RDR::consolidate(&mb, os.current_);
    int size = os.current_->length();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaClientHandler: after 2 BodySize=%d SIZE=%d\n"), packet->getPacketHeader()->getBodySize(), size));
    int snd_cnt = 0;
    do {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaClientHandler: after 3\n")));
        snd_cnt = super::peer().send(os.current_->rd_ptr(), size, 0);
        //ACE_DEBUG((LM_DEBUG,
            //ACE_TEXT("(%T) MedusaClientHandler:snd count=%d!\n"), snd_cnt));
        if (snd_cnt < 0) {
            ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%T) MedusaClientHandler:send error!\n")));
            return -1;
        }
        os.current_->rd_ptr(snd_cnt);
        size -= snd_cnt;
    } while (size > 0);

    return 0;
}

int MedusaBaseHandler::open(void *p)
{
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaClientHandler: open\n")));
    ACE_Time_Value iter_delay(2); // Two seconds
    if (super::open(p) == -1)
        return -1;
    //this->notifier_.reactor(this->reactor());
    //this->msg_queue()->notification_strategy(&this->notifier_);
    //int ret = this->reactor()->register_handler
        //(this, ACE_Event_Handler::READ_MASK);
    this->iterations_ = 0;
    m_begin = 0;
    m_header = 0;
    m_partialMessage = 0;
    this->notifier_.reactor(ACE_Reactor::instance()); //this->reactor ());
    this->msg_queue()->notification_strategy(&this->notifier_);
    int ret = this->reactor()->register_handler
        (this, ACE_Event_Handler::READ_MASK);
    /*return this->reactor ()->schedule_timer
      (this, 0, ACE_Time_Value::zero, iter_delay);*/
    return 0;
}

int MedusaBaseHandler::handle_input(ACE_HANDLE)
{
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Handle_input\n")));
    ssize_t recv_cnt;
    ssize_t readSize = 0;
    //new message
    ACE_Message_Block* buffer = 0;
    if (m_header == 0) {
        if (m_partialMessage == 0) {
            readSize = MedusaHeader::getSize();
            buffer = new ACE_Message_Block(readSize);
        } else {
            readSize = MedusaHeader::getSize() - m_partialMessage->length();
            buffer = new ACE_Message_Block(MedusaHeader::getSize());
            ACE_OS::memcpy(buffer->rd_ptr(), m_partialMessage->rd_ptr(), m_partialMessage->length());
            buffer->rd_ptr(m_partialMessage->length());
            delete m_partialMessage;
            m_partialMessage = 0;
        }
    } else {
        if (m_partialMessage == 0) {
            readSize = m_header->getBodySize();
            buffer = new ACE_Message_Block(readSize);
        } else {
            readSize = m_header->getBodySize();
            readSize -= m_partialMessage->length();
            buffer = new ACE_Message_Block(m_header->getBodySize());
            ACE_OS::memcpy(buffer->rd_ptr(), m_partialMessage->rd_ptr(), m_partialMessage->length());
            buffer->rd_ptr(m_partialMessage->length());
            delete m_partialMessage;
            m_partialMessage = 0;
        }
    }

    if ((recv_cnt = this->peer().recv(buffer->wr_ptr(), buffer->space())) <= 0) {
        ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T) Connection closed\n")));
        return -1;
    }
    buffer->wr_ptr(recv_cnt);
    //ACE_DEBUG((LM_DEBUG,
        //ACE_TEXT("(%T) MedusaServiceHandler::handle_input:%d\n"), recv_cnt));
    if (m_header == 0) {
        if (recv_cnt < readSize) {
            m_partialMessage = buffer;
            m_partialMessage->wr_ptr(recv_cnt);
        } else {//complete
            ByteInputStream bis(buffer);
            m_header = new MedusaHeader(bis);
            //ACE_DEBUG((LM_DEBUG,
                //ACE_TEXT("(%T)INFO: Got Header!\n")));
            if (!bis.good_bit()) {
                delete m_header;
                return -1;
            }
        }
        return 0;
    } else {
        if (recv_cnt < readSize) {
            m_partialMessage = buffer;
            m_partialMessage->wr_ptr(recv_cnt);
        } else {
            //complete
            ByteInputStream bis(buffer);
            MedusaPacket* packet = MedusaPacketFactory::createPacket(m_header, bis);
            //ACE_DEBUG((LM_DEBUG,
                //ACE_TEXT("(%T) Got Packet! Body size=%d\n"), packet->getPacketHeader()->getBodySize()));
            int handleRet = this->handlePacket(packet);
            if(handleRet != 0){
                return -1;
            }
            //this->send(packet);
            m_header = 0;
        }
        //ACE_DEBUG((LM_DEBUG,
            //ACE_TEXT("(%T) MedusaServiceHandler::handle_input: end\n")));
        return 0;
    }
}

int MedusaBaseHandler::handle_timeout(const ACE_Time_Value &, const void *)
{
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaClientHandler:handle_timeout - timeout (%d,%d)\n"), this->iterations_, MedusaBaseHandler::ITERATIONS));
    if (++this->iterations_ >= MedusaBaseHandler::ITERATIONS) {
        this->peer().close_writer();
        return 0;
    }

    ACE_Message_Block *mb;
    ACE_NEW_RETURN(mb, ACE_Message_Block(128), -1);
    int nbytes = ACE_OS::sprintf(mb->wr_ptr(), "Iteration %d\n", this->iterations_);
    ACE_ASSERT(nbytes > 0);
    mb->wr_ptr(static_cast<size_t> (nbytes));
    this->putq(mb);
    return 0;
}

int MedusaBaseHandler::handle_output(ACE_HANDLE)
{
    ACE_Message_Block *mb;
    ACE_Time_Value nowait(ACE_OS::gettimeofday());
    while (-1 != this->getq(mb, &nowait)) {
        ssize_t send_cnt =
            this->peer().send(mb->rd_ptr(), mb->length());
        if (send_cnt == -1)
            ACE_ERROR((LM_ERROR,
            ACE_TEXT("(%T) %@\n"),
            ACE_TEXT("(%T)send")));
        else
            mb->rd_ptr(static_cast<size_t> (send_cnt));
        if (mb->length() > 0) {
            this->ungetq(mb);
            break;
        }
        mb->release();
    }
    if (this->msg_queue()->is_empty())
        this->reactor()->cancel_wakeup
        (this, ACE_Event_Handler::WRITE_MASK);
    else
        this->reactor()->schedule_wakeup
        (this, ACE_Event_Handler::WRITE_MASK);
    return 0;
}


