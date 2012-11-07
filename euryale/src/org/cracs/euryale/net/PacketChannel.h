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

#ifndef _PACKETCHANNEL_H
#define	_PACKETCHANNEL_H

#include <euryale/net/AbstractStreamChannel.h>
#include <euryale/common/io/ByteInputStream.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <euryale/net/packet/Packet.h>
#include <euryale/net/packet/PacketFactory.h>

template <class HEADER, class PACKET>
class WriteBlockPacket : public WriteBlock {
public:

    typedef ACE_Strong_Bound_Ptr<WriteBlockPacket<HEADER, PACKET>, ACE_Recursive_Thread_Mutex> WriteBlockPacketPtr;

    WriteBlockPacket(PACKET* packet, ACE_Time_Value* timeout) :
    WriteBlock(timeout) {
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
    }

    virtual ~WriteBlockPacket() {
        if (m_mb != 0) {
            delete m_mb;
        }
    }
};

template <class PACKET, class HEADER, //class PACKET_FACTORY,
class SOCK_TYPE, class SYNC_TYPE>
class PacketChannel : public AbstractStreamChannel<SOCK_TYPE, SYNC_TYPE> {
public:
    typedef AbstractStreamChannel<SOCK_TYPE, SYNC_TYPE> SuperType;
    typedef typename SuperType::CloseListenerType CloseListenerType;
    static const int MAX_BUFFER_SIZE = 1024;
    static const int CONSOLIDATE_LIMIT = 768;

    PacketChannel() :
    SuperType(),
    /*m_begin(0), m_current(0), */m_header(0), m_partialMessage(0), m_factory(0) {
        m_partialMessage = new ACE_Message_Block(MAX_BUFFER_SIZE);
        m_debugPacketChannel = RuntimeConfiguration::isClassDebugged("PacketChannel");
    }

    PacketChannel(PacketFactory<PACKET, HEADER>* factory, bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListenerType* closeListener = 0) :
    SuperType(asyncRead, asyncWrite, ec, reserve, qos, closeListener),
    m_factory(factory), /*m_begin(0), m_current(0),*/ m_header(0), m_partialMessage(0) {
        m_partialMessage = new ACE_Message_Block(MAX_BUFFER_SIZE);
        m_debugPacketChannel = RuntimeConfiguration::isClassDebugged("PacketChannel");
    }

