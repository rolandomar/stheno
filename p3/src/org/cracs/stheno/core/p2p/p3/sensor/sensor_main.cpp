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
 * File:   leaf_main.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 2, 2010, 11:07 AM
 */

#include <cstdlib>
#include <stheno/Stheno.h>
#include <stheno/core/p2p/Overlay.h>
using namespace std;
#include <ace/DLL.h>
#include <ace/svc_export.h>
#include <ace/ARGV.h>

/*
 * 
 */

typedef Overlay* (*create_overlay) (void);

int main(int argc, char** argv) {

    //SuperPeer p;
    ACE_UNUSED_ARG(argc);
    ACE_UNUSED_ARG(argv);
    ACE_ARGV args(argc,argv);    

    ACE_DLL dll;
    //int retval = dll.open(ACE_DLL_PREFIX ACE_TEXT("(%T)superpeer_plugin"));
    int retval = dll.open("libsensor_plugin");

    if (retval != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)HEERE: error\n")));
        ACE_ERROR_RETURN((LM_ERROR,
                "%@",
                "dll.open"),
                -1);
    }
    create_overlay mc = 0;

    // Cast the void* to non-pointer type first - it's not legal to
    // cast a pointer-to-object directly to a pointer-to-function.
    void *void_ptr = dll.symbol(ACE_TEXT("create_overlay"));

    ptrdiff_t tmp = reinterpret_cast<ptrdiff_t> (void_ptr);

    mc = reinterpret_cast<create_overlay> (tmp);

    if (mc == 0) {
        ACE_ERROR_RETURN((LM_ERROR,
                "%@",
                "dll.symbol"),
                -1);
    }

    {
        Overlay* overlay = mc();
        Stheno* runtime = Stheno::createRuntime();
        runtime->attachOverlay(overlay);
        try {
            runtime->open(args);
        } catch (...) {
        }

        while (getchar() != 'q');
        delete runtime;

        //magazine->getUUID();
    }

    dll.close();
    getchar();
    return 0;
}


