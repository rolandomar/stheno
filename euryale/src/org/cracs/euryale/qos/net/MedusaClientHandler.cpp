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
 * File:   MedusaClientHandler.cpp
 * Author: rmartins
 * 
 * Created on December 16, 2009, 3:28 PM
 */



#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_errno.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_sys_time.h>
#include "MedusaClientHandler.h"
//#include <euryale/common/io/RDRStream.h>
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <euryale/qos/net/packet/MedusaPacketFactory.h>

MedusaClientHandler::~MedusaClientHandler() {

}

int MedusaClientHandler::send(MedusaPacket* packet) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    //OutputStream* os = ObjectStreamFactory::createOutputStream(packet->getSerializationSize());
    //ByteOutputStream os(packet->getSerializationSize());
    MedusaHeader *header = static_cast<MedusaHeader*> (packet->getPacketHeader());
    if (header->getPacketID() == 0) {
        header->m_requestID = ++m_sequenceID;
    }
    StreamSize ss;
    packet->serialize(ss);
    ByteOutputStream os(ss.total_length());
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MedusaClientHandler: before ser PACKET ID=%d\n"), header->m_requestID));
    packet->serialize(os);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MedusaClientHandler: after\n")));
    ACE_Message_Block mb(RDR::total_length(&os.start_, 0));
    RDR::consolidate(&mb, &os.start_);
    int size = mb.length();
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MedusaClientHandler: after 2 BodySize=%d SIZE=%d\n"), packet->getPacketHeader()->getBodySize(), size));
    int snd_cnt = 0;
    do {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MedusaClientHandler: after 3\n")));
        snd_cnt = super::peer().send(mb.rd_ptr(), size, 0);
        //ACE_DEBUG((LM_DEBUG,
                //ACE_TEXT("(%T) MedusaClientHandler:snd count=%d!\n"), snd_cnt));
        if (snd_cnt < 0) {
            ACE_DEBUG((LM_DEBUG,
                    ACE_TEXT("(%T) MedusaClientHandler:send error!\n")));
            return -1;
        }
        mb.rd_ptr(snd_cnt);
        size -= snd_cnt;
    } while (size > 0);

    return 0;
}

int MedusaClientHandler::open(void *p) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MedusaClientHandler: open\n")));
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
    this->notifier_.reactor(this->reactor());
    this->msg_queue()->notification_strategy(&this->notifier_);
    int ret = this->reactor()->register_handler
            (this, ACE_Event_Handler::READ_MASK);
    InitMedusaClientPacket* initPacket = new InitMedusaClientPacket(m_uuid);
    return this->send(initPacket);

    /*return this->reactor ()->schedule_timer
      (this, 0, ACE_Time_Value::zero, iter_delay);*/
    //return 0;
}

int MedusaClientHandler::handle_input(ACE_HANDLE) {
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
	  //readSize = MedusaHeader::getSize() - m_partialMessage->length();
            //buffer = new ACE_Message_Block(MedusaHeader::getSize());
            //ACE_OS::memcpy(buffer->rd_ptr(), m_partialMessage->rd_ptr(), m_partialMessage->length());
            //buffer->rd_ptr(m_partialMessage->length());
            //delete m_partialMessage;
            //m_partialMessage = 0;
	    readSize = MedusaHeader::getSize() - m_partialMessage->length();
            buffer = new ACE_Message_Block(MedusaHeader::getSize());
	    size_t total_size = m_partialMessage->wr_ptr() - m_partialMessage->base();
            readSize-=total_size;
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
	  //readSize = m_header->getBodySize();
            //readSize -= m_partialMessage->length();
            //buffer = new ACE_Message_Block(m_header->getBodySize());
            //ACE_OS::memcpy(buffer->rd_ptr(), m_partialMessage->rd_ptr(), m_partialMessage->length());
            //buffer->rd_ptr(m_partialMessage->length());
            //delete m_partialMessage;
            //m_partialMessage = 0;
	    readSize = m_header->getBodySize();
            buffer = new ACE_Message_Block(m_header->getBodySize());            
            size_t total_size = m_partialMessage->wr_ptr() - m_partialMessage->base();
            readSize-=total_size;
	    ACE_OS::memcpy(buffer->base(), m_partialMessage->base(), total_size);            
            buffer->wr_ptr(total_size);
	    ACE_Message_Block::release(m_partialMessage);
            m_partialMessage = 0;
        }
    }

    /*if ((recv_cnt = this->peer().recv(buffer->wr_ptr(), buffer->space())) <= 0) {
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
	  //m_partialMessage = buffer;
	  // m_partialMessage->wr_ptr(recv_cnt);
	  m_partialMessage = buffer;
	  buffer = 0;
        } else {//complete
	  /*ByteInputStream bis(buffer);
            m_header = new MedusaHeader(bis);
            //ACE_DEBUG((LM_DEBUG,
                    //ACE_TEXT("(%T)INFO: Got Header!\n")));
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
	    MedusaPacket* packet = MedusaPacketFactory::createPacket(m_header, bis); //PACKET_FACTORY::createPacket(m_header, bis);
	    if (this->handlePacket(packet) != 0) {
	      return -1;
            }
	    m_header = 0;
	  }
        }
        return 0;
    } else {
        if (recv_cnt < readSize) {
	  //m_partialMessage = buffer;
            //m_partialMessage->wr_ptr(recv_cnt);
	  m_partialMessage = buffer;
	  buffer = 0;
        } else {
	  /*//complete
            ByteInputStream bis(buffer);
            MedusaPacket* packet = MedusaPacketFactory::createPacket(m_header, bis);
            //this->send(packet);
            if (this->handlePacket(packet) != 0) {
                return -1;
            }
            m_header = 0;*/
	  //complete
	  ByteInputStream bis(buffer);
	  ACE_Message_Block::release(buffer);
	  buffer = 0;
	  MedusaPacket* packet = MedusaPacketFactory::createPacket(m_header, bis); //PACKET_FACTORY::createPacket(m_header, bis);
	  if (this->handlePacket(packet) != 0) {
	    return -1;
	  }
	  m_header = 0;
        }
        //ACE_DEBUG((LM_DEBUG,
                //ACE_TEXT("(%T) MedusaServiceHandler::handle_input: end\n")));
        return 0;
    }
}