    virtual ~PacketChannel() {
        //~AbstractChannel<SOCK_TYPE, SYNC_TYPE>();
        if (m_debugPacketChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)PacketChannel::~PacketChannel() this=%@\n"), this));
        }
        if (m_factory != 0) {
            delete m_factory;
        }
        ACE_Message_Block::release(m_partialMessage);
        if (m_header != 0) {
            delete m_header;
        }
    }

    virtual int sendPacket(PACKET* packet, ACE_Time_Value* timeout = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, -1);
        ACE_Message_Block* mb = serializePacket(packet);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)PacketChannel::send() %d\n"), mb->length()));
        int ret = SuperType::send(mb, timeout);
        ACE_Message_Block::release(mb);
        return ret;
    }

    //virtual int sendPacketAsync(WriteBlockPacket<HEADER, PACKET>::WriteBlockPacketPtr* wb) {

    virtual int sendPacketAsync(WriteBlockPtr* wb) {
        return SuperType::sendAsync(wb);
    }

    /*virtual int receivePacket(PACKET*& packet, ACE_Time_Value* timeout) {
        if (SuperType::hasAsyncRead()) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel:receivePacket() hasAsyncRead failover to -1\n")));
            return -1;
        } else {
            ACE_Time_Value absTimeout = ACE_OS::gettimeofday();
            if (timeout != 0) {
                absTimeout += *timeout;
            }
            while (packet == 0) {
                if (timeout != 0) {
                    ACE_Time_Value now = ACE_OS::gettimeofday();
                    if (now > absTimeout) {
                        //if (m_debugPacketChannel) {
                            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel:receivePacket() timeout\n")));
                        //}
                        return -1;
                    }
                }
                //if (m_debugPacketChannel) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel:receivePacket() before read %@\n"), this));
                //}
                int ret = handleInputParsePacket(packet, 0);
                //if (m_debugPacketChannel) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)PacketChannel:receivePacket() after read %@ packet=%@\n"), this, packet));
                //}
                if (ret == -1) {
                    //if (m_debugPacketChannel) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel:receivePacket() ret == -1, calling close()\n")));
                    //}
                    SuperType::close();
                    return -1;
                }

            }
            return 0;
        }
    }*/
    int receivePacket(PACKET*& packet, ACE_Time_Value* timeout) {
        if (SuperType::hasAsyncRead()) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel:receivePacket() hasAsyncRead failover to -1\n")));
            return -1;
        } else {
            ACE_Time_Value absTimeout = ACE_OS::gettimeofday();
            if (timeout != 0) {
                absTimeout += *timeout;
            }
            //ACE_Time_Value start = ACE_OS::gettimeofday();
            while (packet == 0) {
                if (timeout != 0) {
                    ACE_Time_Value now = ACE_OS::gettimeofday();
                    if (now > absTimeout) {
                        //if (m_debugPacketChannel) {
                        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel:receivePacket() timeout\n")));
                        //}
                        return -1;
                    }
                }
                //if (m_debugPacketChannel) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel:receivePacket() before read %@\n"), this));
                //}
                int ret = handleInputParsePacket(packet, 0);
                //if (m_debugPacketChannel) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)PacketChannel:receivePacket() after read %@ packet=%@\n"), this, packet));
                //}
                if (ret == -1) {
                    //if (m_debugPacketChannel) {
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel:receivePacket() ret == -1, calling close()\n")));
                    //}
                    SuperType::close();
                    return -1;
                }
                if (packet != 0) {
                    //ACE_Time_Value end = ACE_OS::gettimeofday() - start;
                    //ULongLong deltausec;
                    //end.to_usec(deltausec);
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)TRACE: PacketChannel::receivePacket time=%Q\n"), deltausec));
                    return 0;
                }

                if (readInputBuffer(timeout) == -1) {
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel:receivePacket() readInputBuffer == -1, calling close()\n")));
                    SuperType::close();
                    return -1;
                }
                //start = ACE_OS::gettimeofday();

            }
            return 0;
        }
    }

    virtual int asynchronous(bool readAsync, bool writeAsync) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, -1);
        if (readAsync) {
            PACKET* packet = 0;
            do {
                packet = 0;
                int ret = handleInputParsePacket(packet, 0);
                if (ret == -1) {
                    ace_mon.release();
                    close(0);
                    return -1;
                }
                if (packet == 0) {
                    break;
                } else {
                    ACE_DEBUG((LM_DEBUG,
                            ACE_TEXT("(%t|%T)ERROR: PacketChannel::asynchronous() flushing type=%d\n"),
                            packet->getPacketHeader()->getType()
                            ));
                    if (handlePacket(packet) == -1) {
                        return -1;
                    }
                }

            } while (packet != 0);
        }
        return SuperType::asynchronous(readAsync, writeAsync);
    }

