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
 * File:   Alignment.h
 * Author: rmartins
 *
 * Created on February 7, 2010, 8:50 PM
 */

#ifndef _ALIGNMENT_H
#define	_ALIGNMENT_H
#include <euryale/Types.h>

class Alignment {
public:
    enum {ZERO_ALIGNMENT = 0};
    Alignment();
    Alignment(int type);
    virtual ~Alignment();

    int getType(){
        return m_type;
    }
      
protected:
    int m_type;
private:
    Alignment(const Alignment& orig);
};

#endif	/* _ALIGNMENT_H */

