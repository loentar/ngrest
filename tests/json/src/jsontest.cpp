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
        char test3[] = "{\"ab c\": 1}";
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

        const char* testsOut[testsCount] = {
            "{}",
            "[]",
            "{\"ab c\":1}",
            "{\"q\":[]}",
            "{\"x\":{\"abc\":1}}",
            "{\"x\":{\"abc\":1},\"y\":[1,2e2,\"3\",null,NaN]}",
            "[{},{\"\":\"\"}]",
            "[[[],[]],{\"1\":[]}]"
        };

        for (int t = 0; t < testsCount; ++t) {
            std::cout << "Basic test: #" << t << std::endl;
            ngrest::MemPool pool;
            ngrest::MemPool poolOut;
            ngrest::Node* root = ngrest::json::JsonReader::read(tests[t], &pool);

            ngrest::json::JsonWriter::write(root, &poolOut, 2);

            const ngrest::MemPool::Chunk* chunk = poolOut.flatten();
            NGREST_ASSERT(!strcmp(chunk->buffer, testsOut[t]), std::string("Basic Test failed. Expected [")
                          + testsOut[t] + "] found [" + chunk->buffer + "].")
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }



    // quoting test
    try {
        const char* json = R"(["no special chars","\b\f\n\r\t\"\\\/\u0008\u000a","__\n\r__","\t__","__\t"])";
        const char* values[] = {
            "no special chars", "\b\f\n\r\t\"\\/\x08\x0a", "__\n\r__", "\t__", "__\t"
        };
        const char* jsonOut = R"(["no special chars","\b\f\n\r\t\"\\/\b\n","__\n\r__","\t__","__\t"])";
        ngrest::MemPool poolIn;

        // test reader
        char* jsonIn = poolIn.putCString(json, true);
        const ngrest::Node* root = ngrest::json::JsonReader::read(jsonIn, &poolIn);
        NGREST_ASSERT(root->type == ngrest::NodeType::Array, "Read node is not array");
        const ngrest::Array* arr = static_cast<const ngrest::Array*>(root);
        int index = 0;
        for (const ngrest::LinkedNode* node = arr->firstChild; node; node = node->nextSibling, ++index) {
            std::cout << "Reader test: #" << index << std::endl;
            NGREST_ASSERT_NULL(node->node);
            NGREST_ASSERT(node->node->type == ngrest::NodeType::Value, "Child is not Value");
            const ngrest::Value* value = static_cast<const ngrest::Value*>(node->node);
            NGREST_ASSERT_NULL(value->value);
            NGREST_ASSERT(!strcmp(value->value, values[index]), std::string("Reader Test failed. Expected [")
                          + values[index] + "] found [" + value->value + "].")
        }


        std::cout << "Writer test" << std::endl;
        // test writer
        ngrest::MemPool poolOut;
        ngrest::json::JsonWriter::write(root, &poolOut);
        ngrest::MemPool::Chunk* res = poolOut.flatten();
        NGREST_ASSERT_NULL(res);
        NGREST_ASSERT(!strcmp(res->buffer, jsonOut), std::string("Writer Test failed. Expected:\n===================\n")
                      + jsonOut + "\n===================\nfound:\n===================\n"
                      + res->buffer + "\n===================\n")
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::cout << "All json tests passed" << std::endl;

    return 0;
}
