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
 * File:   CPUQoS.h
 * Author: rmartins
 *
 * Created on November 3, 2009, 4:49 PM
 */

#ifndef _CPUQOS_H
#define	_CPUQOS_H

#include <euryale/serialization/Serializable.h>
#include <euryale/Types.h>
#include <ace/OS_NS_Thread.h>

class CPUQoS : public Serializable {
public:

    enum {
        SCHEDULE_FIFO = ACE_SCHED_FIFO,
        SCHEDULE_RR = ACE_SCHED_RR,
        SCHEDULE_OTHER = ACE_SCHED_OTHER,
        SCHEDULE_DEADLINE = 6, //as in SCHED_DEADLINE
        SCHEDULE_RT_DEFAULT = ACE_SCHED_FIFO
    };
    
    
    
    static const int BE_PRIO = 0;
    static const int LOW_RT_PRIO = 1;
    static const int MED_RT_PRIO = 24;
    static const int MAX_RT_PRIO = 48;//99;

    CPUQoS();
    CPUQoS(int m_policy);
    CPUQoS(const CPUQoS& orig);
    virtual ~CPUQoS();

    virtual CPUQoS* duplicate() {
        return new CPUQoS(*this);
    }

    virtual void serialize(OutputStream& outputStream)
    throw (SerializationException&) {
        outputStream.write_long(m_policy);
    }

    virtual void deserialize(InputStream& inputStream)
    throw (SerializationException&) {
        inputStream.read_long(m_policy);
    }

    int getPolicy() {
        return m_policy;
    }

protected:
    int m_policy;
};

class CPUPriorityQoS : public CPUQoS {
public:

    CPUPriorityQoS() {
    }

    CPUPriorityQoS(int type, UShort priority) :
    CPUQoS(type), m_priority(priority) {
    }

    CPUPriorityQoS(const CPUPriorityQoS& orig) :
    CPUQoS(orig) {
        m_priority = orig.m_priority;
    }

    virtual CPUQoS* duplicate() {
        return new CPUPriorityQoS(*this);
    }

    virtual ~CPUPriorityQoS() {
    }

    int getPriority() {
        return m_priority;
    }

    virtual void serialize(OutputStream& outputStream)
    throw (SerializationException&) {
        CPUQoS::serialize(outputStream);
        outputStream.write_long(m_priority);
    }

    virtual void deserialize(InputStream& inputStream)
    throw (SerializationException&) {
        CPUQoS::deserialize(inputStream);
        inputStream.read_long(m_priority);
    }

    virtual void deserialize(CPUQoS& cpuQoS, InputStream& inputStream)
    throw (SerializationException&) {
        //CPUQoS::deserialize(inputStream);
        this->m_policy = cpuQoS.getPolicy();
        inputStream.read_long(m_priority);
    }

protected:
    int m_priority;
};

class CPUPriorityRTFifo : public CPUPriorityQoS {
public:

    CPUPriorityRTFifo() {
    }

    CPUPriorityRTFifo(int priority) :
    CPUPriorityQoS(SCHEDULE_FIFO, priority) {
    }

    CPUPriorityRTFifo(CPUPriorityRTFifo& orig) :
    CPUPriorityQoS(orig) {
    }

    virtual ~CPUPriorityRTFifo() {
    }

    virtual CPUQoS* duplicate() {
        return new CPUPriorityRTFifo(*this);
    }
};

class CPUPriorityRTRoundRobin : public CPUPriorityQoS {
public:

    CPUPriorityRTRoundRobin() {
    }

    CPUPriorityRTRoundRobin(int priority) :
    CPUPriorityQoS(SCHEDULE_RR, priority) {
    }

    CPUPriorityRTRoundRobin(CPUPriorityRTFifo& orig) :
    CPUPriorityQoS(orig) {
    }

    virtual ~CPUPriorityRTRoundRobin() {
    }
};

class CPUPriorityOther : public CPUPriorityQoS {
public:

    CPUPriorityOther() {
    }

    CPUPriorityOther(int priority) :
    CPUPriorityQoS(SCHEDULE_OTHER, priority) {
    }

    CPUPriorityOther(CPUPriorityOther& orig) :
    CPUPriorityQoS(orig) {
    }

    virtual ~CPUPriorityOther() {
    }

    virtual CPUQoS* duplicate() {
        return new CPUPriorityOther(*this);
    }
};

class CPUDeadlineQoS : public CPUQoS {
public:

    CPUDeadlineQoS() {
    }

    CPUDeadlineQoS(int runtime, int period) :
    CPUQoS(SCHEDULE_DEADLINE), m_period(period), m_runtime(runtime) {
    }

    CPUDeadlineQoS(const CPUDeadlineQoS& orig) :
    CPUQoS(orig) {
        m_runtime = orig.m_runtime;
        m_period = orig.m_period;
    }

    virtual ~CPUDeadlineQoS() {
    }

    virtual CPUQoS* duplicate() {
        return new CPUDeadlineQoS(*this);
    }

    int getPeriod() {
        return m_period;
    }

    int getRuntime() {
        return m_runtime;
    }

    virtual void serialize(OutputStream& outputStream)
    throw (SerializationException&) {
        CPUQoS::serialize(outputStream);
        outputStream.write_long(m_runtime);
        outputStream.write_long(m_period);
    }

    virtual void deserialize(InputStream& inputStream)
    throw (SerializationException&) {
        CPUQoS::deserialize(inputStream);
        inputStream.read_long(m_runtime);
        inputStream.read_long(m_period);
    }

    virtual void deserialize(CPUQoS& cpuQoS, InputStream& inputStream)
    throw (SerializationException&) {
        //CPUQoS::deserialize(inputStream);
        this->m_policy = cpuQoS.getPolicy();
        inputStream.read_long(m_runtime);
        inputStream.read_long(m_period);
    }

protected:
    int m_period;
    int m_runtime;
};

class CPUQoSFactory {
public:

    static CPUQoS* createCPUQoS(InputStream& inputStream) throw (SerializationException&) {
        CPUQoS qos;
        qos.deserialize(inputStream);
        switch (qos.getPolicy()) {
            case CPUQoS::SCHEDULE_DEADLINE:
            {
                CPUDeadlineQoS* newItem = new CPUDeadlineQoS();
                newItem->deserialize(qos, inputStream);
                return newItem;
            }
            case CPUQoS::SCHEDULE_FIFO:
            {
                CPUPriorityRTFifo* newItem = new CPUPriorityRTFifo();
                newItem->deserialize(qos, inputStream);
                return newItem;
            }
            case CPUQoS::SCHEDULE_RR:
            {
                CPUPriorityRTRoundRobin* newItem = new CPUPriorityRTRoundRobin();
                newItem->deserialize(qos, inputStream);
                return newItem;
            }
            case CPUQoS::SCHEDULE_OTHER:
            {
                CPUPriorityOther* newItem = new CPUPriorityOther();
                newItem->deserialize(qos, inputStream);
                return newItem;
            }
            default:
                throw new SerializationException(SerializationException::BAD_STREAM);
        }
    }

};
#endif	/* _CPUQOS_H */

