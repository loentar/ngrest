#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/MemPool.h>
#include <ngrest/json/JsonTypes.h>
#include <ngrest/json/JsonReader.h>
#include <ngrest/json/JsonWriter.h>

int main()
{
    try {
        const int testsCount = 8;
        char test1[] = "{}";
        char test2[] = "[]";
        char test3[] = "{\"abc\": 1}";
        char test4[] = "{ \"q\": [ ] }";
        char test5[] = "{\"x\": {\"abc\": 1}}";
        char test6[] = "{  \"x\" : {\"abc\": 1 }, \"y\": [1, 2e2, \"3\", null , undefined, NaN]}";
        char test7[] = "[{}, {\"\": \"\"}]";
        char test8[] = "[[[],[]],{\"1\": []}]";
        char* tests[testsCount] = {
            test1,
            test2,
            test3,
            test4,
            test5,
            test6,
            test7,
            test8
        };

        for (int t = 0; t < testsCount; ++t) {
            ngrest::MemPool pool;
            ngrest::MemPool poolOut;
            ngrest::json::Node* root = ngrest::json::JsonReader::read(tests[t], pool);

            ngrest::json::JsonWriter::write(root, poolOut, 2);

            int chunksCount = poolOut.getChunkCount();
            const ngrest::MemPool::Chunk* chunks = poolOut.getChunks();
            printf("test #%d \n----------\n", t + 1);
            for (int i = 0; i < chunksCount; ++i) {
                const ngrest::MemPool::Chunk* chunk = chunks + i;
                fwrite(chunk->buffer, chunk->size, 1, stdout);
            }
            printf("\n----------\n");
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
