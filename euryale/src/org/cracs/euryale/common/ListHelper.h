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
 * File:   ListHelper.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 4, 2010, 3:00 PM
 */

#ifndef LISTHELPER_H
#define	LISTHELPER_H

#include <list>
using std::list;

template <class RESULT>
class ListHelper {
public:
    ListHelper();    
    virtual ~ListHelper();

    static void deleteList(list<RESULT>*& l){
        if(l == 0){ 
            printf("ListHelper::deleteList() - list is null!\n");
            return;
        }
        typename list<RESULT>::iterator iter = l->begin();
        while(iter != l->end()){
            delete *iter;
            iter++;
        }
        delete l;
    }
    static void deleteRequestList(list<RESULT>*& l){
        if(l == 0){ 
            return;
        }
        typename list<RESULT>::iterator iter = l->begin();
        while(iter != l->end()){
            //(*iter)->close();
            delete *iter;
            iter++;
        }
        delete l;
    }

    static void clearList(list<RESULT>& l){
        typename list<RESULT>::iterator iter = l.begin();
        while(iter != l.end()){
            delete *iter;
            iter++;
        }
        l.clear();
    }

    static void printRefList(list<RESULT>& l){
        typename list<RESULT>::iterator iter = l.begin();
        while(iter != l.end()){
            String temp = (*iter).toString();            
            iter++;
        }
    }

    static void printSmartPtrList(list<RESULT>& l){
        typename list<RESULT>::iterator iter = l.begin();
        while(iter != l.end()){
            String temp = (*iter)->toString();            
            iter++;
        }
    }

    static void printPointerList(list<RESULT>& l){
        typename list<RESULT>::iterator iter = l.begin();        
        while(iter != l.end()){
            String temp = (*iter)->toString();            
            iter++;
        }
    }

};

#endif	/* LISTHELPER_H */

