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
 * File:   UUIDDistance.cpp
 * Author: rmartins
 * 
 * Created on August 16, 2010, 12:10 PM
 */

#include "UUIDDistance.h"

UUIDDistance::UUIDDistance() {
}

UUIDDistance::~UUIDDistance() {
}

int UUIDDistance::LD(UUIDPtr& u1, UUIDPtr& u2) {
    return LD(u1.get(),u2.get());

}

int UUIDDistance::LD(UUID* u1, UUID* u2) {
    String& u1Str = u1->toString();
    String& u2Str = u2->toString();
    return LD(u1Str,u2Str);
}

int UUIDDistance::LD(String& s, String& t) {
    int** d; // matrix
    int n; // length of s
    int m; // length of t
    int i; // iterates through s
    int j; // iterates through t
    char s_i; // ith character of s
    char t_j; // jth character of t
    int cost; // cost
    // Step 1
    n = s.length();
    m = t.length();
    if (n == 0) {
        return m;
    }
    if (m == 0) {
        return n;
    }
    //d = new int[n + 1][m + 1];
    d = new int*[n+1];
    for(i=0; i < n+1; i++){
        d[i] = new int[m+1];
    }
    // Step 2
    for (i = 0; i <= n; i++) {
        d[i][0] = i;
    }
    for (j = 0; j <= m; j++) {
        d[0][j] = j;
    }
    // Step 3
    for (i = 1; i <= n; i++) {
        //s_i = s.charAt(i - 1);
        s_i = s.fast_rep()[i - 1];
        // Step 4
        for (j = 1; j <= m; j++) {
            //t_j = t.charAt(j - 1);
            t_j = t.fast_rep()[j - 1];
            // Step 5
            if (s_i == t_j) {
                cost = 0;
            } else {
                cost = 1;
            }
            // Step 6
            d[i][j] = Minimum(d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost);
        }
    }
    // Step 7
    int ret = d[n][m];
    for(i=0; i < n+1; i++){
        delete d[i];
    }
    delete d;
    return ret;
}

//****************************
// Get minimum of three values
//****************************

int UUIDDistance::Minimum(int a, int b, int c) {
    int mi;

    mi = a;
    if (b < mi) {
        mi = b;
    }
    if (c < mi) {
        mi = c;
    }
    return mi;

}