int MedusaClientHandler::handlePacket(MedusaPacket* packet) {
    MedusaHeader *header = static_cast<MedusaHeader*> (packet->getPacketHeader());
    //ACE_DEBUG((LM_DEBUG,
            //ACE_TEXT("(%T) Got Packet! Type=%d Body size=%d\n"), header->getType(), header->getBodySize()));
    ACE_Future<MedusaPacket*>* future = 0;
    switch (header->getType()) {
        case GET_PARTITION_REPLY_PACKET_ID:
        {
            GetPartitionReplyPacket* packetImpl = static_cast<GetPartitionReplyPacket*> (packet);
	    {
	      ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
	      future = m_pendingRequests[packetImpl->getRequestID()];
	      m_pendingRequests.erase(packetImpl->getRequestID());
	    }
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)===================== %d %d %@\n"), header->m_requestID, packetImpl->getRequestID(), future));
            if (future != 0) {
                future->set(packet);
            }else{
	      delete packet;
	    }
            break;
        }
        case CPUGROUP_RESERVE_REPLY_PACKET_ID:
        {
            CPUGroupReserveReplyPacket* packetImpl = static_cast<CPUGroupReserveReplyPacket*> (packet);
	    {
	      ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
	      future = m_pendingRequests[packetImpl->getRequestID()];
	      m_pendingRequests.erase(packetImpl->getRequestID());
	    }
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)===================== %d %d %@\n"), header->m_requestID, packetImpl->getRequestID(), future));
            if (future != 0) {
                future->set(packet);
            }else{
	      delete packet;
	    }
            break;
        }
        case THREAD_RESERVE_REPLY_PACKET_ID:
        {
            ThreadReserveReplyPacket* packetImpl = static_cast<ThreadReserveReplyPacket*> (packet);
	    {
	      ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
	      future = m_pendingRequests[packetImpl->getRequestID()];
	      m_pendingRequests.erase(packetImpl->getRequestID());
	    }
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)===================== %d %d %@\n"), header->m_requestID, packetImpl->getRequestID(), future));
            if (future != 0) {
                future->set(packet);
            }else{
	      delete packet;
	    }
            break;
        }
        default:
            delete packet;

    }
    return 0;
}

