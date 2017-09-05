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

#include <core/utils/tostring.h>
#include <core/utils/fromstring.h>
#include "Value.h"

namespace ngrest {
namespace xml {

Value::Value()
{
}

Value::Value(const Value& other):
    value(other.value)
{
}

Value::Value(const std::string& value_):
    value(value_)
{
}

Value::Value(const char* value_):
    value(value_ ? value_ : "")
{
}

Value::Value(float value_):
    value(toString(value_))
{
}

Value::Value(double value_):
    value(toString(value_))
{
}

Value::Value(byte value_):
    value(toString(value_))
{
}

Value::Value(int value_):
    value(toString(value_))
{
}

Value::Value(short value_):
    value(toString(value_))
{
}

Value::Value(long value_):
    value(toString(value_))
{
}

Value::Value(long long value_):
    value(toString(value_))
{
}

Value::Value(unsignedByte value_):
    value(toString(value_))
{
}

Value::Value(unsigned int value_):
    value(toString(value_))
{
}

Value::Value(unsigned short value_):
    value(toString(value_))
{
}

Value::Value(unsigned long value_):
    value(toString(value_))
{
}

Value::Value(unsigned long long value_):
    value(toString(value_))
{
}

Value::Value(bool value_):
    value(value_ ? "true" : "false")
{
}


Value& Value::operator=(const Value& other)
{
    this->value = other.value;
    return *this;
}


Value& Value::operator=(const std::string& value)
{
    this->value = value;
    return *this;
}

Value& Value::operator=(const char* value)
{
    this->value = value ? value : "";
    return *this;
}

Value& Value::operator=(float value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(double value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(byte value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(int value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(short value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(long value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(long long value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(unsignedByte value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(unsigned int value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(unsigned short value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(unsigned long value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(unsigned long long value)
{
    toString(value, this->value);
    return *this;
}

Value& Value::operator=(bool value)
{
    this->value = value ? "true" : "false";
    return *this;
}


Value::operator const std::string&() const
{
    return value;
}

Value::operator float() const
{
    float result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator double() const
{
    double result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator byte() const
{
    byte result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator int() const
{
    int result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator short() const
{
    short result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator long() const
{
    long result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator long long() const
{
    long long result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator unsignedByte() const
{
    unsignedByte result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator unsigned int() const
{
    unsigned int result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator unsigned short() const
{
    unsigned short result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator unsigned long() const
{
    unsigned long result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator unsigned long long() const
{
    unsigned long long result = 0;
    fromString(this->value, result);
    return result;
}

Value::operator bool() const
{
    return value == "true" || value == "1";
}


Value::operator std::string&()
{
    return value;
}

const std::string& Value::asString() const
{
    return value;
}

std::string& Value::asString()
{
    return value;
}

bool Value::operator==(const Value& value) const
{
    return this->value == value.value;
}


}
}
