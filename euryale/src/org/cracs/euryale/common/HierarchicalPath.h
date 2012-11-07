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
 * File:   HierarchicalPath.h
 * Author: rmartins
 *
 * Created on February 25, 2010, 5:21 PM
 */

#ifndef _HIERARCHICALPATH_H
#define	_HIERARCHICALPATH_H

#include <euryale/Types.h>
#include <euryale/common/exception/Exception.h>

#include <list>
using std::list;
#include <vector>
using std::vector;

class MalformedException : public Exception {
public:
	MalformedException(){
        m_toString = 0;
        m_messageString = 0;
    }
	virtual ~MalformedException(){
        if (m_messageString != 0) {
            delete m_messageString;
        }
        if(m_toString!=0){
            delete m_toString;
        }
    }

	virtual void raise(void) {
		throw *this;
	}

	virtual const String& message() {
		if (m_messageString == 0) {
			m_messageString = new String(MalformedExceptionMessage);
		}
		return *m_messageString;
	}


	/*int
	 * Object inheritance
	 * */
	virtual const char* objectID() const {
		return "f129a7830fbff657573b1687d34b5be0";
	}

	virtual const String& toString() {
		if (m_toString == 0)
			m_toString = new String("Mal");
		return *m_toString;
	}

private:
	static const char* MalformedExceptionMessage;
	String* m_messageString;
};


class HierarchicalPath {
public:
    HierarchicalPath();
    HierarchicalPath(ACE_CString& path) throw(Exception*);
    HierarchicalPath(const HierarchicalPath& orig) throw(Exception*);
    virtual ~HierarchicalPath();

    
    static void appendPaths(String& path1,const char* path2){
        String path2Str(path2);
        appendPaths(path1,path2Str);
    }
    
    static void appendPaths(String& path1,String path2){
        if(path1.fast_rep()[path1.length()-1] == '/'){
            if(path2.fast_rep()[0] == '/'){
                path1 = path1.substr(0,path1.length()-1);
            }
        }else{
            if(path2.fast_rep()[0] != '/'){
                path1 += '/';
            }
        }
        path1 += path2;        
    }
    
    
    vector<ACE_CString>& getHierarchyNodes(){
        return m_partials;
    }
    ACE_CString& getCanonicalPath(){
        return m_path;
    }
    ACE_CString& getName(){
        return m_partials.front();
    }
    
    ACE_CString& getParentName(){
        return m_parent;
    }

    void setPath(ACE_CString& path) throw (Exception*){
        m_path = path;
        m_partials.clear();
        parsePath();
    }

protected:
    ACE_CString m_path;
    ACE_CString m_parent;
    vector<ACE_CString> m_partials;

    void parsePath() throw (Exception*);


};

class FlatHierarchicalPath {
public:
    FlatHierarchicalPath();
    FlatHierarchicalPath(ACE_CString& path) throw(Exception*);
    FlatHierarchicalPath(const FlatHierarchicalPath& orig) throw(Exception*);
    virtual ~FlatHierarchicalPath();

    
    static void appendPaths(String& path1,const char* path2){
        String path2Str(path2);
        appendPaths(path1,path2Str);
    }
    
    static void appendPaths(String& path1,String path2){
        if(path1.fast_rep()[path1.length()-1] == '-'){
            if(path2.fast_rep()[0] == '-'){
                path1 = path1.substr(0,path1.length()-1);
            }
        }else{
            if(path2.fast_rep()[0] != '-'){
                path1 += '-';
            }
        }
        path1 += path2;        
    }
    
    
    vector<ACE_CString>& getHierarchyNodes(){
        return m_partials;
    }
    ACE_CString& getCanonicalPath(){
        return m_path;
    }
    ACE_CString& getName(){
        return m_partials.front();
    }
    
    ACE_CString& getParentName(){
        return m_parent;
    }

    void setPath(ACE_CString& path) throw (Exception*){
        m_path = path;
        m_partials.clear();
        parsePath();
    }

protected:
    ACE_CString m_path;
    ACE_CString m_parent;
    vector<ACE_CString> m_partials;

    void parsePath() throw (Exception*);


};

#endif	/* _HIERARCHICALPATH_H */

