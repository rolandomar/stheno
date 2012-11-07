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
 * File:   RequestHandler.h
 * Author: rmartins
 *
 * Created on October 14, 2010, 8:37 PM
 */

#ifndef REQUESTHANDLER_H
#define	REQUESTHANDLER_H

#include <euryale/net/Request.h>

/*
 * For handling asynchronous requests
 */
template <class REQUEST>
class RequestHandler: public EventListener {
public:
    RequestHandler():m_request(0){
    }
    virtual ~RequestHandler(){}
    
    virtual int open(REQUEST* request){
        m_request = request;
        m_request->setListener(this);
        if(m_request->isStateFinal()){
            onEvent(m_request,m_request->getState(),m_request->getState());
        }
    }
    
    virtual void close(){
        m_request->removeListener();
    }
    
    //if oldstate == newState, it was sent in open    
    virtual void onEvent(Event* event, int oldState, int newState) {
        
    }
    
protected:
    REQUEST* m_request;

};

#endif	/* REQUESTHANDLER_H */

