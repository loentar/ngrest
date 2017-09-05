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

#ifndef NGREST_JSONWRITER_H
#define NGREST_JSONWRITER_H

namespace ngrest {

class MemPool;
struct Node;

namespace json {

/**
 * @brief JSON writer. writes OM to memory pool
 */
class JsonWriter {
public:
    /**
     * @brief writes OM to memory pool as JSON
     * @param node OM node to write from
     * @param memPool memory pool to write to
     * @param indent indentation
     * @throw AssertException
     */
    static void write(const Node* node, MemPool* memPool, int indent = 0);
};

}
}

#endif
