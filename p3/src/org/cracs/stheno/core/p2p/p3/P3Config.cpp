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
 * File:   P3Config.cpp
 * Author: rmartins
 * 
 * Created on July 29, 2010, 12:09 PM
 */

#include "P3Config.h"

P3Config::P3Config() {
    m_config.readFile("p3.properties");
    bool r;
    m_config.lookupValue("FT", r);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Config: FT=%d\n"), r));
}

P3Config::~P3Config() {
}

bool P3Config::lookupValue(const char *path, Boolean &value) const throw () {
    return m_config.lookupValue(path, value);
}

/*bool P3Config::lookupValue(const char *path, Long &value) const throw () {
    return m_config.lookupValue(path,value);
}

bool P3Config::lookupValue(const char *path, ULong &value) const throw () {
    return m_config.lookupValue(path,value);
}*/

bool P3Config::lookupValue(const char *path, Short &value) const throw () {
    Int value_ = 0;
    bool ret = lookupValue(path, value_);
    if (!ret || value_ > (1 << 16)) {
        return false;
    }
    value = value_;
    return true;
}

bool P3Config::lookupValue(const char *path, UShort &value) const throw () {
    UInt value_ = 0;
    bool ret = lookupValue(path, value_);
    if (!ret || value_ > (1 << 16)) {
        return false;
    }
    value = value_;
    return true;
}

bool P3Config::lookupValue(const char *path, Int &value) const throw () {
    return m_config.lookupValue(path, value);
}

bool P3Config::lookupValue(const char *path, Int &value, Int defaultValue) {
    try {
        if (!lookupValue(path, value)) {
            value = defaultValue;
            return false;
        }
        return true;
    } catch (...) {
        value = defaultValue;
        return false;
    }
}

bool P3Config::lookupValue(const char *path, UInt &value) const throw () {
    return m_config.lookupValue(path, value);
}

/*bool P3Config::lookupValue(const char *path, LongLong &value) const throw () {
    return m_config.lookupValue(path, value);
}

bool P3Config::lookupValue(const char *path, ULongLong &value) const throw () {
    return m_config.lookupValue(path, value);
    }*/

bool P3Config::lookupValue(const char *path, Double &value) const throw () {
    return m_config.lookupValue(path, value);
}

bool P3Config::lookupValue(const char *path, Float &value) const throw () {
    return m_config.lookupValue(path, value);
}

bool P3Config::lookupValue(const char *path, const char *&value) const throw () {
    return m_config.lookupValue(path, value);
}

bool P3Config::lookupValue(const char *path, String &value) const throw () {
    std::string str;
    if (m_config.lookupValue(path, str)) {
        value = str.c_str();
        return true;
    }
    return false;
}

Config& P3Config::getConfig() {
    return m_config;
}