protected:
    bool m_debugPacketChannel;

    virtual PACKET* createPacket(HEADER* header, ByteInputStream& bis);
    virtual int handlePacket(PACKET* packet) = 0;
    virtual int handleInput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int handleInputParsePacket(PACKET*& packet, ACE_Time_Value* timeout);

    int readInputBuffer(ACE_Time_Value* timeout) {
        //ACE_Time_Value start = ACE_OS::gettimeofday();
        ssize_t readSize = m_partialMessage->space(); //MAX_BUFFER_SIZE
        ssize_t recv_cnt = 0;
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::readInputBuffer() -before read readSize=%d buffer_space=%d len=%d\n"),
        //        readSize,
        //        m_partialMessage->space(), m_partialMessage->length()));
        if ((recv_cnt = SuperType::peer().recv(m_partialMessage->wr_ptr(), readSize, timeout)) <= 0) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::readInputBuffer() - < -1\n")));
            return -1;
        }
        //if (m_debugPacketChannel) {

        //}
        m_partialMessage->wr_ptr(recv_cnt);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::readInputBuffer() - %d buffer_len=%d\n"), recv_cnt, m_partialMessage->length()));
        //ACE_Time_Value end = ACE_OS::gettimeofday() - start;
        //ULongLong deltausec;
        //end.to_usec(deltausec);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)TRACE: PacketChannel::readInputBuffer time=%Q\n"), deltausec));

        return recv_cnt;
    }

    virtual ACE_Message_Block* serializePacket(PACKET* packet) {
        HEADER *header = static_cast<HEADER*> (packet->getPacketHeader());
        if (header->getPacketID() == 0) {
            header->setRequestID(++m_sequenceID);
        }
        //StreamSize ss;
        //packet->serialize(ss);
        //ByteOutputStream os(ss.total_length());
        ByteOutputStream os;
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)serializePacket: before ser PACKET ID=%d\n"), header->m_requestID));
        packet->serialize(os);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)serializePacket: after %d RDR=%d ss.total_length()=%d\n"),os.total_length(),RDR::total_length(&os.start_, 0),ss.total_length()));
        ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
        RDR::consolidate(mb, &os.start_);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)serializePacket: len=%d RDR_len=%d RDR_cont=%@\n"),mb->length(),os.start_.length(),os.start_.cont()));
        //ACE_Message_Block* mb = os.start_.duplicate();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)serializePacket: after MB=%d total=%d\n"),mb->length(),mb->total_length()));
        return mb;

    }
    PacketFactory<PACKET, HEADER>* m_factory;
    //ACE_Message_Block* m_begin;
    //ACE_Message_Block* m_current;
    HEADER* m_header;
    ACE_Message_Block* m_partialMessage;
    ACE_Atomic_Op<ACE_Thread_Mutex, UInt> m_sequenceID;
};

template <class PACKET, class HEADER, class SOCK_TYPE, class SYNC_TYPE>
int PacketChannel<PACKET, HEADER, SOCK_TYPE, SYNC_TYPE>::handleInput_i(ACE_HANDLE fd) {
    //ACE_Time_Value start = ACE_OS::gettimeofday();
    if (readInputBuffer(0) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)------------------>PacketChannel: handle_input() readInputBuffer -1\n")));
        return -1;
    }
    PACKET* packet = 0;
    do {
        packet = 0;
        int ret = handleInputParsePacket(packet, 0);
        if (ret == -1) {
            return -1;
        }
        if (packet == 0) {
            //ACE_Time_Value end = ACE_OS::gettimeofday() - start;
            //ULongLong deltausec;
            //end.to_usec(deltausec);
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)TRACE: PacketChannel::handleInput_i() - missing data time=%Q\n"), deltausec));
            return ret;
        } else {
            if (handlePacket(packet) == -1) {
                return -1;
            }
            //ACE_Time_Value end = ACE_OS::gettimeofday() - start;
            //ULongLong deltausec;
            //end.to_usec(deltausec);
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)TRACE: PacketChannel::handleInput_i() - after handlePacket time=%Q\n"), deltausec));
            //start = ACE_OS::gettimeofday();
        }

    } while (packet != 0);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel:handle_input() - handlePacket no more packets\n")));
    //ACE_Time_Value end = ACE_OS::gettimeofday() - start;
    //ULongLong deltausec;
    //end.to_usec(deltausec);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)TRACE: PacketChannel::handleInput_i() - nos packets time=%Q\n"), deltausec));
    return 0;
}

/*template <class PACKET, class HEADER, class SOCK_TYPE, class SYNC_TYPE>
int PacketChannel<PACKET, HEADER, SOCK_TYPE, SYNC_TYPE>::handleInput_i(ACE_HANDLE fd) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: ------------------>PacketChannel: handle_input()\n")));
    PACKET* packet = 0;
    int ret = handleInputParsePacket(packet, 0);
    if (ret == -1) {
        if (m_debugPacketChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::handle_input() - -1 on packet fetch this=%@\n"), this));
        }
        return -1;
    }
    if (packet == 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)------------------>PacketChannel: handle_input() packet==0?\n");
        return ret;
    } else {
        //if (SuperType::m_hasInputQueue) {
        //    ACE_Message_Block* mbMeta = new ACE_Message_Block(sizeof (PACKET*));
        //    ACE_OS::memcpy(mbMeta->base(), &packet, sizeof (PACKET*));
       //     ACE_Time_Value now = ACE_OS::gettimeofday();
       //     return SuperType::m_inputQueue.enqueue_tail(mbMeta, &now);
        //} else {
        if (handlePacket(packet) == -1) {
            if (m_debugPacketChannel) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel:handle_input() - handlePacket failed -1\n")));
            }
            return -1;
        }
        return 0;
        //}

    }

}*/

