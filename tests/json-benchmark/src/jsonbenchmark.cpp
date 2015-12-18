#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

#include <json-c/json.h>

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/MemPool.h>
#include <ngrest/common/ObjectModel.h>
#include <ngrest/json/JsonReader.h>
#include <ngrest/json/JsonWriter.h>


inline unsigned long long getTime() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
}

void writeToFile(const char* fileName, const char* json, unsigned long size)
{
    int fd = ::open(fileName, O_WRONLY | O_CREAT, 0644);
    if (fd >= 0) {
        ::write(fd, json, size);
        ::close(fd);
    }
}

int main()
{
    try {
        uint64_t start;
        uint64_t mid;
        uint64_t end;

        ngrest::MemPool poolFile(NGREST_MEMPOOL_CHUNK_SIZE * 10);
        int fd = ::open("deploy/bin/test.json", O_RDONLY);
        struct stat st;
        ::fstat(fd, &st);
        __off_t remaining = st.st_size;
        if (fd >= 0) {
            while (remaining > 0) {
                int blockSize = (remaining > 4096) ? 4096 : remaining;
                ssize_t res = ::read(fd, poolFile.grow(blockSize), blockSize);
                if (res != blockSize) {
                    std::cerr << "failed to read" << std::endl;
                    return 1;
                }
                remaining -= res;
            }
            ::close(fd);
        }
        ngrest::MemPool::Chunk* chunk = poolFile.flatten();

        ////////////////////////////////////////////////////////////////////////

        start = getTime();
        struct json_tokener* tokener = json_tokener_new();

        json_object* json_obj = json_tokener_parse_ex(tokener, chunk->buffer, chunk->size);
        if (tokener->err != json_tokener_success)
        {
            std::cerr << "FAILED" << std::endl;
        }
        json_tokener_free(tokener);

        mid = getTime();

        const char* result = json_object_to_json_string_ext(json_obj, JSON_C_TO_STRING_PLAIN);

        end = getTime();

        std::cout << "JSON-C:   "
                  << "\tparse = " << (mid - start) << "; "
                  << "\twrite = " << (end - mid) << "; "
                  << "\tTOTAL = " << (end - start)
                  << std::endl;

        writeToFile("deploy/bin/out-json-c.json", result, strlen(result));


        ///////////////////////////////////////////////////////////////////////////////

        start = getTime();
        ngrest::MemPool poolJson;
        ngrest::Node* root = ngrest::json::JsonReader::read(chunk->buffer, poolJson);

        mid = getTime();

        ngrest::MemPool poolOut;
        ngrest::json::JsonWriter::write(root, poolOut);
        end = getTime();

        std::cout << "NGREST:   "
                  << "\tparse = " << (mid - start) << "; "
                  << "\twrite = " << (end - mid) << "; "
                  << "\tTOTAL = " << (end - start)
                  << std::endl;


        ngrest::MemPool::Chunk* outChunk = poolOut.flatten();
        writeToFile("deploy/bin/out-json-ngrest.json", outChunk->buffer, outChunk->size);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

//    JSON-C:   	parse = 163; 	write = 110; 	TOTAL = 273
//    NGREST:   	parse = 58; 	write = 28; 	TOTAL = 86

    return 0;
}
