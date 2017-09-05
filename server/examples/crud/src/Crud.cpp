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

#include <unordered_map>

#include <ngrest/common/HttpException.h>

#include "Crud.h"

namespace ngrest {
namespace examples {

// here should be database connector: replace to real database connection
class Db
{
public:
    static Db& inst()
    {
        static Db instance;
        return instance;
    }

    std::vector<int> getIds()
    {
        // "select id from storage;"
        std::vector<int> res;
        for (const auto& item : storage)
            res.push_back(item.first);
        return res;
    }

    void create(int id, const std::string& data)
    {
        // "insert into storage(id, data) values (:id, :data);"
        auto it = storage.find(id);
        NGREST_ASSERT_HTTP(it == storage.end(), HTTP_STATUS_409_CONFLICT, "Item already exist");
        storage.insert({id, data});
    }

    std::string get(int id)
    {
        // "select data from storage where id = :id;"
        auto it = storage.find(id);
        NGREST_ASSERT_HTTP(it != storage.end(), HTTP_STATUS_404_NOT_FOUND, "Item not found");
        return it->second;
    }

    void update(int id, const std::string& data)
    {
        // "update storage set data = :data where id = :id;"
        auto it = storage.find(id);
        NGREST_ASSERT_HTTP(it != storage.end(), HTTP_STATUS_404_NOT_FOUND, "Item not found");
        it->second = data;
    }

    void del(int id)
    {
        // "delete from storage where id = :id;"
        auto it = storage.find(id);
        NGREST_ASSERT_HTTP(it != storage.end(), HTTP_STATUS_404_NOT_FOUND, "Item not found");
        storage.erase(it);
    }

private:
    std::unordered_map<int, std::string> storage;
};


std::vector<int> Crud::getIds()
{
    return Db::inst().getIds();
}

void Crud::create(int id, const std::string& data)
{
    Db::inst().create(id, data);
}

std::string Crud::read(int id) const
{
    return Db::inst().get(id);
}

void Crud::update(int id, const std::string& data)
{
    Db::inst().update(id, data);
}

void Crud::del(int id)
{
    Db::inst().del(id);
}

}
}
