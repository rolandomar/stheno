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
 * File:   ReplicationMessage.h
 * Author: rmartins
 *
 * Created on August 22, 2010, 10:56 PM
 */

#ifndef REPLICATIONMESSAGE_H
#define	REPLICATIONMESSAGE_H

class ReplicationMessage {
public:
    ReplicationMessage();
    ReplicationMessage(const ReplicationMessage& orig);
    virtual ~ReplicationMessage();
private:

};

#endif	/* REPLICATIONMESSAGE_H */

