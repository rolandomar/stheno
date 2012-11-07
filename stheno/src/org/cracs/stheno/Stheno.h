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
 * File:   Stheno.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 29, 2010, 2:54 PM
 */

#ifndef STHENO_H
#define	STHENO_H

#include <ace/INET_Addr.h>
#include <ace/ARGV.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/service/UserService.h>
#include <stheno/service/ClientParams.h>
#include <stheno/service/ServiceParams.h>
#include <stheno/service/ServiceClient.h>
#include <stheno/exception/RuntimeException.h>
#include <stheno/core/p2p/common/ServiceInfo.h>
#include <stheno/core/p2p/common/ServiceInstanceInfo.h>
#include <stheno/core/DefaultServiceFactory.h>
#include <stheno/core/LocalService.h>
#include <euryale/qos/RTParams.h>
#include <stheno/core/p2p/Overlay.h>
#include <stheno/exception/RuntimeException.h>

/** 
 * @class Stheno
 *
 * @brief The runtime frontend class.
 * 
 * This class mediates the access to the underlying runtime core, 
 * implemented in @e SthenoCore.
 *
 */
class OverlayInterface;
//class SthenoCorePtr;
typedef ACE_Strong_Bound_Ptr<SthenoCore, ACE_Recursive_Thread_Mutex> SthenoCorePtr;
class Stheno {
public:
   
    friend class UserService;
    friend class Overlay;

    virtual ~Stheno();
    /**
     * Creates a new runtime and respective frontend with @a uuid as the node id.
     * @param[in] uuid Runtime's node id.
     * @return Returns the frontend for the newly created runtime with node id @a uuid.
     */
    static Stheno* createRuntime(UUIDPtr& uuid);
    /**
     * Creates a new runtime and respective frontend with random node id.
     * @param[in] uuid Runtime's node id.
     * @return Returns the frontend for the newly created runtime with node id @a uuid.
     */
    static Stheno* createRuntime();
    /**
     * 
     * @return Returns a new frontend for the previously created runtime.
     */
    Stheno* duplicate();

    /**
     * Opens the frontend that in turn opens the runtime.
     * Throws @RuntimeException in case of failure.
     */
    //void open(int argc, char** argv) throw (RuntimeException&);
    void open(ACE_ARGV& args) throw (RuntimeException&);

    /**
     * Closes the runtime frontend.
     * Throws @RuntimeException in case of failure.
     */
    void close() throw (RuntimeException&);

    /**
     * Starts service identified by @a sid with parameters @a params at a 
     * location chosen by the middleware.
     * Throws @e ServiceException in case of failure.
     * 
     * @param[in] sid Service id. 
     * @param[in] params Service params.
     * @param[out] iid Returns the @e iid of the deployed service.
     */
    void startService(const UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid,UUIDPtr& deployUUIDD) throw (ServiceException&);
    /**
     * Starts service identified by @a sid at node @a uuid with parameters @a params. 
     * Throws @a ServiceException in case of failure.
     * @param[in] sid Service id.
     * @param[in] uuid Node id. 
     * @param[in] params Service params.
     * @param[out] iid Returns the instance @e iid of the deployed service.
     *      
     */
    void startService(UUIDPtr& sid, UUIDPtr& uuid, ServiceParamsPtr& params, UUIDPtr& iid) throw (ServiceException&);

    /**
     * Stops service instance @iid of service @e sid.
     * @param[in] sid Service id.
     * @param[in] iid Service instance id.s
     */
    void stopService(UUIDPtr& sid, UUIDPtr& iid);

    //list of services
    /**
     * Retrieves the list of the local running instances of service @a sid.
     * @param[in] sid Service id.
     * @return List of local running instances of service @a sid.
     */
    list<UUIDPtr>* getListOfRuntimes(UUIDPtr& sid) throw (ServiceException&);


    ServiceFactory& getServiceFactory();

    /**
     * Creates a new client for service @a sid with the middleware choosing the 
     * proper @a idd to bind to (ex: MapReduce).

     * @param[in] sid Service id.
     * @return Returns the client to the service.
     */
    ServiceClient* getClient(const UUIDPtr& sid,ClientParamsPtr& clientParams);
    
    
    /**
     * Creates a new client for service @a sid with instance @a idd (ex: RPC).
     * 
     * @param[in] sid Service id.
     * @return Returns the client to the service.
     */
    ServiceClient* getClient(const UUIDPtr& sid, const UUIDPtr& iid,ClientParamsPtr& clientParams) 
            throw (RuntimeException&, ServiceException&);

    /**
     * Retrieves the node's id.
     * Throws @e RuntimeException in case of failure.
     * 
     * @return Returns the node id.
     * 
     */
    void getUUID(UUIDPtr& uuid) throw (RuntimeException&);

    /**
     * Attachs a overlay to the runtime.
     * @param[in] overlay The plugin overlay to be attached to the runtime.
     */
    void attachOverlay(Overlay* overlay) throw (RuntimeException&);

    /**
     * Retrieves the overlay interface.
     * @return Returns the @a OverlayInterface of the runtime.
     */
    OverlayInterface* getOverlayInterface();

private:
    OverlayInterface* m_overlayInterface;
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;

    Stheno();
    Stheno(UUIDPtr& rid);
    Stheno(const SthenoCorePtr& runtime);
    //local services
    //Callback from Service
    void onServiceClose(UserService* service);
    void addService(UserService* service);
    //get local service instance (if available)
    UserServicePtr* getService(UUIDPtr& sid);

    SthenoCorePtr& getRuntime() throw (RuntimeException&);
private:
    SthenoCorePtr m_runtime;


};

#endif	/* STHENO_H */

