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

    std::vector<User> getIds()
    {
        // "select * from storage;"
        std::vector<User> res;
        for (const auto& item : storage) {
            res.push_back(item.second);
        }
        return res;
    }

    int create(User user)
    {
        // "insert into storage(id, user) values (:id, :user);"
        user.id = nextUserId;
        auto it = storage.find(nextUserId);
        NGREST_ASSERT_HTTP(it == storage.end(), HTTP_STATUS_409_CONFLICT, "Item already exist");
        storage.insert({nextUserId, user});

        ++nextUserId;
        return user.id;
    }

    User get(int id)
    {
        // "select user from storage where id = :id;"
        auto it = storage.find(id);
        NGREST_ASSERT_HTTP(it != storage.end(), HTTP_STATUS_404_NOT_FOUND, "Item not found");
        return it->second;
    }

    void update(int id, User user)
    {
        // "update storage set user = :user where id = :id;"
        auto it = storage.find(id);
        NGREST_ASSERT_HTTP(it != storage.end(), HTTP_STATUS_404_NOT_FOUND, "Item not found");
        user.id = id;
        it->second = user;
    }

    void patch(int id, const UserPatch& user)
    {
        // "update storage set user = :user where id = :id;"
        auto it = storage.find(id);
        NGREST_ASSERT_HTTP(it != storage.end(), HTTP_STATUS_404_NOT_FOUND, "Item not found");
        if (user.name.isValid()) {
            it->second.name = *user.name;
        }
        if (user.email.isValid()) {
            it->second.email = *user.email;
        }
    }

    void del(int id)
    {
        // "delete from storage where id = :id;"
        auto it = storage.find(id);
        NGREST_ASSERT_HTTP(it != storage.end(), HTTP_STATUS_404_NOT_FOUND, "Item not found");
        storage.erase(it);
    }

private:
    std::unordered_map<int, User> storage;
    int nextUserId = 1;
};


std::vector<User> Crud::getAll()
{
    return Db::inst().getIds();
}

int Crud::create(const User& user)
{
    return Db::inst().create(user);
}

User Crud::get(int id) const
{
    return Db::inst().get(id);
}

void Crud::update(int id, const User &user)
{
    Db::inst().update(id, user);
}

void Crud::patch(int id, const UserPatch &user)
{
    Db::inst().patch(id, user);
}

void Crud::del(int id)
{
    Db::inst().del(id);
}

}
}