template <class PACKET, class HEADER, class SOCK_TYPE, class SYNC_TYPE>
PACKET* PacketChannel<PACKET, HEADER, SOCK_TYPE, SYNC_TYPE>::createPacket(HEADER* header, ByteInputStream& bis) {
    //return PACKET_FACTORY::createPacket(m_header, bis);
    return m_factory->createPacket(m_header, bis);
}

template <class PACKET, class HEADER, class SOCK_TYPE, class SYNC_TYPE>
int PacketChannel<PACKET, HEADER, SOCK_TYPE, SYNC_TYPE>::
handleInputParsePacket(PACKET*& inputPacket, ACE_Time_Value* timeout) {
    //ACE_Time_Value start = ACE_OS::gettimeofday();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: PacketChannel::Handle_input\n")));    
    ssize_t readSize = m_partialMessage->length();
    inputPacket = 0;

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::handleInputParsePacket() - buffer =%d space=%d\n"), readSize, m_partialMessage->space()));

    if (readSize == 0) {
        //ACE_DEBUG((LM_DEBUG,
        //ACE_TEXT("(%t|%T)AAAAAAAAAAAa TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"),
        //m_partialMessage->length()));
        return 0;
    }

    ByteInputStream bis(m_partialMessage);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::handleInputParsePacket() - at begin buffer =%d\n"), readSize));
    if (m_header == 0) {
        //ACE_DEBUG((LM_DEBUG,
        //ACE_TEXT("(%t|%T)BBBBBBBBBB TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"),
        //m_partialMessage->length()));
        if (readSize < HEADER::getSize()) {
            //ACE_DEBUG((LM_DEBUG,
            //        ACE_TEXT("(%t|%T)CCCCCCCCCCCCcc TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"),
            //       m_partialMessage->length()));
            return 0;
        } else {//complete                                    
            //ACE_DEBUG((LM_DEBUG,
            //        ACE_TEXT("(%t|%T)FFFFFFFFFFFFFFf TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"),
            //        m_partialMessage->length()));
            m_header = new HEADER(bis);
            if (!bis.good_bit()) {
                // ACE_DEBUG((LM_DEBUG,
                //         ACE_TEXT("(%t|%T)VVVVVVVVVVVVVVvv TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"),
                //         m_partialMessage->length()));
                //if (m_debugPacketChannel) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::handleInputParsePacket() - !bis.good_bit()\n")));
                //}
                delete m_header;
                return -1;
            }
            ssize_t newLen = readSize - bis.start()->length();
            m_partialMessage->rd_ptr(newLen);
            readSize = m_partialMessage->length();
            //printf("**************BODY=%d readSize=%d\n", m_header->getBodySize(), readSize);
            /*if (readSize == 0) {
                //ACE_DEBUG((LM_DEBUG,
                //ACE_TEXT("(%t|%T)QQQQQQQQQQQQQq TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"),
                //m_partialMessage->length()));
                m_partialMessage->rd_ptr(m_partialMessage->base());
                m_partialMessage->wr_ptr(m_partialMessage->base());
            }*/

            if (m_header->getBodySize() == 0) {
                //ACE_DEBUG((LM_DEBUG,
                //ACE_TEXT("(%t|%T)jjjjjjjjjjjjj TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"),
                //m_partialMessage->length()));
                PACKET* packet = createPacket(m_header, bis); //PACKET_FACTORY::createPacket(m_header, bis);
                ssize_t newLen = readSize - bis.start()->length();
                m_partialMessage->rd_ptr(newLen);
                readSize = m_partialMessage->length();
                inputPacket = packet;
                m_header = 0;
                //ACE_Message_Block *b = new ACE_Message_Block(m_partialMessage->size());
                //RDR::consolidate(b, m_partialMessage);
                //ACE_Message_Block::release(m_partialMessage);
                //m_partialMessage = b;
                ACE_Message_Block *buffer = new ACE_Message_Block(m_partialMessage->size());
                //RDR::consolidate(b, m_partialMessage);                    
                ACE_OS::memcpy(buffer->base(), m_partialMessage->rd_ptr(), m_partialMessage->length());
                buffer->wr_ptr(m_partialMessage->length());
                ACE_Message_Block::release(m_partialMessage);
                m_partialMessage = buffer;
                //RDR::consolidate(m_partialMessage, m_partialMessage);

                return 0;
            }

            if (m_header->getBodySize() > readSize) {
                //ACE_DEBUG((LM_DEBUG,
                //        ACE_TEXT("(%t|%T)TTTTTTTTTTTTTTTt TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d %d\n"),
                ////         readSize,
                //        m_partialMessage->space(),
                //         (m_header->getBodySize() - readSize)
                //        ));
                if ((m_header->getBodySize() - readSize) > m_partialMessage->space()) {
                    size_t bSize = HEADER::getSize() + m_header->getBodySize() + MAX_BUFFER_SIZE;

                    ACE_Message_Block* buffer = new ACE_Message_Block(bSize);
                    ACE_OS::memcpy(buffer->base(), m_partialMessage->rd_ptr(), m_partialMessage->length());
                    buffer->wr_ptr(m_partialMessage->length());
                    //RDR::grow(m_partialMessage, m_header->getBodySize() + m_partialMessage->space());
                    //RDR::consolidate(b, m_partialMessage);
                    ACE_Message_Block::release(m_partialMessage);
                    m_partialMessage = buffer;
                    //printf("****newSize=%d newlenght after consolidate=%d space=%d\n", bSize, m_partialMessage->length(), m_partialMessage->space());
                } else {
                    //if (m_partialMessage->space() < CONSOLIDATE_LIMIT) {
                    //ACE_DEBUG((LM_DEBUG,
                    //         ACE_TEXT("(%t|%T)EEEEEEEEE TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"),
                    //        m_partialMessage->length()));
                    ACE_Message_Block *buffer = new ACE_Message_Block(m_partialMessage->size());
                    //RDR::consolidate(b, m_partialMessage);                    
                    ACE_OS::memcpy(buffer->base(), m_partialMessage->rd_ptr(), m_partialMessage->length());
                    buffer->wr_ptr(m_partialMessage->length());
                    ACE_Message_Block::release(m_partialMessage);
                    m_partialMessage = buffer;
                    //}
                }
            }


        }
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::handleInputParsePacket() - BODY SECTION buffer =%d packetbody=%d\n"), readSize, m_header->getBodySize()));
    if (m_partialMessage->length() < m_header->getBodySize()) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d\n"), m_partialMessage->length(), m_header->getBodySize()));
        return 0;
    } else {
        PACKET* packet = createPacket(m_header, bis); //PACKET_FACTORY::createPacket(m_header, bis);
        ssize_t newLen = readSize - bis.start()->length();
        m_partialMessage->rd_ptr(newLen);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::handleInputParsePacket() - packet is finshed,with body %d bopdy=%d\n"), m_partialMessage->length(), m_header->getBodySize()));
        inputPacket = packet;
        m_header = 0;
        /*if (readSize == 0) {
            m_partialMessage->rd_ptr(m_partialMessage->base());
            m_partialMessage->wr_ptr(m_partialMessage->base());
        } else {*/
        //RDR::consolidate(m_partialMessage, m_partialMessage);
        //if (m_partialMessage->space() < CONSOLIDATE_LIMIT) {
        //ACE_Message_Block *b = new ACE_Message_Block(m_partialMessage->size());
        //RDR::consolidate(b, m_partialMessage);
        //ACE_Message_Block::release(m_partialMessage);
        //m_partialMessage = b;
        ACE_Message_Block *buffer = new ACE_Message_Block(m_partialMessage->size());
        //RDR::consolidate(b, m_partialMessage);                    
        ACE_OS::memcpy(buffer->base(), m_partialMessage->rd_ptr(), m_partialMessage->length());
        buffer->wr_ptr(m_partialMessage->length());
        ACE_Message_Block::release(m_partialMessage);
        m_partialMessage = buffer;

        //ACE_DEBUG((LM_DEBUG,
        //       ACE_TEXT("(%t|%T)DDDDDDD4444444444D TRACE: PacketChannel::handleInputParsePacket() - body < len buf=%d body=%d space=%d\n"),
        //       m_partialMessage->length(), m_partialMessage->space()));
        //}
        //}
        return 0;
    }


}


