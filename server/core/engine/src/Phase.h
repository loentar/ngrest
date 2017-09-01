/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
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

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */


#ifndef PHASE_H
#define PHASE_H

#include "ngrestengineexport.h"

namespace ngrest {

/**
 * @brief Message processing phase
 */
enum class Phase
{
    Header,         //!< only message header has been read, no body (if any) is yet read
    PreDispatch,    //!< full request including body has been read, not yet parsed
    PreInvoke,      //!< the service is going to be invoked with parsed/generated OM request
    PostDispatch,   //!< request is processed by the service, response written to OM, but not to output buffer
    PreSend,        //!< response body written to output buffer and going to send to client
    Count           //!< the number of phases
};

class NGREST_ENGINE_EXPORT PhaseInfo
{
public:
    static const char* phaseToString(Phase phase);
};

} // namespace ngrest

#endif // PHASE_H

