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
 * File:   CellID.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 30, 2010, 2:22 PM
 */

#ifndef CELLID_H
#define	CELLID_H

#include <vector>
using std::vector;

#include <euryale/Types.h>
#include <euryale/common/Integer.h>
//#include <euryale/common/uuid/UUID.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/common/io/ByteInputStream.h>


class CellID;
typedef ACE_Strong_Bound_Ptr<CellID, ACE_Recursive_Thread_Mutex> CellIDPtr;


class CellID: public UUID {
public:
    //static String ROOT_CELL_ID_STR;
    static String INVALID_CELL_ID_STR;

    static int KEY_STR_SIZE;
    static int CHAR_PER_RANK;

    static CellIDPtr INVALID_CELL_ID;
    //static CellIDPtr ROOT_CELL_ID;
    
    
    static UUIDPtr INVALID_CELL_ID_UUIDPTR;
    //static UUIDPtr ROOT_CELL_ID_UUIDPTR;
    //static CellIDPtr INVALID_CELL_ID;
    //static CellIDPtr ROOT_CELL_ID;

    
    CellID(const char* str);
    CellID(UUID* id);
    CellID(const UUID& id);
    CellID(String& id);
    CellID(const CellID& orig);
    CellID(InputStream& inputStream);

    //bool isRoot();

    int getCellSpan(int depth);

    int getCellDepth();
    
    void getCellLevels(vector<int>& vec);

    CellID* getNextGroupID() throw(int);
    CellID* getNextGroupID_2() throw(int);
    static CellID* convertLvlsToCellID(vector<int>& lvls);

    CellID* getParentGroupID() throw(int);

    bool belongsToCellTree(CellID* cellID);
    
    UUID* uuid(){
        return this;
    }

    virtual ~CellID();
private:

};

//typedef ACE_Strong_Bound_Ptr<CellID, ACE_Recursive_Thread_Mutex> CellIDPtr;

#endif	/* CELLID_H */

