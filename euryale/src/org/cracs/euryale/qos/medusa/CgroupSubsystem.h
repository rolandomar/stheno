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
 * File:   CgroupSubsystem.h
 * Author: rmartins
 *
 * Created on November 28, 2009, 6:10 PM
 */

#ifndef _CGROUPSUBSYSTEM_H
#define	_CGROUPSUBSYSTEM_H

#include <iostream>
#include <sstream>
using std::stringstream;


#include "ace/Dirent.h"
#include "ace/Dirent_Selector.h"
#include "ace/OS_NS_sys_stat.h"
#include "ace/OS_NS_unistd.h"
//#include "ace/OS_String.h"
#include "ace/SString.h"
#include "ace/streams.h"
#include "ace/Containers.h"
#include "ace/Malloc_T.h"

#include <euryale/Types.h>
#include <euryale/qos/exception/CgroupException.h>
#include <euryale/common/HierarchicalPath.h>
#include <euryale/common/Integer.h>


#include <list>
using std::list;
class Cgroup;

class CgroupSubsystem {
public:
    static const int RECURSION_INDENT = 3;
    static const bool SSH_IS_TOP_LEVEL = true;

    CgroupSubsystem(Cgroup* cg);
    virtual ~CgroupSubsystem();

    virtual void init(bool onlyReset) = 0;

    virtual void close() {
    }
    virtual void getCgroupDirectory(String& path) = 0;

    /// specific subsystem api

    inline void setTask(const ACE_TCHAR* dir_path, UInt taskID) {
        String path;
        this->getCgroupDirectory(path);
        //path += dir_path;
        HierarchicalPath::appendPaths(path, dir_path);
        //path += "/tasks";
        HierarchicalPath::appendPaths(path, "/tasks");
        //stringstream taskNum(stringstream::in | stringstream::out);
	stringstream taskNum(stringstream::out);
        taskNum << taskID;
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: setTask() writeFile=%s tid=%d\n"), dir_path, taskID));        
        writeFile(path.c_str(), taskNum.str().c_str());
        //double write?
        //writeFile(path.c_str(),taskNum.str().c_str());
    }


    static bool dirExists(const ACE_TCHAR* dir_path);
    static bool fileExists(const ACE_TCHAR* file_path);

    static void writeFile(const ACE_TCHAR* dir_path, const ACE_TCHAR* data) throw (CgroupException&);
    static void writeFile(const ACE_TCHAR* dir_path, String& data) throw (CgroupException&);
    static String* readFile(const ACE_TCHAR* dir_path) throw (CgroupException&);


    static void migrateTasks(const ACE_TCHAR* root_path,
            ACE_DLList<String>& tasks, bool excludeSystemTasks = false) throw (CgroupException&);

    static void migrateTask(ACE_hthread_t task,
            const ACE_TCHAR* sourcePath,
            const ACE_TCHAR* destPath) throw (CgroupException&);

    static void readTasks(const ACE_TCHAR* taskFile, ACE_DLList<String>& tasks) throw (CgroupException&);
    static void readTasks(const ACE_TCHAR* taskFile, list<UInt>& tasks) throw (CgroupException&);

    void removeTree(const ACE_TCHAR* dir_path) throw (CgroupException&);

    void readChildrenDirectories(const ACE_TCHAR* dir_path, ACE_DLList<String>& dirs) throw (CgroupException&);

    void reset(const ACE_TCHAR* dir_path) throw (CgroupException&);

    void reset_i(//const ACE_TCHAR* root_path,
            const ACE_TCHAR* dir_path,
            ACE_DLList<String>& tasks) throw (CgroupException&);

    void removeSubDomain(bool removeRoot, const ACE_TCHAR* dir_path) throw (CgroupException&);
protected:
    Cgroup* m_cg;

    static int getExcludedTasks(ACE_DLList<String>& tasks) {
        const char* command = "ps ax";
        FILE *pf;
        char data[100000];
        int num;
        char c2[10000];
        char c3[10000];
        char c4[10000];
        char c5[20000];

        // Setup our pipe for reading and execute our command.
        pf = popen(command, "r");

        if (!pf) {
            fprintf(stderr, "Could not open pipe for output.\n");
            return -1;
        }

        // Grab data from process execution
        //read first line
        fgets(data, 100000, pf);
        while (feof(pf) == 0) {
            fgets(data, 100000, pf);
            sscanf(data, "%d %s %s %s %s", &num, c2, c3, c4, c5);
            //fprintf(stdout, "(%d,%s)\n", num, c5);
            //String prog(c5);
            //std::string prog(c5);

            std::string prog(c5);
            bool flag = false;

            if (c5[0] == '[') {
                flag = true;
            }

            if (SSH_IS_TOP_LEVEL) {
                if (prog.find("sshd", 0) != std::string::npos) {
                    flag = true;
                }
                if (prog.find("bash", 0) != std::string::npos) {
                    flag = true;
                }
            }

            if (prog.find("upstart-udev-bridge", 0) != std::string::npos) {
                flag = true;
            }
            if (prog.find("/sbin/init", 0) != std::string::npos) {
                flag = true;
            }

            if (prog.find("medusa", 0) != std::string::npos) {
                flag = true;
            }
            if (prog.find("/usr/sbin/irqbalance", 0) != std::string::npos) {
                flag = true;
            }
            if (prog.find("rsyslogd", 0) != std::string::npos) {
                flag = true;
            }

            if (prog.find("udevd", 0) != std::string::npos) {
                flag = true;
            }
            if (prog.find("acpid", 0) != std::string::npos) {
                flag = true;
            }

            if (flag) {
                //fprintf(stdout, "Exclude (%d,%s)\n", num, c5);
                String* entry;
                ACE_DLList_Iterator<String> iter(tasks);
                while (!iter.done()) {
                    entry = iter.next();
                    UInt taskNo = Integer::parseInt(*(entry), 10);
                    if (taskNo == num) {
                        //iter = tasks.erase(iter);
                        iter.advance_and_remove(false);
                        delete entry;
                    } else {
                        iter++;
                    }

                }

            }
        }

        // Print grabbed data to the screen.
        //fprintf(stdout, "-%s-\n", data);
        if (pclose(pf) != 0) {
            fprintf(stderr, " Error: Failed to close command stream \n");
            return -1;
        }

        return 0;
    }














private:
    CgroupSubsystem(const CgroupSubsystem& orig);
};

#endif	/* _CGROUPSUBSYSTEM_H */

