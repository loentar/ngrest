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

#ifndef NGREST_TESTDEPLOYMENTWRAPPER_H
#define NGREST_TESTDEPLOYMENTWRAPPER_H

#include <ngrest/engine/ServiceWrapper.h>

namespace ngrest {

class TestDeployment;

class TestDeploymentWrapper: public ServiceWrapper
{
public:
    TestDeploymentWrapper();
    ~TestDeploymentWrapper();

    virtual Service* getServiceImpl() override;
    virtual void invoke(const OperationDescription* operation, MessageContext* context) override;
    virtual const ServiceDescription* getDescription() const override;

private:
    TestDeployment* service;
};

} // namespace ngrest

#endif // NGREST_TESTDEPLOYMENTWRAPPER_H
