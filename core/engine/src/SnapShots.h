/*
 *  Copyright 2017 NAM system, a.s.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef NGREST_SNAPSHOTS_H
#define NGREST_SNAPSHOTS_H

#include <atomic>
#include <mutex>
#include "Engine.h"
#include "FilterDispatcher.h"
#include "FilterDeployment.h"
#include "ServiceDispatcher.h"
#include "Deployment.h"

namespace ngrest {

/**
 * @brief snapshot of deployment, contains objects that where originally singletons in modules/share/ngrest_server.cpp.
 */
class SnapShot {
public:
    SnapShot(const std::string& servicesPath, const std::string& filtersPath, time_t timeServices, time_t timeFilters, SnapShot* oldSnapShot, class SnapShots* snapShots);
    ~SnapShot();

    ngrest::ServiceDispatcher dispatcher;
    ngrest::Deployment deployment;
    ngrest::FilterDispatcher filterDispatcher;
    ngrest::FilterDeployment filterDeployment;
    ngrest::Engine engine;
    time_t timeServices; // directory modify time, just for serverstatus
    time_t timeFilters;
};

/**
 * @brief all snapshots of deployments
 */
class SnapShots {

    /**
     * @brief Slot structure is ment to deal with dangling SnapShot pointer. Slot is permanent to allow lockfree reading. Concurrent write is detected by comparing version before and after read. If 'refs' is increased without detecting version change, pointer is safe.
     */
    struct Slot {
        /**
         * @brief snapShot contained in slot
         */
        SnapShot* volatile snapShot;
        /**
         * @brief version of snapShot
         */
        long volatile version;
        /**
         * @brief number of references (Ref instances) to this slot
         */
        std::atomic<int> refs;
    };

    /**
     * @brief reference to Slot, modifies counter in Slot in RAII manner
     */
    class Ref {
    public:
        Slot& slot;
        Ref(Slot& _slot) : slot(_slot) { slot.refs++; }
        Ref(const Ref& src) : slot(src.slot) { slot.refs++; }
        ~Ref() { slot.refs--; }
    };


    /**
     * @brief max snapshots at once
     */
    static const int LIMIT = 8;

    /**
     * @brief reference to Slot, modifies counter in Slot in RAII manner
     */
    Slot slots[LIMIT] = {};

    /**
     * @brief for cleaning unused snapshots
     */
    int oldestVersion {};

    /**
     * @brief latest version, chooses slot for new request
     */
    std::atomic<int> version {};

    /**
     * @brief last time when directory change was polled
     */
    std::atomic<time_t> timePoll {};

    /**
     * @brief only one thread can make new snapshot or make cleaning
     */
    std::mutex deployMutex;

    /**
     * @brief services directory modify time at time of deployment
     */
    time_t timeDeployedServices;

    /**
     * @brief filters directory modify time at time of deployment
     */
    time_t timeDeployedFilters;

public:
    /**
     * @brief directory to search for services
     */
    std::string servicesPath;

    /**
     * @brief directory to search for filters
     */
    std::string filtersPath;

    /**
     * @brief return latest deployment snapshot
     */
    Ref latest();

    /**
     * @brief poll for directory changs and deploy new snapshot
     */
    void pollDeploy();

    /**
     * @brief undeploy all snapshots
     */
    void undeployAll();

    /**
     * @brief dump as html table
     */
    void dumpHtml(class MemPool* out);
};

}

#endif
