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

#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/MemPool.h>
#include <ngrest/common/ObjectModel.h>
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
        char test6[] = "{  \"x\" : {\"abc\": 1 }, \"y\": [1, 2e2, \"3\", null , NaN]}";
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
            ngrest::Node* root = ngrest::json::JsonReader::read(tests[t], &pool);

            ngrest::json::JsonWriter::write(root, &poolOut, 2);

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