/*template <class PACKET, class HEADER, class SOCK_TYPE, class SYNC_TYPE>
int PacketChannel<PACKET, HEADER, SOCK_TYPE, SYNC_TYPE>::
handleInputParsePacket(PACKET*& inputPacket, ACE_Time_Value* timeout) {

    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: PacketChannel::Handle_input\n")));
    ssize_t recv_cnt;
    ssize_t readSize = 0;
    inputPacket = 0;
    //new message
    ACE_Message_Block* buffer = 0;
    if (m_header == 0) {
        if (m_partialMessage == 0) {
            readSize = HEADER::getSize();
            buffer = new ACE_Message_Block(readSize);
        } else {
            readSize = HEADER::getSize() - m_partialMessage->length();
            buffer = new ACE_Message_Block(HEADER::getSize());
            size_t total_size = m_partialMessage->wr_ptr() - m_partialMessage->base();
            readSize = HEADER::getSize() - total_size;
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
            readSize = m_header->getBodySize();
            buffer = new ACE_Message_Block(m_header->getBodySize());
            size_t total_size = m_partialMessage->wr_ptr() - m_partialMessage->base();
            readSize = m_header->getBodySize() - total_size;
            ACE_OS::memcpy(buffer->base(), m_partialMessage->base(), total_size);
            buffer->wr_ptr(total_size);
            ACE_Message_Block::release(m_partialMessage);
            m_partialMessage = 0;
        }
    }
    //if (m_debugPacketChannel) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel::handleInputParsePacket() - before readsize=%d %@\n"), readSize, this));
    //}
    if ((recv_cnt = SuperType::peer().recv(buffer->wr_ptr(), readSize, timeout)) <= 0) {
        ACE_DEBUG((LM_DEBUG,
        ACE_TEXT("(%T)ERROR: PacketChannel:handleInputParsePacket()-  Connection closed, ret=%d timeout=%@\n"), recv_cnt, timeout));
        return -1;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel::handleInputParsePacket() - after read() %d \n"),recv_cnt));    
    buffer->wr_ptr(recv_cnt);
    if (m_debugPacketChannel) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PacketChannel::handleInputParsePacket() - after read()=%d readSize=%d this=%@ header=%@\n"), recv_cnt, readSize, this, m_header));
    }
    if (m_header == 0) {
        if (recv_cnt < readSize) {
            m_partialMessage = buffer;
            buffer = 0;
        } else {//complete
            ByteInputStream bis(buffer);
            ACE_Message_Block::release(buffer);
            buffer = 0;
            m_header = new HEADER(bis);
            if (!bis.good_bit()) {
                if (m_debugPacketChannel) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PacketChannel::handleInputParsePacket() - !bis.good_bit()\n")));
                }
                delete m_header;
                return -1;
            }
            if (m_header->getBodySize() == 0) {
                PACKET* packet = createPacket(m_header, bis); //PACKET_FACTORY::createPacket(m_header, bis);
                inputPacket = packet;
                m_header = 0;
            }
        }
        return 0;
    } else {
        if (recv_cnt < readSize) {
            m_partialMessage = buffer;
            buffer = 0;
        } else {
            //complete
            ByteInputStream bis(buffer);
            ACE_Message_Block::release(buffer);
            buffer = 0;
            PACKET* packet = createPacket(m_header, bis); //PACKET_FACTORY::createPacket(m_header, bis);
            inputPacket = packet;
            m_header = 0;
        }
        return 0;
    }
}*/

#endif	/* _PACKETCHANNEL_H */