int MedusaClientHandler::handle_timeout(const ACE_Time_Value &, const void *) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MedusaClientHandler:handle_timeout - timeout (%d,%d)\n"), this->iterations_, MedusaClientHandler::ITERATIONS));
    if (++this->iterations_ >= MedusaClientHandler::ITERATIONS) {
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

int MedusaClientHandler::handle_output(ACE_HANDLE) {
    ACE_Message_Block *mb;
    ACE_Time_Value nowait(ACE_OS::gettimeofday());
    while (-1 != this->getq(mb, &nowait)) {
        ssize_t send_cnt =
                this->peer().send(mb->rd_ptr(), mb->length());
        if (send_cnt == -1) {
            ACE_ERROR((LM_ERROR,
                    ACE_TEXT("(%T) %@\n"),
                    ACE_TEXT("(%T)send")));
            delete mb;
            return -1;
        } else {
            mb->rd_ptr(static_cast<size_t> (send_cnt));
        }
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

/*int ACE_TMAIN (int, ACE_TCHAR *[])
{
  //ACE_INET_Addr port_to_connect (ACE_TEXT ("HAStatus"), ACE_LOCALHOST);
    ACE_INET_Addr port_to_connect (55555, ACE_LOCALHOST);
  ACE_Connector<MedusaClientHandler, ACE_SOCK_CONNECTOR> connector;
  MedusaClientHandler client;
  MedusaClientHandler *pc = &client;
  if (connector.connect (pc, port_to_connect) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%@\n"),
                       ACE_TEXT ("connect")), 1);

  ACE_Reactor::instance ()->run_reactor_event_loop ();
  return (0);
}*/


list<CPUPartition*>* MedusaClientHandler::getCPUPartitionList() throw (CgroupException&) {
  ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    UInt id = getNextSequenceID();
    GetPartitionRequestPacket packet(id);
    ACE_Future<MedusaPacket*> future;
    m_pendingRequests[id] = &future;
    ace_mon.release();
    if (this->send(&packet) != 0) {
        return 0;
    }
    MedusaPacket* replyPacketAbs = 0;
    if (future.get(replyPacketAbs, 0) != 0) {
      {
	ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,0);
        m_pendingRequests.erase(id);
      }
        return 0;
    }
    {
      ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
      m_pendingRequests.erase(id);
    }
    GetPartitionReplyPacket* replyPacket = static_cast<GetPartitionReplyPacket*> (replyPacketAbs);
    list<CPUPartition*>* ret = new list<CPUPartition*>();
    list<CPUPartition*>::iterator iter;
    iter = replyPacket->getCPUPartitions().begin();
    while (iter != replyPacket->getCPUPartitions().end()) {
        cout << "Partition=" << (*iter)->toString() << endl;
        ret->push_back(*iter);
        iter++;
    }
    replyPacket->getCPUPartitions().clear();
    delete replyPacket;
    return ret;
}

void MedusaClientHandler::createRuntimeCPUReserve(//ACE_CString& cpusetGroup,
        ACE_CString& scheDomain, CPUGroupQoS& qos) throw (CgroupException&) {
  ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    UInt id = getNextSequenceID();
    CPUGroupReserveRequestPacket packet(scheDomain, id, qos);
    ACE_Future<MedusaPacket*> future;
    m_pendingRequests[id] = &future;
    ace_mon.release();
    if (this->send(&packet) != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)failed to send\n")));
        return;
    }
    MedusaPacket* replyPacketAbs = 0;
    if (future.get(replyPacketAbs, 0) != 0) {
      {
	ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
        m_pendingRequests.erase(id);
      }
        throw new CgroupException("Operation failed!");
    }
    {
      ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
      m_pendingRequests.erase(id);
    }
    CPUGroupReserveReplyPacket* replyPacket = static_cast<CPUGroupReserveReplyPacket*> (replyPacketAbs);
    if (replyPacket->getReply() == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)failed operation\n")));
        throw new CgroupException("Operation failed!");
    }
    delete replyPacket;
    return;
}

void MedusaClientHandler::deleteCPUGroupReserve(ACE_CString& cpuGroup) throw (CgroupException&) {
    return;
}

void MedusaClientHandler::setTaskInCPUPartition(UInt task, ACE_CString& cpusetGroup,
        ACE_CString& scheDomain, CPUQoS* qos) throw (CgroupException&) {
  //ACE_Time_Value start = ACE_OS::gettimeofday();
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    UInt id = getNextSequenceID();
    ThreadReserveRequestPacket packet(id, task, cpusetGroup, scheDomain, qos);
    ACE_Future<MedusaPacket*> future;
    m_pendingRequests[id] = &future;
    ace_mon.release();
    if (this->send(&packet) != 0) {
        return;
    }
    MedusaPacket* replyPacketAbs = 0;
    
    if (future.get(replyPacketAbs, 0) != 0) {
      {
	ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
        m_pendingRequests.erase(id);
      }
        throw new CgroupException("Operation failed!");
    }
    {
      ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
      m_pendingRequests.erase(id);
    }
    ThreadReserveReplyPacket* replyPacket = static_cast<ThreadReserveReplyPacket*> (replyPacketAbs);
    if (replyPacket->getReply() == -1) {
      delete replyPacket;
        throw new CgroupException("Operation failed!");
    }
    delete replyPacket;
    //ACE_Time_Value end = ACE_OS::gettimeofday();
    //end -= start;
    //ACE_UINT64 usecs;
    //end.to_usec(usecs);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MedusaClientHandler::setTaskInCPUPartition time=%d\n"),usecs));
    return;
}

/**
 * Creates a cpu schedule domain (aka cpu cgroup) reservation group
 * @param cpuGroup Name of the reservation group to be created
 * @return 0 sucess; -1 error
 */
void MedusaClientHandler::createCPUGroupReservation(ACE_CString& cpuGroup) throw (CgroupException&) {
    return;
}

void MedusaClientHandler::deleteCPUGroupReservation(ACE_CString& cpuGroup) throw (CgroupException&) {
    return;
}

/**
 * Retrieves the tasks numbers in a given cgroup.
 * @param cpuGroup CGroup name
 * @return List of tasks currently assigned to the given cgroup.
 */
list<UInt>* MedusaClientHandler::getTasksInCPUGroup(ACE_CString& cpuGroup) throw (CgroupException&) {
    return 0;
}

int MedusaClientHandler::close(u_long flags) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (reactor() != 0) {        
        reactor()->remove_handler(this, flags);
    }
    this->peer().close();
    return 0;
}

int MedusaClientHandler::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask mask) {            
    m_listener->onMedusaClientHandlerClose();
    close();
    //this->peer().close();
    //return ACE_Svc_Handler<ACE_LSOCK_STREAM, ACE_MT_SYNCH>::handle_close(handle, mask);
    return 0;
}
