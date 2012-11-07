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
 * File:   Cgroup.h
 * Author: rmartins
 *
 * Created on November 28, 2009, 5:11 PM
 */

#ifndef _CGROUP_H
#define	_CGROUP_H


#include <euryale/qos/exception/CgroupException.h>


class CPUCgroup;
class CPUSchedCgroup;

class Cgroup {
public:
    static const bool CPUSET_AND_SCHED_MERGED = true;//true;
    static const bool DISABLE_CPU_CGROUP = false;
    static const bool FLAT_HIERARCHY = true;
    static const bool PARTITIONING = false;//false;

    
    static const char* getHRTBase(){
        if(PARTITIONING){
            return "/HRT";
        }else{
            return "";
        }
    }
    
    static const char* getSRTBase(){
        if(PARTITIONING){
            return "/SRT";
        }else{
            return "";
        }
    }
    
    static const char* getBEBase(){
        if(PARTITIONING){
            return "/BE";
        }else{
            return "";
        }
    }
    
    static const char* getOSBase(){
        if(PARTITIONING){
            return "/OS";
        }else{
            return "";
        }
    }
    
    static char getSep(){
        if(FLAT_HIERARCHY){
            return '-';
        }else{
            return '/';
        }
    }
            
    Cgroup();
    Cgroup(const Cgroup& orig);
    virtual ~Cgroup();
    
    void init(bool onlyReset) throw(CgroupException&);
    void close();

    void remove() throw (CgroupException&);

    const char* getCgroupRootDirectory(){
        return "/cgroup";
    }

    CPUCgroup* getCPUSetGroup(){
        return m_cgCPU;
    }

    CPUSchedCgroup* getCPUSchedGroup(){
        return m_cgCPUSched;
    }   

private:
    CPUCgroup* m_cgCPU;
    CPUSchedCgroup* m_cgCPUSched;
    bool m_inited;

};

#endif	/* _CGROUP_H */

