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
 * File:   P3Config.h
 * Author: rmartins
 *
 * Created on July 29, 2010, 12:09 PM
 */

#ifndef P3CONFIG_H
#define	P3CONFIG_H

#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>

#include <euryale/Types.h>
#include <libconfig.h++>
using libconfig::Config;


class P3Config{
public:
    P3Config();
    virtual ~P3Config();

    bool lookupValue(const char *path, Boolean &value) const throw ();
    bool lookupValue(const char *path, Short &value) const throw ();
    bool lookupValue(const char *path, UShort &value) const throw ();
    bool lookupValue(const char *path, Int &value) const throw ();
    bool lookupValue(const char *path, Int &value,Int defaultValue);
    bool lookupValue(const char *path, UInt &value) const throw ();
    //bool lookupValue(const char *path, LongLong &value) const throw ();
    //bool lookupValue(const char *path, ULongLong &value) const throw ();
    bool lookupValue(const char *path, Double &value) const throw ();
    bool lookupValue(const char *path, Float &value) const throw ();
    bool lookupValue(const char *path, const char *&value) const throw ();
    bool lookupValue(const char *path, String &value) const throw ();

    Config& getConfig();
private:
    Config m_config;
};

typedef ACE_Singleton<P3Config, ACE_Thread_Mutex>  P3Configuration;

#endif	/* P3CONFIG_H */

