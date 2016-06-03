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

#ifndef NGREST_MEMPOOLER_H
#define NGREST_MEMPOOLER_H

#include <vector>
#include <unordered_map>

#include "MemPool.h"

namespace ngrest {

struct Pools
{
    std::vector<MemPool*> used;
    std::vector<MemPool*> unused;
};

/**
 * @brief memory pool manager. intended to manage, store and re-use memory pools
 */
class NGREST_UTILS_EXPORT MemPooler
{
public:
    /**
     * @brief constructor
     */
    MemPooler();

    /**
     * @brief destructor
     */
    ~MemPooler();

    /**
     * @brief obtain memory pool with preferred chunk size
     * @param chunkSize default chunk size of memory pool
     * @return memory pool
     */
    MemPool* obtain(uint64_t chunkSize = NGREST_MEMPOOL_CHUNK_SIZE);

    /**
     * @brief recycle memory pool for later reuse
     * @param pool pool to recycle
     */
    void recycle(MemPool* pool);

private:
    MemPooler(const MemPooler&);
    MemPooler& operator=(const MemPooler&);

private:
    std::unordered_map<uint64_t, Pools> pools;
};

}

#endif // NGREST_MEMPOOLER_H
