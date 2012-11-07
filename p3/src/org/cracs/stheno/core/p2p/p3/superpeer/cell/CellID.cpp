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
 * File:   CellID.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 30, 2010, 2:22 PM
 */

#include "CellID.h"
//String CellID::ROOT_CELL_ID_STR("00000000000000000000000000000000");
//String CellID::INVALID_CELL_ID_STR("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");

//String CellID::ROOT_CELL_ID_STR("00000000000000000000000000000000");
String CellID::INVALID_CELL_ID_STR("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");

CellIDPtr CellID::INVALID_CELL_ID(new CellID(INVALID_CELL_ID_STR));
//CellIDPtr CellID::ROOT_CELL_ID(new CellID(ROOT_CELL_ID_STR));


UUIDPtr CellID::INVALID_CELL_ID_UUIDPTR(new CellID(INVALID_CELL_ID_STR));
//UUIDPtr CellID::ROOT_CELL_ID_UUIDPTR(new CellID(ROOT_CELL_ID_STR));

int CellID::KEY_STR_SIZE = 32;
int CellID::CHAR_PER_RANK = 1;

CellID::CellID(const char* str) : UUID(str) {
}

CellID::CellID(const UUID& id): UUID(id){

}

CellID::CellID(UUID* id) : UUID(*id) {
    //delete id;
}

CellID::CellID(String& id) : UUID(id.c_str()) {
}

CellID::CellID(InputStream& inputStream) : UUID(inputStream) {
    //this->deserialize(inputStream);

}

CellID::CellID(const CellID& orig) : UUID(orig) {
}

CellID::~CellID() {
}

CellID* CellID::getNextGroupID() throw (int) {
    String prefix(toString());
    int i = 0;
    for (i = CHAR_PER_RANK; i < prefix.length(); i += CHAR_PER_RANK) {
        bool flag = true;
        for (int j = 0; j < CHAR_PER_RANK; j++) {
            if (prefix.c_str()[i + j] != '0') {
                flag = false;
                break;
            }
        }
        if (flag) {
            break;
        }
    }
    int lastCellPos = i - CHAR_PER_RANK;
    int newCellPos = i;
    if (newCellPos == CHAR_PER_RANK) {
        lastCellPos = newCellPos;
    }
    String result;
    int lowestRank = Integer::parseInt(prefix.substring(lastCellPos, CHAR_PER_RANK), 16);
    int treeSpan = 2; //Cell::getTreeSpan(getCellDepth()
    if (lowestRank < treeSpan) {
        //update the current level
        String newLowerRank = Integer::toHexString(++lowestRank);
        //handle the one digit number case
        for (int k = 0; k < CHAR_PER_RANK - newLowerRank.length(); k++) {
            newLowerRank = '0' + newLowerRank;
        }
        //result = prefix.substring(0, lastCellPos) + newLowerRank + prefix.substring(lastCellPos + CHAR_PER_RANK, KEY_SIZE);
        result = prefix.substring(0, lastCellPos) + newLowerRank + prefix.substring(lastCellPos + CHAR_PER_RANK, KEY_STR_SIZE - (lastCellPos + CHAR_PER_RANK));
    } else {
        if (newCellPos == KEY_STR_SIZE) {
            //throw new Exception("No more IDs possible!");
            throw (-1);
        }
        result = prefix.substring(0, newCellPos);
        for (int j = 0; j < CHAR_PER_RANK - 1; j++) {
            result += '0';
        }
        result += Integer::toHexString(1) + prefix.substring(newCellPos + CHAR_PER_RANK, KEY_STR_SIZE - (newCellPos + CHAR_PER_RANK));
    }
    return new CellID(result);
}

