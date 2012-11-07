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
 * File:   CgroupSubsystem.cpp
 * Author: rmartins
 * 
 * Created on November 28, 2009, 6:10 PM
 */
#include "CgroupSubsystem.h"


#include <euryale/qos/medusa/Cgroup.h>
#include <list>
using std::list;

CgroupSubsystem::CgroupSubsystem(Cgroup* cg) : m_cg(cg) {
}

CgroupSubsystem::CgroupSubsystem(const CgroupSubsystem& orig) {
}

CgroupSubsystem::~CgroupSubsystem() {
}

bool CgroupSubsystem::dirExists(const ACE_TCHAR* dir_path) {
    ACE_Dirent dir;
    if (dir.open(dir_path) == -1) {
        return false;
    }
    return true;

}

bool CgroupSubsystem::fileExists(const ACE_TCHAR* file_path) {
    ifstream infile(file_path, ios::in);
    if (infile.bad()) {
        return false;
    }
    return true;
}

void CgroupSubsystem::reset(const ACE_TCHAR* dir_path) throw (CgroupException&) {
    ACE_DLList<String> tasks;
    //migrateTask(1122,"/cgroup/cpu/tasks","");
    /// gather info from 'tasks' files
    //reset_i(dir_path, dir_path, tasks);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::reset(%s)\n"), dir_path));
    reset_i(dir_path, tasks);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::reset(%s) after reset_i\n"), dir_path));
    /// migrate tasks to root container
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::reset(%s) before migrate\n"), dir_path));
    migrateTasks(dir_path, tasks);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::reset(%s) after migrate\n"), dir_path));
    /// remove sub-tree
    ACE_DEBUG((LM_DEBUG, "about to remove directory %s\n", dir_path));
    this->removeTree(dir_path);
    ACE_DEBUG((LM_DEBUG, "about to removed directory %s\n", dir_path));
}

void CgroupSubsystem::removeSubDomain(bool removeRoot, const ACE_TCHAR* dir_path) throw (CgroupException&) {
    ACE_DLList<String> tasks;
    //migrateTask(1122,"/cgroup/cpu/tasks","");
    /// gather info from 'tasks' files
    //reset_i(dir_path, dir_path, tasks);
    reset_i(dir_path, tasks);
    /// migrate tasks to root container
    String path;
    this->getCgroupDirectory(path);
    migrateTasks(path.c_str(), tasks);
    /// remove sub-tree
    //ACE_DEBUG((LM_DEBUG, "about to remove directory %s\n", dir_path));
    this->removeTree(dir_path);
    if (removeRoot) {
        printf("Removing top root = %s\n", dir_path);
        ACE_OS::rmdir(dir_path);
    }
}

void CgroupSubsystem::writeFile(const ACE_TCHAR* file_path, const ACE_TCHAR* data)
throw (CgroupException&) {
    String dataStr(data);
    cout << "writeFile:" << file_path << "<" << data << endl;
    ////ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)writeFile=%s\n"),file_path));
    ////writeFile(file_path, dataStr);
    //ofstream outfile(file_path, ios::out);
    //outfile << dataStr;// << flush;    
    //outfile.close();
    dataStr += '\0';
    FILE * pFile;
    pFile = fopen(file_path, "w");
    if (pFile != NULL) {
        fputs(dataStr.c_str(), pFile);
        fclose(pFile);
    }else{
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) failed ! writeFile=%s\n"),file_path));
    }

}

void CgroupSubsystem::writeFile(const ACE_TCHAR* file_path, String& data)
throw (CgroupException&) {
    //String command;
    //command = "echo ";
    //command += data;
    //command += " >  ";
    //command += file_path;
    ////ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO writeFile=%s\n"),command.c_str()));
    //int ret = ACE_OS::system(command.c_str());
    ///*if (ret != 0) {
    //    throw CgroupException(ACE_OS::strerror(errno));
    //}*/

    //ofstream outfile(file_path, ios::out);
    //outfile << data; // << flush;    
    //outfile.close();
    data += '\0';
    FILE * pFile;
    pFile = fopen(file_path, "w");
    if (pFile != NULL) {
        fputs(data.c_str(), pFile);
        fclose(pFile);
    }else{
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) failed ! writeFile=%s\n"),file_path));
    }
}

