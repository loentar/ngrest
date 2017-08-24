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

#include "SnapShots.h"
#include <ngrest/utils/Exception.h>
#include <ngrest/utils/MemPool.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef WIN32
#define stat _stat
#endif

namespace ngrest {

static const int POLL_INTERVAL = 1; // min period between deployments

static time_t timeModified(const std::string& path)
{
    struct stat attr;
    int err = stat(path.c_str(), &attr);
    NGREST_ASSERT(err == 0, std::string("Failed to read directory ") + strerror(errno) + " " + path);
    return attr.st_mtime;
}

SnapShot::SnapShot(const std::string& servicesPath, const std::string& filtersPath,
    time_t timeServices, time_t timeFilters, SnapShot* oldSnapShot, SnapShots* snapShots) :
    deployment(dispatcher), filterDeployment(filterDispatcher), engine(dispatcher)
{
	engine.setSnapShots(snapShots);
    deployment.deployAll(servicesPath, oldSnapShot ? &oldSnapShot->deployment : 0);

    if (!filtersPath.empty()) {
        engine.setFilterDispatcher(&filterDispatcher);
        filterDeployment.deployAll(filtersPath, oldSnapShot ? &oldSnapShot->filterDeployment : 0);
    }

    this->timeServices = timeServices;
    this->timeFilters = timeFilters;
}

SnapShot::~SnapShot()
{
    deployment.undeployAll();
    filterDeployment.undeployAll();
}

SnapShots::Ref SnapShots::latest()
{
    pollDeploy();
    for (int i=0; i<10*LIMIT; i++) {
        int version = this->version;
        Ref ref(slots[version % LIMIT]);
        if (ref.slot.version == version) return ref;
    }
    NGREST_THROW_ASSERT("Failed to lock snapshot");
}

void SnapShots::pollDeploy()
{
    time_t now = time(0);
    time_t last = timePoll;
    if (last > now || !timePoll.compare_exchange_weak(last, now + POLL_INTERVAL)) return;

    std::lock_guard<std::mutex> guard(deployMutex);

    int prevVersion = version;
    for (; oldestVersion < prevVersion; oldestVersion++) {
        Slot& slot = slots[oldestVersion % LIMIT];
        if (slot.snapShot) {
			if (slot.refs > 0) break;
            delete slot.snapShot;
            slot.snapShot = 0;
        }
    }

    int newVersion = prevVersion + 1;
    Slot& slot = slots[newVersion % LIMIT];
    if (slot.snapShot) return; // no room

    time_t timeServices = timeModified(servicesPath);
    time_t timeFilters = filtersPath.empty() ? 0 : timeModified(filtersPath);

    if (timeDeployedServices < timeServices ||
        timeDeployedFilters < timeFilters) {

        SnapShot* oldSnapShot = slots[newVersion % LIMIT].snapShot;
        slot.snapShot = new SnapShot(servicesPath, filtersPath, timeServices, timeFilters, oldSnapShot, this);
        slot.version = newVersion;

        timeDeployedServices = timeServices;
        timeDeployedFilters = timeFilters;
        version = newVersion;
    }
}

void SnapShots::undeployAll()
{
    std::lock_guard<std::mutex> guard(deployMutex);
    for (Slot& slot : slots) {
        if (slot.refs == 0 && slot.snapShot) {
            delete slot.snapShot;
            slot.snapShot = 0;
        }
    }
}

static char* printLong(char* out, long val)
{
	sprintf(out, "%ld", val);
	return out;
}

template<size_t n> char* printTime(char (&out) [n], time_t time)
{
	if (!time) *out = 0;
	else strftime(out, n, "%F %T", localtime(&time));
	return out;
}

void SnapShots::dumpHtml(MemPool* out)
{
	char buff[30];
    std::lock_guard<std::mutex> guard(deployMutex);
    out->putCString("<table border=1><tr>"
        "<td>#</td>"
        "<td>Service Time</td>"
        "<td>Filter Time</td>"
        "</tr>");
    for (int i=version; i>=oldestVersion; i--) {
        Slot& slot = slots[i % LIMIT];
        SnapShot* shot = slot.snapShot;
        if (shot) {
			out->putCString("<tr><td>");
			out->putCString(printLong(buff, slot.version));
			out->putCString("</td><td>");
			out->putCString(printTime(buff, shot->timeServices));
			out->putCString("</td><td>");
			out->putCString(printTime(buff, shot->timeFilters));
			out->putCString("</td></tr>");
        }
    }
	out->putCString("</table>");
}

} // namespace ngrest
