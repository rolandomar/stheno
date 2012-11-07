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
 *
 * File:   StreamChannelGroup.h
 * Author: rmartins
 *
 * Created on August 31, 2010, 11:14 AM
 */

#ifndef STREAMCHANNELGROUP_H
#define	STREAMCHANNELGROUP_H

#include <euryale/net/AbstractStreamChannel.h>
#include <ace/Hash_Map_Manager.h>

#include <list>
using std::list;

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
class ChannelManagerListener {
public:
    typedef AbstractStreamChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE> ChannelType;
    typename ChannelType::channel_ptr ChannelTypePtr;

    virtual void onClose(ChannelType* channel) = 0;
    virtual void onAdd(ChannelType* channel) = 0;
};

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
class ChannelManager : public CloseListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE> {
public:
    typedef AbstractStreamChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE> ChannelType;
    typedef typename ChannelType::channel_ptr ChannelTypePtr;
    typedef ChannelManagerListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE> ListenerType;
    typedef ACE_SYNCH_RECURSIVE_MUTEX RECURSIVE_MUTEX;;
    //typedef ACE_SYNCH_DECL::RECURSIVE_MUTEX RECURSIVE_MUTEX;

    ChannelManager(ListenerType* listener = 0) :
    m_listener(listener) {
        m_debugChannelManager = RuntimeConfiguration::isClassDebugged("ChannelManager");
    }

    virtual ~ChannelManager() {
    }

    void close() {
        ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
        typename ACE_Hash_Map_Manager<UUIDPtr, ChannelTypePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();
        while (iter != m_map.end()) {
            (*iter).item()->close();
            iter++;
        }
        m_map.close();
    }

    int add(ChannelType* channel) {
        ACE_GUARD_RETURN(RECURSIVE_MUTEX, ace_mon, m_lock,-1);
        ChannelTypePtr channelPtr(channel);
        int ret = m_map.bind(channel->getChannelID(), channelPtr);
        if (ret == -1) {
            channel->close();
            delete channel;
            return -1;
        }
        //if (m_debugChannelManager) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamChannelGroup::add() ret=%d pid=%s\n"), ret, channel->getPID()->toString().c_str()));
        //}        
        if (channel->setCloseListener(this) == -1) {
            remove(channel);
            //channel->close();
            //delete channel;
            return -1;
        }
        fireOnAdd(channel);
        return ret;
    }

    void remove(ChannelType* channel) {
        ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
        //if (m_debugChannelManager) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamChannelGroup::remove() - calling close()\n")));
        //}

        ChannelTypePtr channelPtr;
        //if (m_debugChannelManager) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamChannelGroup::remove() - removing from map\n")));
        //}
        int ret = m_map.unbind(channel->getChannelID(), channelPtr);
        //if (m_debugChannelManager) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamChannelGroup::remove() - removedfrom map ret =%d\n"), ret));
        //}
        if (ret == 0) {
            channel->setCloseListener(0);
            channel->close();
            fireOnClose(channel);
        }
    }

    void remove(UUIDPtr& uuid) {
        ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
        if (m_debugChannelManager) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamChannelGroup::remove() - calling close()\n")));
        }
        ChannelTypePtr channelPtr;
        if (m_map.unbind(uuid, channelPtr) == 0) {
            channelPtr->setCloseListener(0);
            channelPtr->close();
            fireOnClose(channelPtr.get());
        }
    }

    virtual void onClose(ChannelType* channel) {
        remove(channel);
    }

    virtual int getChannelByID(UUIDPtr& channelID, ChannelTypePtr& channel) {
        ACE_GUARD_RETURN(RECURSIVE_MUTEX, ace_mon, m_lock, 0);
        return m_map.find(channelID, channel);
    }

    ChannelManagerListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* deattach() {
        ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
        ChannelManagerListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* temp = m_listener;
        m_listener = 0;
        return temp;
    }

    void setListener(ChannelManagerListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* listener) {
        ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
        m_listener = listener;
    }

    UInt getSize() {
        //return m_channelList.size();
        return m_map.total_size();
    }

    void getPeerList(list<UUIDPtr>& l) {
        ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
        typename ACE_Hash_Map_Manager<UUIDPtr, ChannelTypePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();
        while (iter != m_map.end()) {
            l.push_back((*iter).item()->getPID());
            iter++;
        }
    }

    void getMemberList(list<ChannelTypePtr>& l) {
        ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
        typename ACE_Hash_Map_Manager<UUIDPtr, ChannelTypePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();
        while (iter != m_map.end()) {
            l.push_back(iter->item());
            iter++;
        }
    }

protected:
    bool m_debugChannelManager;
    RECURSIVE_MUTEX m_lock;
    ACE_Hash_Map_Manager<UUIDPtr, ChannelTypePtr, ACE_SYNCH_RW_MUTEX> m_map;
    ChannelManagerListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* m_listener;

    bool hasValidListener() {
        return (m_listener != 0);
    }

    virtual void fireOnClose(AbstractStreamChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* channel) {
        if (hasValidListener()) {
            m_listener->onClose(channel);
        }
    }

    virtual void fireOnAdd(AbstractStreamChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* channel) {
        if (hasValidListener()) {
            m_listener->onAdd(channel);
        }
    }
};

#endif	/* STREAMCHANNELGROUP_H */