String* CgroupSubsystem::readFile(const ACE_TCHAR* dir_path) throw (CgroupException&) {
    ifstream infile(dir_path, ios::in);
    if (infile.bad()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::readFile() - error reading file\n")));
        throw CgroupException(ACE_OS::strerror(errno));
    }

    char line[BUFSIZ];

    if (infile.getline(line, BUFSIZ, '\n')) {
        //std::cout << line << std::endl;
        String* str = new String(line);
        return str;

    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::readFile() - error reading file 2\n")));
        throw CgroupException(ACE_OS::strerror(errno));
    }
}

void CgroupSubsystem::readChildrenDirectories(const ACE_TCHAR* dir_path,
        ACE_DLList<String>& dirs) throw (CgroupException&) {
    if (ACE_OS::chdir(dir_path) == -1)
        ACE_ERROR((LM_ERROR,
            ACE_TEXT("(%T)readChildrenDirectories - chdir: %@\n"),
            dir_path)
            );


    ACE_Dirent dir;
    if (dir.open(ACE_TEXT(".")) == -1)
        ACE_ERROR
            ((LM_ERROR, ACE_TEXT("(%T)readChildrenDirectories: open of dir . at %s failed\n"), dir_path));

    // Since the dir struct d_name type changes depending on the setting
    // of ACE_LACKS_STRUCT_DIR, copy each name into a neutral format
    // array to work on it.
    size_t const maxnamlen = MAXNAMLEN;
    ACE_TCHAR tname[maxnamlen + 1];

    int entry_count = 0;

    for (ACE_DIRENT *directory; (directory = dir.read()) != 0;) {
        // Skip the ".." and "." files.
        if (ACE::isdotdir(directory->d_name) == true)
            continue;

        ACE_OS::strncpy(tname,
                ACE_TEXT_CHAR_TO_TCHAR(directory->d_name),
                maxnamlen);

        ACE_stat stat_buf;
        if (ACE_OS::lstat(directory->d_name, &stat_buf) == -1) {
            ACE_ERROR((LM_ERROR,
                    ACE_TEXT("(%T)%@\n"),
                    tname));
            return;
        }

        switch (stat_buf.st_mode & S_IFMT) {
            case S_IFDIR:
                String* str = new String(directory->d_name);
                dirs.insert_tail(str);
                break;
        }
    }
}

