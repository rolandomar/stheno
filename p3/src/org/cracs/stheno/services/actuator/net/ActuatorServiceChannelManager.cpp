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
 * File:   ActuatorServiceChannelManager.cpp
 * Author: rmartins
 * 
 * Created on November 24, 2010, 2:55 PM
 */

#include "ActuatorServiceChannelManager.h"
#include "stheno/core/p2p/p3/leaf/actuator/net/ActuatorClientHandler.h"
#include <euryale/net/RequestGroup.h>

ActuatorServiceChannelManager::ActuatorServiceChannelManager(ListenerType* listener) :
ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>(listener) {
}

ActuatorServiceChannelManager::~ActuatorServiceChannelManager() {
}

int ActuatorServiceChannelManager::sendAction(ActionPacket* ap) {
    ACE_GUARD_RETURN(RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    RequestGroup<ActuatorClientHandler, ActuatorClientHandler::RequestPtr*> requestGroup(0);
    list<ChannelTypePtr> memberList;
    getMemberList(memberList);
    ace_mon.release();
    //ACE_Time_Value start_1 = ACE_OS::gettimeofday();

    list<ActuatorClientHandler::RequestPtr*> lr;
    list<ChannelTypePtr>::iterator iter = memberList.begin();
    while (iter != memberList.end()) {
        //ACE_Time_Value start = ACE_OS::gettimeofday();
        /*ActuatorClientHandler* clientHandler = static_cast<ActuatorClientHandler*> ((*iter).get());
        ActuatorActionPacket* aap = new ActuatorActionPacket(0, ap->getTimestamp(), ap->getPayload()->duplicate());
        //payload is freed upon packet deletion
        ActuatorClientHandler::RequestPtr* request = clientHandler->sendRequest(aap, 0);*/

        //ACE_Time_Value start = ACE_OS::gettimeofday();
        ActuatorClientHandler* clientHandler = static_cast<ActuatorClientHandler*> ((*iter).get());
        ActuatorActionPacket* aap = new ActuatorActionPacket(0, ap->getTimestamp(), ap->getPayload()->duplicate());
        ActuatorClientHandler::RequestPtr* request = clientHandler->sendRequest(aap, 0);
        //ACE_Time_Value end = ACE_OS::gettimeofday();
        //end -= start;
        //ULongLong deltausec;
        //end.to_usec(deltausec);
        //ACE_DEBUG((LM_DEBUG,
        //        ACE_TEXT("(%t|%T)INFO: ActuatorServiceChannelManager per sendRequest lat(%Q)usec \n"),
        //        deltausec
        //        ));
        if (request != 0) {
            lr.push_back(request);
        }

        /*ACE_Time_Value end = ACE_OS::gettimeofday();
        end -= start;
        ULongLong deltausec;
        end.to_usec(deltausec);
        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%t|%T)INFO: ActuatorServiceChannelManager per sendRequest lat(%Q)usec \n"),
                deltausec
                ));*/
        delete aap;
        /*if (request != 0) {
            requestGroup.addRequest(request);
        } else {
            printf("failed request\n");
        }*/
        iter++;
    }
    /*ACE_Time_Value end = ACE_OS::gettimeofday();
    end -= start_1;
    ULongLong deltausec;
    end.to_usec(deltausec);
    ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%t|%T)INFO: ActuatorServiceChannelManager total send lat(%Q)usec \n"),
            deltausec
            ));*/

    /*requestGroup.open();
    list<ActuatorClientHandler::RequestPtr*>* l = requestGroup.waitFuture(0);
    if (l != 0) {
        ListHelper<ActuatorClientHandler::RequestPtr*>::deleteList(l);
        return 0;
    }*/

    list<ActuatorClientHandler::RequestPtr*>::iterator iter2 = lr.begin();
    while (iter2 != lr.end()) {
        //ACE_Time_Value start = ACE_OS::gettimeofday();
        (*(*iter2))->waitFuture(0);
        //ACE_Time_Value end = ACE_OS::gettimeofday();
        //end -= start;
        //ULongLong deltausec;
        //end.to_usec(deltausec);
        //ACE_DEBUG((LM_DEBUG,
         //       ACE_TEXT("(%t|%T)INFO: ActuatorServiceChannelManager wait request lat(%Q)usec \n"),
         //       deltausec
         //       ));
        delete *iter2;
        iter2++;
    }



    /*else
        list<SthenoStreamPacket*> *channelResults = (*l->front())->detach();
    printf("ActionTime=")*/

    //requestGroup.close();
    return 0;

}
