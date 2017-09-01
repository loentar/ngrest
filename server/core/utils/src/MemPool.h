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

#ifndef NGREST_MEMPOOL_H
#define NGREST_MEMPOOL_H

#include <stdint.h>
#include <new>
#include "ngrestutilsexport.h"

namespace ngrest {

#define NGREST_MEMPOOL_CHUNK_SIZE 4096

/**
 * @brief Memory pool to store large amount of small POCOs and joined strings
 */
class NGREST_UTILS_EXPORT MemPool
{
public:
    /**
     * @brief chunk
     */
    struct Chunk
    {
        char* buffer;           //!< begin of chunk buffer
        uint64_t bufferSize;    //!< buffer size
        uint64_t size;          //!< current size of chunk
    };

public:
    /**
     * @brief constructor
     * @param chunkSize default chunk size
     */
    MemPool(uint64_t chunkSize = NGREST_MEMPOOL_CHUNK_SIZE);

    /**
     * @brief destructor
     */
    ~MemPool();

    /**
     * @brief allocate data in memory pool
     */
    template <typename T> inline T* alloc()
    {
        return new (grow(sizeof(T))) T();
    }

    /**
     * @brief allocate data in memory pool
     * @param p1 parameter to pass to constructor
     */
    template <typename T, typename P1> inline T* alloc(P1 p1)
    {
        return new (grow(sizeof(T))) T(p1);
    }

    /**
     * @brief allocate data in memory pool
     * @param p1 parameter to pass to constructor
     * @param p2 parameter to pass to constructor
     */
    template <typename T, typename P1, typename P2> inline T* alloc(P1 p1, P2 p2)
    {
        return new (grow(sizeof(T))) T(p1, p2);
    }

    /**
     * @brief allocate data in memory pool
     * @param p1 parameter to pass to constructor
     * @param p2 parameter to pass to constructor
     * @param p3 parameter to pass to constructor
     */
    template <typename T, typename P1, typename P2, typename P3> inline T* alloc(P1 p1, P2 p2, P3 p3)
    {
        return new (grow(sizeof(T))) T(p1, p2, p3);
    }

    /**
     * @brief just allocate data for given type, don't call constructor
     */
    template <typename T> inline T* allocNoCtor()
    {
        return reinterpret_cast<T*>(grow(sizeof(T)));
    }

    /**
     * @brief reset memory pool, but don't free memory allocated. intended for memory-pool re-use
     */
    void reset();

    /**
     * @brief free all allocated memory
     */
    void free();

    /**
     * @brief tests if memory pool donesn't contain any data
     * @return true if memory pool donesn't contain any data
     */
    bool isClean() const;

    /**
     * @brief put a char into memory pool
     * @param ch char to put
     */
    inline void putChar(const char ch)
    {
        *grow(sizeof(char)) = ch;
    }

    /**
     * @brief put C-string into memory pool
     * @param string C-string to put
     * @param terminate add '\0' after string
     * @return begin to string written
     */
    char* putCString(const char* string, bool terminate = false);

    /**
     * @brief put C-string into memory pool
     * @param string C-string to put
     * @param size write only first string's number of chars
     * @param terminate add '\0' after string
     * @return begin to string written
     */
    char* putCString(const char* string, uint64_t size, bool terminate = false);

    /**
     * @brief put binary data into memory pool
     * @param data data to write
     * @param size size of data
     * @return
     */
    char* putData(const char* data, uint64_t size);

    /**
     * @brief get chunks allocated
     * @return chunks
     */
    inline Chunk const* getChunks() const
    {
        return chunks;
    }

    /**
     * @brief get chunks allocated
     * @return chunks
     */
    inline Chunk* getChunks()
    {
        return chunks;
    }

    /**
     * @brief get number of chunks allocated
     * @return number of chunks
     */
    inline int getChunkCount() const
    {
        return chunksCount;
    }

    /**
     * @brief get last chunk used
     * @return last chunk used
     */
    inline Chunk const* getLastChunk() const
    {
        return currChunk;
    }

    /**
     * @brief get last chunk used
     * @return last chunk used
     */
    inline Chunk* getLastChunk()
    {
        return currChunk;
    }

    /**
     * @brief allocate data in memory pool
     * @param growSize size of data
     * @return pointer to allocated data
     */
    inline char* grow(uint64_t growSize)
    {
        uint64_t offset;
        if (currChunk == nullptr || (currChunk->size + growSize) > currChunk->bufferSize) {
            offset = 0;
            newChunk((growSize > chunkSize) ? growSize : chunkSize);
        } else {
            offset = currChunk->size;
        }

        currChunk->size += growSize;
        return currChunk->buffer + offset;
    }

    /**
     * @brief shrink last chunk by number of bytes specified
     * @param shrinkSize number of last bytes to remove from memory pool
     * @return true if success
     */
    inline bool shrinkLastChunk(uint64_t shrinkSize)
    {
        if (currChunk == nullptr || currChunk->size < shrinkSize)
            return false;

        currChunk->size -= shrinkSize;
        return true;
    }

    /**
     * @brief get total size of memory pool (data allocated with grow/allocate functions)
     * @return size of memory pool
     */
    inline uint64_t getSize() const
    {
        uint64_t result = 0;
        if (currChunk) {
            for (const MemPool::Chunk* chunk = chunks; chunk <= currChunk; ++chunk)
                result += chunk->size;
        }
        return result;
    }

    /**
     * @brief concatenate all chunks into one continuous memory fragment
     *   WARNING: after this operation existing stored pointers will be invalidated
     * @param terminate if true, append '\0' to the existing data
     * @return pointer to single chunk
     */
    Chunk* flatten(bool terminate = true);

    /**
     * @brief reserve number of bytes
     * @param size number of bytes to reserve
     */
    void reserve(uint64_t size);

    /**
     * @brief get default chunk size
     * @return default chunk size
     */
    inline uint64_t getChunkSize() const
    {
        return chunkSize;
    }

    /**
     * @brief remove chunks after currChunk
     */
    void trim();

private:
    void newChunk(uint64_t size = NGREST_MEMPOOL_CHUNK_SIZE);

private:
    const uint64_t chunkSize;

    Chunk* chunks = nullptr;
    int chunksReserved = 0;
    int chunksCount = 0;
    int chunkIndex = 0;
    Chunk* currChunk = nullptr; // current chunk
};

}

#endif