void CgroupSubsystem::reset_i(/*const ACE_TCHAR* root_path,*/
        const ACE_TCHAR* dir_path,
        ACE_DLList<String>& tasks) throw (CgroupException&) {

    int recursion_level = CgroupSubsystem::RECURSION_INDENT;
    if (ACE_OS::chdir(dir_path) == -1)
        ACE_ERROR((LM_ERROR,
            ACE_TEXT("(%T)reset_i - chdir: %@\n"),
            dir_path)
            );


    ACE_Dirent dir;
    //if (dir.open(ACE_TEXT("(%T).")) == -1)
    if (dir.open(ACE_TEXT(".")) == -1)
        ACE_ERROR
            ((LM_ERROR, ACE_TEXT("(%T)reset_i open of dir . at %s failed\n"), dir_path));

    // Since the dir struct d_name type changes depending on the setting
    // of ACE_LACKS_STRUCT_DIR, copy each name into a neutral format
    // array to work on it.
    size_t const maxnamlen = MAXNAMLEN;
    ACE_TCHAR tname[maxnamlen + 1];

    int entry_count = 0;

    for (ACE_DIRENT *directory; (directory = dir.read()) != 0;) {
        // Skip the ".." and "." files.
        if (ACE::isdotdir(directory->d_name) == true)
            continue;



        ACE_OS::strncpy(tname,
                ACE_TEXT_CHAR_TO_TCHAR(directory->d_name),
                maxnamlen);


        int local_file_count = 0;
        int local_dir_count = 0;
        ACE_stat stat_buf;
        if (ACE_OS::lstat(directory->d_name, &stat_buf) == -1) {
            ACE_ERROR((LM_ERROR,
                    ACE_TEXT("(%T)%@\n"),
                    tname));
            return;
        }

        switch (stat_buf.st_mode & S_IFMT) {
            case S_IFREG: // Either a regular file or an executable.
                /*if (ACE_OS::strcmp(dir_path, root_path) == 0) {
                    continue;
                }*/
                if (ACE_OS::strcmp(directory->d_name, "tasks") == 0) {
                    //cout << "Filename=" << directory->d_name << endl;
                    readTasks(directory->d_name, tasks);
                }
                break;

            case S_IFLNK: // Either a file or directory link, so let's find out.
                if (ACE_OS::stat(directory->d_name, &stat_buf) == -1) {
                    ACE_ERROR((LM_ERROR,
                            tname));
                    return;
                }

                /*if ((stat_buf.st_mode & S_IFMT) == S_IFDIR)
                    // Don't recurse through symbolic directory links!
                    ++dir_count;
                else
                    ++file_count;*/
                break;

            case S_IFDIR:
                //ACE_DEBUG((LM_DEBUG, "entering subdirectory %s\n",
                //tname));
                //reset_i(root_path, tname, tasks);
                reset_i(tname, tasks);
                if (ACE_OS::chdir(ACE_TEXT("..")) == -1) {
                    ACE_ERROR((LM_ERROR,
                            ACE_TEXT("(%T)CgroupSubsystem::reset_i - hdir: %@\n"),
                            dir_path)
                            );
                    return;
                }
                break;

            default: // Must be some other type of file (PIPE/FIFO/device)
                //++file_count;
                break;
        }
    }
}

void CgroupSubsystem::readTasks(const ACE_TCHAR* taskFile, ACE_DLList<String>& tasks) throw (CgroupException&) {
    ifstream infile(taskFile, ios::in);
    if (infile.bad()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::readTasks() - error reading tasks\n")));
        throw CgroupException(ACE_OS::strerror(errno));
    }

    char line[BUFSIZ];

    while (infile.getline(line, BUFSIZ, '\n')) {
        //std::cout << line << std::endl;
        String* str = new String(line);
        tasks.insert_tail(str);
    }
}

void CgroupSubsystem::readTasks(const ACE_TCHAR* taskFile, list<UInt>& tasks) throw (CgroupException&) {
    ifstream infile(taskFile, ios::in);
    if (infile.bad()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::readTasks() (ints) - error reading tasks\n")));
        throw CgroupException(ACE_OS::strerror(errno));
    }

    char line[BUFSIZ];

    while (infile.getline(line, BUFSIZ, '\n')) {
        //std::cout << line << std::endl;
        stringstream ss(line);
        //String str = new String(line);
        UInt t;
        ss >> t;
        tasks.push_back(t);
    }
}

