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
 * File:   HierarchicalPath.cpp
 * Author: rmartins
 * 
 * Created on February 25, 2010, 5:21 PM
 */

#include "HierarchicalPath.h"
const char* MalformedException::MalformedExceptionMessage = "Path malformed";

HierarchicalPath::HierarchicalPath() {
}

HierarchicalPath::HierarchicalPath(ACE_CString& path) throw (Exception*) {
    m_path = path;
    parsePath();
    int size = m_partials.size();
    if (size > 2) {
        for (int i = 0; i < size - 2; i++) {
            m_parent += m_partials[i];
        }
    }

}

void HierarchicalPath::parsePath() throw (Exception*) {
    int pos = 0;
    int size = m_path.length();
    if (size <= 1) {
        return;
    }

    if (m_path.fast_rep()[0] != '/') {
        throw new MalformedException();
    }
    if (m_path.fast_rep()[size - 1] != '/') {
        m_path = m_path + "/";
    }

    pos = 1;
    while (pos != ACE_CString::npos) {
        int newpos = m_path.find("/", pos);
        if (newpos != ACE_CString::npos) {
            ACE_CString partial = m_path.substr(pos, (newpos - pos));
            m_partials.push_back(partial);
        } else {
            break;
        }
        pos = newpos + 1;
        if (pos >= m_path.length()) {
            break;
        }
    }
}

HierarchicalPath::HierarchicalPath(const HierarchicalPath& orig) throw (Exception*) {
    this->m_path = orig.m_path;
    this->m_parent = orig.m_parent;
    parsePath();

}

HierarchicalPath::~HierarchicalPath() {
}







FlatHierarchicalPath::FlatHierarchicalPath() {
}

FlatHierarchicalPath::FlatHierarchicalPath(ACE_CString& path) throw (Exception*) {
    m_path = path;
    parsePath();
    int size = m_partials.size();
    if (size > 2) {
        for (int i = 0; i < size - 2; i++) {
            m_parent += m_partials[i];
        }
    }

}

void FlatHierarchicalPath::parsePath() throw (Exception*) {
    int pos = 0;
    int size = m_path.length();
    if (size <= 1) {
        return;
    }

    /*if (m_path.fast_rep()[0] != '/') {
        throw new MalformedException();
    }*/
    if (m_path.fast_rep()[size - 1] != '-') {
        m_path = m_path + "-";
    }

    pos = 1;
    while (pos != ACE_CString::npos) {
        int newpos = m_path.find("-", pos);
        if (newpos != ACE_CString::npos) {
            ACE_CString partial = m_path.substr(pos, (newpos - pos));
            m_partials.push_back(partial);
        } else {
            break;
        }
        pos = newpos + 1;
        if (pos >= m_path.length()) {
            break;
        }
    }
}

FlatHierarchicalPath::FlatHierarchicalPath(const FlatHierarchicalPath& orig) throw (Exception*) {
    this->m_path = orig.m_path;
    this->m_parent = orig.m_parent;
    parsePath();

}

FlatHierarchicalPath::~FlatHierarchicalPath() {
}

