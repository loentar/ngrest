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


#include "Exception.h"
#include "MemPooler.h"

namespace ngrest {

// max pools count per size
#define MAX_POOLS_COUNT 128

MemPooler::MemPooler()
{
}

MemPooler::~MemPooler()
{
    for (auto& poolByChunk : pools) {
        for (MemPool* pool : poolByChunk.second.used)
            delete pool;
        for (MemPool* pool : poolByChunk.second.unused)
            delete pool;
    }
    pools.clear();
}

MemPool* MemPooler::obtain(uint64_t chunkSize)
{
    MemPool* pool;
    Pools& poolsByChunk = pools[chunkSize];
    if (!poolsByChunk.unused.empty()) {
        pool = poolsByChunk.unused.back();
        poolsByChunk.unused.pop_back();
    } else {
        pool = new MemPool(chunkSize);
        if (poolsByChunk.used.empty()) {
            poolsByChunk.used.reserve(MAX_POOLS_COUNT);
            poolsByChunk.unused.reserve(MAX_POOLS_COUNT);
        }
    }

    poolsByChunk.used.push_back(pool);
    return pool;
}

void MemPooler::recycle(MemPool* pool)
{
    NGREST_ASSERT_PARAM(pool);
    Pools& poolsByChunk = pools[pool->getChunkSize()];
    for (auto it = poolsByChunk.used.begin(); it != poolsByChunk.used.end(); ++it) {
        if (*it == pool) {
            poolsByChunk.used.erase(it);
            break;
        }
    }

    if (poolsByChunk.unused.size() >= MAX_POOLS_COUNT) {
        delete pool;
    } else {
        pool->reset();
        pool->trim();
        poolsByChunk.unused.push_back(pool);
    }
}

}