CellID* CellID::getNextGroupID_2() throw (int) {
    vector<int> lvls;
    int treeSpan = 2;
    this->getCellLevels(lvls);
    int depth = this->getCellDepth();
    if(depth == 0){
        lvls[1] = 1;
        return convertLvlsToCellID(lvls);
    }
    for(int i=depth; i > 0; i--){
        if(lvls[i] < treeSpan){
            lvls[i]++;
            break;
        }else{
            if(i == 1){
                for(int j=1; j <= depth; j++){
                    lvls[j] = 1;
                }
                lvls[depth+1] = 1;
            }
        }
    }
    return convertLvlsToCellID(lvls);
}
CellID* CellID::convertLvlsToCellID(vector<int>& lvls){
    String result;

    for(int i=0; i <lvls.size(); i++){
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)%d",lvls[i]);
        result += Integer::toHexString(lvls[i]);
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)\n");
    CellID* cellID = new CellID(result);
    return cellID;
}

CellID* CellID::getParentGroupID() throw (int) {
    String prefix(toString());
    int i = 0;

    for (i = CHAR_PER_RANK; i < prefix.length(); i += CHAR_PER_RANK) {
        bool flag = true;
        for (int j = 0; j < CHAR_PER_RANK; j++) {
            if (prefix[i + j] != '0') {
                flag = false;
                break;
            }
        }
        if (flag) {
            break;
        }
    }
    if (i == CHAR_PER_RANK) //top rank
    {
        return 0;
    }

    //char[] tempString = prefix.c_str();
    int lastCellPos = i - CHAR_PER_RANK;
    for (int j = 0; j < CHAR_PER_RANK; j++) {
        //tempString[lastCellPos + j] = '0';
        prefix[lastCellPos + j] = '0';

    }
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)PREFIX=%s\n"), prefix.c_str()));
    return new CellID(prefix);
    //return new CellID(new String(tempString));//tempString.toString());
}

bool CellID::belongsToCellTree(CellID* cellID) {
    if (*cellID == *this) {
        return true;
    }
    int cellDepth = cellID->getCellDepth();
    int thisDepth = this->getCellDepth();
    int depth = 0;
    if (cellDepth > thisDepth) {
        depth = thisDepth;
    } else {
        depth = cellDepth;
    }
    vector<int> lvl1;
    cellID->getCellLevels(lvl1);
    vector<int> lvl2;
    getCellLevels(lvl2);
  
    for (int i = 1; i <= depth; i++) {
        if (lvl1[i] != lvl2[i]) {
            if (lvl1[i] == 0 && lvl2[i] > 0) {
                return true;
            }
            if (lvl2[i] == 0 && lvl1[i] > 0) {
                return true;
            }
            return false;
        }
    }
    return true;
}

/*bool CellID::isRoot() {
    return (*this == *(ROOT_CELL_ID.get()));
}*/

int CellID::getCellSpan(int depth) {
    String prefix(toString());
    int index = depth * CHAR_PER_RANK;
    return Integer::parseInt(prefix.substring(index, CHAR_PER_RANK), 16);
}

int CellID::getCellDepth() {
    String prefix(toString());
    int i = 0;
    for (i = CHAR_PER_RANK; i < prefix.length(); i += CHAR_PER_RANK) {
        bool flag = true;
        for (int j = 0; j < CHAR_PER_RANK; j++) {
            if (prefix.c_str()[i + j] != '0') {
                flag = false;
                break;
            }
        }
        if (flag) {
            break;
        }
    }
    if (i == CHAR_PER_RANK) {
        return 0;
    } else {
        return i / CHAR_PER_RANK - 1;
    }
}

void CellID::getCellLevels(vector<int>& vec) {
    String prefix = this->toString();
    int index = 0;
    //vector<int>* vec = new vector<int>();
    vec.push_back(0);
    index += CHAR_PER_RANK;
    for (int i = CHAR_PER_RANK; i < KEY_STR_SIZE; i++) {
        int cur = Integer::parseInt(prefix.substring(index, CHAR_PER_RANK), 16);
        //if (cur == 0) {
            //break;
        //}
        vec.push_back(cur);
        index += CHAR_PER_RANK;
    }
    //return vec;//lvls;
}