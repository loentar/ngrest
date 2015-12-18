#ifndef NGREST_MEMPOOL_H
#define NGREST_MEMPOOL_H

#include <stdint.h>
#include <new>

namespace ngrest {

#define NGREST_MEMPOOL_CHUNK_SIZE 4096

/* Pool to store large amount of small POCOs and joined strings */
class MemPool
{
public:
    struct Chunk
    {
        char* buffer;
        uint64_t bufferSize;
        uint64_t size;
    };

public:
    MemPool(uint64_t chunkSize = NGREST_MEMPOOL_CHUNK_SIZE);
    ~MemPool();

    template <typename T> inline T* alloc()
    {
        return new (grow(sizeof(T))) T();
    }

    template <typename T, typename P1> inline T* alloc(P1 p1)
    {
        return new (grow(sizeof(T))) T(p1);
    }

    template <typename T, typename P1, typename P2> inline T* alloc(P1 p1, P2 p2)
    {
        return new (grow(sizeof(T))) T(p1, p2);
    }

    template <typename T, typename P1, typename P2, typename P3> inline T* alloc(P1 p1, P2 p2, P3 p3)
    {
        return new (grow(sizeof(T))) T(p1, p2, p3);
    }

    template <typename T> inline T* allocNoCtor()
    {
        return reinterpret_cast<T*>(grow(sizeof(T)));
    }

    void reset();
    void free();

    bool isClean() const;

    inline void putChar(const char ch)
    {
        *grow(sizeof(char)) = ch;
    }

    char* putCString(const char* string, bool terminate = false);
    char* putCString(const char* string, uint64_t size, bool terminate = false);
    char* putData(const char* data, uint64_t size);

    inline Chunk const* getChunks() const
    {
        return chunks;
    }

    inline int getChunkCount() const
    {
        return chunksCount;
    }

    inline Chunk const* getLastChunk() const
    {
        return currChunk;
    }

    inline char* grow(uint64_t growSize)
    {
        uint64_t offset;
        if (currChunk == nullptr || (currChunk->size + growSize) > currChunk->bufferSize) {
            offset = 0;
            newChunk((growSize > chunkSize) ? growSize : NGREST_MEMPOOL_CHUNK_SIZE);
        } else {
            offset = currChunk->size;
        }

        currChunk->size += growSize;
        return currChunk->buffer + offset;
    }

    inline bool shrinkLastChunk(uint64_t shrinkSize)
    {
        if (currChunk == nullptr || currChunk->size < shrinkSize)
            return false;

        currChunk->size -= shrinkSize;
        return true;
    }

    inline uint64_t getSize() const
    {
        uint64_t result = 0;
        const MemPool::Chunk* chunk = chunks;
        for (int i = 0; i < chunksCount; ++i, ++chunk)
            result += chunk->size;
        return result;
    }

    Chunk* flatten();

    void reserve(uint64_t size);

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