void CgroupSubsystem::removeTree(const ACE_TCHAR* dir_path) throw (CgroupException&) {
    if (ACE_OS::chdir(dir_path) == -1) {
        ACE_ERROR((LM_ERROR,
                ACE_TEXT("(%T)chdir failed: %s\n"),
                dir_path)
                );
    }


    ACE_Dirent dir;
    if (dir.open(ACE_TEXT(".")) == -1) {
        perror("===");
        ACE_ERROR
                ((LM_ERROR, ACE_TEXT("(%T)removeTree: open of dir . at %s failed\n"), dir_path));
    }

    // Since the dir struct d_name type changes depending on the setting
    // of ACE_LACKS_STRUCT_DIR, copy each name into a neutral format
    // array to work on it.
    size_t const maxnamlen = MAXNAMLEN;
    ACE_TCHAR tname[maxnamlen + 1];

    int entry_count = 0;

    for (ACE_DIRENT *directory; (directory = dir.read()) != 0;) {
        // Skip the ".." and "." files.
        if (ACE::isdotdir(directory->d_name) == true)
            continue;

        ACE_OS::strncpy(tname,
                ACE_TEXT_CHAR_TO_TCHAR(directory->d_name),
                maxnamlen);

        ACE_stat stat_buf;
        if (ACE_OS::lstat(directory->d_name, &stat_buf) == -1) {
            ACE_ERROR((LM_ERROR,
                    ACE_TEXT("(%T)%@\n"),
                    tname));
            return;
        }

        switch (stat_buf.st_mode & S_IFMT) {
            case S_IFDIR:
                //ACE_DEBUG((LM_DEBUG, "entering subdirectory %s\n",
                //tname));
                removeTree(tname);
                //ACE_DEBUG((LM_DEBUG, "removed directory %s\n", tname));
                if (ACE_OS::chdir(ACE_TEXT("..")) == -1) {
                    ACE_ERROR((LM_ERROR,
                            ACE_TEXT("(%T)removeTree - chdir: %@\n"),
                            dir_path)
                            );
                    return;
                }
                if (ACE_OS::rmdir(tname) != 0) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::removeTree(): failed rmdir(%s)\n"), tname));
                    throw CgroupException("Unable to remove cgroup directory!");
                }
                break;

            default: // Must be some other type of file (PIPE/FIFO/device)
                break;
        }
    }
}

void CgroupSubsystem::migrateTasks(const ACE_TCHAR* root_path, ACE_DLList<String>& tasks, bool excludeSystemTasks) throw (CgroupException&) {
    String path(root_path);
    path += "/tasks";
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::migrateTasks() path=%s\n"),path.c_str()));    
    if (excludeSystemTasks) {
        getExcludedTasks(tasks);
    }
    String* entry;
    ofstream outfile(path.c_str(), ios::out);
    for (ACE_DLList_Iterator<String> iter(tasks);
            (entry = iter.next()) != 0;
            iter.advance()) {
        //ACE_DEBUG((LM_DEBUG, "mirating task (%s)\n", entry->c_str()));
        String command(*entry);
        //command += "\n";        
        //ACE_DEBUG((LM_DEBUG, "mirating task command(%s)\n",command.c_str()));
        //ofstream outfile(path.c_str(), ios::out);
        outfile << command << flush;

        //outfile.flush();
        //ACE_DEBUG((LM_DEBUG, "mirating task status=%d\n",outfile.good()));
        //outfile.close();
        /*try{
           writeFile(path.c_str(),*entry);
        }catch(CgroupException& ex){            
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ERROR: migrateTasks() - EX=%s\n"), ex.message().c_str()));
        }*/
    }
    outfile.close();
}

void CgroupSubsystem::migrateTask(ACE_hthread_t task,
        const ACE_TCHAR* sourcePath,
        const ACE_TCHAR* destPath) throw (CgroupException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CgroupSubsystem::migrateTasks() 2\n")));
    ACE_stat stat;
    if (ACE_OS::lstat(sourcePath, &stat) != 0) {
        throw CgroupException("Unable to load task file!");
    }
    ACE_DLList<String> tasks;
    stringstream taskNum(stringstream::in | stringstream::out);
    taskNum << task;

    //ACE_DEBUG((LM_DEBUG, "mirating task %s\n",
    //taskNum.str().c_str()));
    readTasks((char*) sourcePath, tasks);
    String* entry;
    for (ACE_DLList_Iterator<String> iter(tasks);
            (entry = iter.next()) != 0;
            iter.advance()) {
        if (*entry == taskNum.str().c_str()) {
            //ACE_DEBUG((LM_DEBUG, "found mirating task %s\n",
            //taskNum.str().c_str()));
            /// task is valid
            ofstream outfile(destPath, ios::out);
            outfile << taskNum.str().c_str();

            //ACE_DEBUG((LM_DEBUG, "mirated task %s\n",
            //taskNum.str().c_str()));
            return;
        }
    }

    throw CgroupException("Task doesn't belong to source container");


}

