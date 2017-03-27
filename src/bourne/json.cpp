// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include "json.hpp"

#include "parser.hpp"
#include "class_type.hpp"
#include "stdfix.hpp"

#include <iostream>
#include <string>
#include <cassert>

namespace bourne
{
json::json() :
    m_internal(), m_type(class_type::null)
{}

json::json(class_type type) :
    json()
{
    set_type(type);
}

json::json(std::initializer_list<json> list) :
    json()
{
    assert(list.size() % 2 == 0 && "Missing value for key value pair.");
    set_type(class_type::object);
    for (auto i = list.begin(); i != list.end(); i += 2)
    {
        operator[](i->to_string()) = *std::next(i);
    }
}

json::json(json&& other) :
    m_internal(other.m_internal),
    m_type(other.m_type)
{
    other.m_type = class_type::null;
    other.m_internal.m_map = nullptr;
}

json::json(std::nullptr_t) :
    m_internal(),
    m_type(class_type::null)
{}

json::json(const json& other)
{
    clear();
    switch (other.m_type)
    {
    case class_type::object:
        m_internal.m_map =
            new json::object_type(
                other.m_internal.m_map->begin(),
                other.m_internal.m_map->end());
        break;
    case class_type::array:
        m_internal.m_array =
            new json::array_type(
                other.m_internal.m_array->begin(),
                other.m_internal.m_array->end());
        break;
    case class_type::string:
        m_internal.m_string =
            new std::string(*other.m_internal.m_string);
        break;
    default:
        m_internal = other.m_internal;
    }
    m_type = other.m_type;
}

json::~json()
{
    clear();
}

json& json::operator=(json&& other)
{
    clear();
    m_internal = other.m_internal;
    m_type = other.m_type;
    other.m_internal.m_map = nullptr;
    other.m_type = class_type::null;
    return *this;
}

json& json::operator=(const json& other)
{
    switch (other.m_type)
    {
    case class_type::object:
        m_internal.m_map =
            new json::object_type(
                other.m_internal.m_map->begin(),
                other.m_internal.m_map->end());
        break;
    case class_type::array:
        m_internal.m_array =
            new json::array_type(
                other.m_internal.m_array->begin(),
                other.m_internal.m_array->end());
        break;
    case class_type::string:
        m_internal.m_string =
            new std::string(*other.m_internal.m_string);
        break;
    default:
        m_internal = other.m_internal;
    }
    m_type = other.m_type;
    return *this;
}

json& json::operator[](const std::string& key)
{
    if (m_type == class_type::null)
        set_type(class_type::object);
    assert(is_object());
    return m_internal.m_map->operator[](key);
}

json& json::operator[](uint32_t index)
{
    if (m_type == class_type::null)
        set_type(class_type::array);
    assert(is_array());
    if (index >= m_internal.m_array->size())
    {
        m_internal.m_array->resize(index + 1);
    }
    return m_internal.m_array->operator[](index);
}

bool json::operator==(const json& other) const
{
    if (m_type != other.m_type)
    {
        std::cout << "not same type" << std::endl;
        return false;
    }

    switch (m_type)
    {
    case class_type::null:
        return true;
    case class_type::object:
    {
        auto this_map = m_internal.m_map;
        auto other_map = other.m_internal.m_map;
        return this_map->size() == other_map->size() &&
               std::equal(this_map->begin(), this_map->end(),
                          other_map->begin());
    }
    case class_type::array:
    {
        auto this_array = m_internal.m_array;
        auto other_array = other.m_internal.m_array;
        return this_array->size() == other_array->size() &&
               std::equal(this_array->begin(), this_array->end(),
                          other_array->begin());
    }
    case class_type::string:
        return (*m_internal.m_string) == (*other.m_internal.m_string);
    case class_type::floating:
        return m_internal.m_float == other.m_internal.m_float;
    case class_type::integral:
        return m_internal.m_int == other.m_internal.m_int;
    case class_type::boolean:
        return m_internal.m_bool == other.m_internal.m_bool;
    }

    return true;
}

json& json::operator=(std::nullptr_t)
{
    set_type(class_type::null);
    return *this;
}

bool json::operator!=(const json& other) const
{
    return !(*this == other);
}

json& json::at(const std::string& key)
{
    assert(is_object());
    return m_internal.m_map->at(key);
}

const json& json::at(const std::string& key) const
{
    assert(is_object());
    return m_internal.m_map->at(key);
}

json& json::at(uint32_t index)
{
    assert(is_array());
    return m_internal.m_array->at(index);
}

const json& json::at(uint32_t index) const
{
    assert(is_array());
    return m_internal.m_array->at(index);
}

bool json::has_key(const std::string& key) const
{
    assert(is_object());
    return m_internal.m_map->find(key) != m_internal.m_map->end();
}

uint32_t json::size() const
{
    assert(is_array() || is_object());
    if (is_object())
        return m_internal.m_map->size();
    if (is_array())
        return m_internal.m_array->size();

    return 0;
}

class_type json::json_type() const
{
    return m_type;
}

bool json::is_null() const
{
    return m_type == class_type::null;
}

bool json::is_bool() const
{
    return m_type == class_type::boolean;
}

bool json::is_int() const
{
    return m_type == class_type::integral;
}

bool json::is_float() const
{
    return m_type == class_type::floating ||
           m_type == class_type::integral;
}

bool json::is_string() const
{
    return m_type == class_type::string;
}

bool json::is_object() const
{
    return m_type == class_type::object;
}

bool json::is_array() const
{
    return m_type == class_type::array;
}

bool json::to_bool() const
{
    assert(is_bool());
    return m_internal.m_bool;
}

int64_t json::to_int() const
{
    assert(is_int());
    return m_internal.m_int;
}

double json::to_float() const
{
    assert(is_float());
    if (m_type == class_type::floating)
    {
        return m_internal.m_float;
    }
    else
    {
        assert(m_type == class_type::integral);
        return (double) m_internal.m_int;
    }
}

std::string json::to_string() const
{
    assert(is_string());
    const std::string& str = *m_internal.m_string;
    std::string output;
    for (uint32_t i = 0; i < str.length(); ++i)
    {
        switch (str[i])
        {
        case '\"':
            output += "\\\"";
            break;
        case '\\':
            output += "\\\\";
            break;
        case '\b':
            output += "\\b";
            break;
        case '\f':
            output += "\\f";
            break;
        case '\n':
            output += "\\n";
            break;
        case '\r':
            output += "\\r";
            break;
        case '\t':
            output += "\\t";
            break;
        default:
            output += str[i];
            break;
        }
    }

    return output;
}

json_wrapper<json::object_type> json::object_range()
{
    assert(is_object());
    return json_wrapper<json::object_type>(m_internal.m_map);
}

json_const_wrapper<json::object_type> json::object_range() const
{
    assert(is_object());
    return json_const_wrapper<json::object_type>(m_internal.m_map);
}

json_wrapper<json::array_type> json::array_range()
{
    assert(is_array());
    return json_wrapper<json::array_type>(m_internal.m_array);
}

json_const_wrapper<json::array_type> json::array_range() const
{
    assert(is_array());
    return json_const_wrapper<json::array_type>(m_internal.m_array);
}

std::string json::dump(uint32_t depth, std::string tab) const
{
    std::string pad = "";

    for (uint32_t i = 0; i < depth; ++i, pad += tab);

    switch (m_type)
    {
    case class_type::null:
        return "null";
    case class_type::object:
    {
        std::string s = "{\n";
        bool skip = true;
        for (auto& p : *m_internal.m_map)
        {
            if (!skip)
            {
                s += ",\n";
            }
            s += pad;
            s += "\"" + p.first + "\" : ";
            s += p.second.dump(depth + 1, tab);
            skip = false;
        }
        s += "\n" + pad.erase(0, 2) + "}";
        return s;
    }
    case class_type::array:
    {
        std::string s = "[";
        bool skip = true;
        for (auto& p : *m_internal.m_array)
        {
            if (!skip)
            {
                s += ", ";
            }
            s += p.dump(depth + 1, tab);
            skip = false;
        }
        s += "]";
        return s;
    }
    case class_type::string:
        return "\"" + to_string() + "\"";
    case class_type::floating:
        return stdfix::to_string(m_internal.m_float);
    case class_type::integral:
        return stdfix::to_string(m_internal.m_int);
    case class_type::boolean:
        return m_internal.m_bool ? "true" : "false";
    default:
        return "";
    }
}

json json::parse(const std::string& string)
{
    auto p = parser(string);
    return p.parse();
}

json json::array()
{
    return json(class_type::array);
}

json json::object()
{
    return json(class_type::object);
}


json json::object(std::initializer_list<json> list)
{
    return json(list);
}

void json::clear()
{
    switch (m_type)
    {
    case class_type::array:
        delete m_internal.m_array;
        break;
    case class_type::object:
        delete m_internal.m_map;
        break;
    case class_type::string:
        delete m_internal.m_string;
        break;
    default:;
    }
    m_type = class_type::null;
}

void json::set_type(class_type type)
{
    clear();
    switch (type)
    {
    case class_type::null:
        m_internal.m_map = nullptr;
        break;
    case class_type::object:
        m_internal.m_map = new json::object_type();
        break;
    case class_type::array:
        m_internal.m_array = new json::array_type();
        break;
    case class_type::string:
        m_internal.m_string = new std::string();
        break;
    case class_type::floating:
        m_internal.m_float = 0.0;
        break;
    case class_type::integral:
        m_internal.m_int = 0;
        break;
    case class_type::boolean:
        m_internal.m_bool = false;
        break;
    }

    m_type = type;
}

std::ostream& operator<<(std::ostream& os, const json& json)
{
    os << json.dump();
    return os;
}
}
