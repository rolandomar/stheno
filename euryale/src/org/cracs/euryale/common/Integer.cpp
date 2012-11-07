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
 * File:   Integer.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 6, 2010, 5:10 PM
 */

#include "Integer.h"
const char Integer::digits[] = {
    '0', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z'
};

Integer::Integer() {
}

Integer::~Integer() {
}

String Integer::toUnsignedString(int i){
    return toUnsignedString(i,10);
}

String Integer::toUnsignedString(int i, int shift) {
    char buf[32];
    ACE_OS::memset(buf,0,32);
    ACE_OS::itoa(i,buf,shift);
    String str(buf, 32);    
    return str;
    /*char buf[32];
    int charPos = 32;
    int radix = 1 << shift;
    int mask = radix - 1;
    do {
        printf("i=%d digits[i & mask]=%d\n",i,digits[i & mask]);
        buf[--charPos] = digits[i & mask];
        i = i >> shift;
        printf("i=%d\n",i);
    } while (i != 0);
    String str(buf, 32);    
    return str.substring(charPos, (32 - charPos));*/
}

int Integer::parseInt(String str, int base) {
    return strtoul(str.c_str(), 0, base);
}

int Integer::parseInt(char* s, int base) {
    return strtoul(s, 0, base);
}

int Integer::parseInt(const char* s, int base) {
    return strtoul(s, 0, base);
}
