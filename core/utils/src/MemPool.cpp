#include <stdlib.h>
#include <memory.h>

#include "MemPool.h"

#define NGREST_MEMPOOL_CHUNK_RESERVE 64

namespace ngrest {

MemPool::MemPool(uint64_t chunkSize_):
    chunkSize(chunkSize_)
{
}

MemPool::~MemPool()
{
    free();
}

void MemPool::reset()
{
    if (chunks) {
        currChunk = chunks;
        currChunk->size = 0;
        chunkIndex = 0;
    }
}

void MemPool::free()
{
    for (int i = 0; i < chunksCount; ++i)
        ::free(chunks[i].buffer);
    ::free(chunks);
    chunksCount = 0;
    chunksReserved = 0;
    chunks = nullptr;
    currChunk = nullptr;
}

bool MemPool::isClean() const
{
    return chunks == nullptr || (currChunk == chunks && currChunk->size == 0);
}

char* MemPool::putCString(const char* string, bool terminate)
{
    return putData(string, strlen(string) + (terminate ? 1 : 0));
}

char* MemPool::putCString(const char* string, uint64_t size, bool terminate)
{
    char* res = putData(string, size);
    if (terminate)
        putChar('\0');
    return res;
}

char* MemPool::putData(const char* data, uint64_t size)
{
    return reinterpret_cast<char*>(memcpy(grow(size), data, size));
}

MemPool::Chunk* MemPool::flatten(bool terminate)
{
    if (!chunksCount)
        return nullptr;

    uint64_t newSize = getSize();
    uint64_t newBufferSize = newSize + (terminate ? 1 : 0);  // +1 - string terminator
    if (newBufferSize > chunks->bufferSize) {

        char* newBuffer = reinterpret_cast<char*>(realloc(chunks->buffer, newBufferSize));
        if (newBuffer == nullptr)
            throw std::bad_alloc();
        char* pos = newBuffer + chunks->size;

        Chunk* curr = chunks + 1;
        for (int i = 1; i < chunksCount; ++i, pos += curr->size, ++curr) {
            memcpy(pos, curr->buffer, curr->size);
            ::free(curr->buffer);
        }
        chunksCount = 1;
        currChunk = chunks;
        chunks->buffer = newBuffer;
        chunks->size = newSize;
        chunks->bufferSize = newBufferSize;
    }
    if (terminate)
        chunks->buffer[newSize] = '\0'; // terminate with \0 for C strings

    return chunks;
}

void MemPool::reserve(uint64_t size)
{
    if (!chunksCount)
        return;

    if (size < currChunk->bufferSize)
        return;

    char* newBuffer = reinterpret_cast<char*>(realloc(currChunk->buffer, size));
    if (newBuffer == nullptr)
        throw std::bad_alloc();
    currChunk->buffer = newBuffer;
    currChunk->bufferSize = size;
}


void MemPool::newChunk(uint64_t size)
{
    if (chunkIndex < chunksCount) {
        Chunk* chunk = chunks + chunkIndex;
        if (chunk->bufferSize < size) {
            char* newBuffer = reinterpret_cast<char*>(realloc(chunk->buffer, size));
            if (!newBuffer)
                throw std::bad_alloc();

            chunk->buffer = newBuffer;
            chunk->bufferSize = size;
        }
        chunk->size = 0;
        currChunk = chunk;
        return;
    }

    char* buffer = reinterpret_cast<char*>(malloc(size));
    if (buffer == nullptr)
        throw std::bad_alloc();

    if (chunksReserved == chunksCount) {
        const int newChunksReserved = chunksCount + NGREST_MEMPOOL_CHUNK_RESERVE;
        Chunk* newChunks = reinterpret_cast<Chunk*>(realloc(chunks, sizeof(Chunk) * newChunksReserved));
        if (!newChunks) {
            ::free(buffer);
            throw std::bad_alloc();
        }

        chunks = newChunks;
        chunksReserved = newChunksReserved;
    }

    currChunk = chunks + chunksCount;
    currChunk->buffer = buffer;
    currChunk->size = 0;
    currChunk->bufferSize = chunkSize;
    ++chunksCount;
    ++chunkIndex;
}

}
