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

#ifndef NGREST_NULLABLE_H
#define NGREST_NULLABLE_H

#include <memory>

namespace ngrest {

/**
 * class to support null value in JSON
 */
template <typename T>
class Nullable
{
public:
    /**
     * @brief default constructor
     */
    inline Nullable()
    {
    }

    /**
     * @brief initializes nullable with given smart pointer
     * @param value_ value
     */
    inline Nullable(const std::shared_ptr<T>& value_):
        value(value_)
    {
    }

    /**
     * @brief initializes nullable with given non-null value, performs private copy of value
     * @param value_ value
     */
    inline Nullable(const T& value_):
        value(std::shared_ptr<T>(new T(value_)))
    {
    }

    /**
     * @brief initializes nullable with given value, takes ovnership of value
     * @param value_ value
     */
    inline Nullable(T* value_):
        value(std::shared_ptr<T>(value_))
    {
    }

    /**
     * @brief share data between Nullable instances
     * @param other other Nullable to share
     */
    inline Nullable(const Nullable<T>& other):
        value(other.value)
    {
    }

    /**
     * @brief destructor
     */
    inline ~Nullable()
    {
    }

    /**
     * @brief initializes nullable with given smart pointer
     * @param value value
     */
    inline Nullable<T>& operator=(const std::shared_ptr<T>& value)
    {
        this->value = value;
        return *this;
    }

    /**
     * @brief initializes nullable with given non-null value, performs private copy of value
     * @param value value
     */
    inline Nullable<T>& operator=(const T& value)
    {
        get() = value;
        return *this;
    }

    /**
     * @brief initializes nullable with given value, takes ovnership of value
     * @param value value
     */
    inline Nullable<T>& operator=(T* value)
    {
        this->value = value;
        return *this;
    }

    /**
     * @brief share data between Nullable instances
     * @param other other Nullable to share
     */
    inline Nullable<T>& operator=(const Nullable<T>& other)
    {
        value = other.value;
        return *this;
    }

    /**
     * @brief tests if other's Nullable value equals to this Nullable stored value
     * @param other other Nullable to test
     * @return true if other Nullable value equals to stored value
     */
    inline bool operator==(const Nullable<T>& other) const
    {
        return ((value != nullptr) && (other.value != nullptr))
                ? (*value == *other.value)
                : ((value == nullptr) && (other.value == nullptr));
    }

    /**
     * @brief tests if other's Nullable value not equals to this Nullable stored value
     * @param other other Nullable to test
     * @return true if other Nullable value not equals to stored value
     */
    inline bool operator!=(const Nullable<T>& other) const
    {
        return !operator==(other);
    }

    /**
     * @brief tests if given value equals to stored value
     * @param value value to test
     * @return true if given value equals to stored value
     */
    inline bool operator==(const T& value) const
    {
        if (this->value == nullptr)
            return false;

        return *this->value == value;
    }

    /**
     * @brief tests if given value not equals to stored value
     * @param value value to test
     * @return true if given value not equals to stored value
     */
    inline bool operator!=(const T& value) const
    {
        return !operator==(value);
    }

    /**
     * @brief tests if given value equals to stored value
     * @param value value to test
     * @return true if given value equals to stored value
     */
    inline bool operator==(const T* value) const
    {
        return ((this->value != nullptr) && (value != nullptr))
                ? (*(this->value.get()) == *value)
                : ((this->value == nullptr) && (value == nullptr));
    }

    /**
     * @brief tests if given value not equals to stored value
     * @param value value to test
     * @return true if given value not equals to stored value
     */
    inline bool operator!=(const T* value) const
    {
        return !operator==(value);
    }

    /**
     * @brief get reference to stored value
     * may produce SIGSEGV if value is null
     */
    inline operator T&()
    {
        return *value.get();
    }

    /**
     * @brief get reference to stored value
     * may produce SIGSEGV if value is null
     */
    inline operator const T&() const
    {
        return *value.get();
    }

    /**
     * @brief value member access method
     * @return pointer to value
     */
    inline T* operator->()
    {
        return value.get();
    }

    /**
     * @brief value member access method
     * @return pointer to value
     */
    inline const T* operator->() const
    {
        return value.get();
    }

    /**
     * @brief get reference to stored value
     * may produce SIGSEGV if value is null
     */
    inline T& operator*()
    {
        return *value.get();
    }

    /**
     * @brief get reference to stored value
     * may produce SIGSEGV if value is null
     */
    inline const T& operator*() const
    {
        return *value.get();
    }

    /**
     * @brief sets value to null
     */
    inline void setNull()
    {
        value.reset();
    }

    /**
     * @brief tests if value is null
     * @return true if value is null
     */
    inline bool isNull() const
    {
        return !value;
    }

    /**
     * @brief tests if value is not null
     * @return true if value is not null
     */
    inline bool isValid() const
    {
        return !isNull();
    }

    /**
     * @brief returns existing value or instantiates the value with default constructor if value was null
     * @return reference to value
     */
    inline T& get()
    {
        if (value == nullptr)
            value = std::shared_ptr<T>(new T());

        return *value;
    }

    /**
     * @brief gets shared pointer to the value
     * @return shared pointer to the value
     */
    inline std::shared_ptr<T>& ptr()
    {
        return value;
    }

private:
    std::shared_ptr<T> value;
};

} // namespace ngrest

#endif // NGREST_NULLABLE_H
