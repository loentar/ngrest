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

#ifndef NGREST_TESTFILTERGROUP_H
#define NGREST_TESTFILTERGROUP_H

#include <ngrest/engine/FilterGroup.h>

namespace ngrest {

class TestFilterGroup: public FilterGroup
{
public:
    TestFilterGroup();
    ~TestFilterGroup();
    const std::string& getName() const override;
    const FiltersMap& getFilters() const override;

private:
    Filter* filter;
    FiltersMap filters;
};

}

#endif // NGREST_TESTFILTERGROUP_H
