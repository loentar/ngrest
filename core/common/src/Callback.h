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

#ifndef NGREST_CALLBACK_H
#define NGREST_CALLBACK_H

namespace ngrest {

class Exception;

/**
 * template callback class with one result parameter
 */
template <typename R>
class Callback
{
public:
    /**
     * @brief destructor
     */
    virtual ~Callback() {}

    /**
     * @brief called when call was successful
     * @param result call result
     */
    virtual void success(R result) = 0;

    /**
     * @brief called when call was unsuccessful
     * @param error exception instance with error description
     */
    virtual void error(const Exception& error) = 0;
};


/**
 * template callback class without parameters
 */
class VoidCallback
{
public:
    /**
     * @brief destructor
     */
    virtual ~VoidCallback() {}

    /**
     * @brief called when call was successful
     */
    virtual void success() = 0;

    /**
     * @brief called when call was unsuccessful
     * @param error exception instance with error description
     */
    virtual void error(const Exception& error) = 0;
};

} // namespace ngrest

#endif // NGREST_CALLBACK_H
