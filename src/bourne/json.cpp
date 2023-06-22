// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include "json.hpp"

#include "class_type.hpp"
#include "detail/parser.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace bourne
{
inline namespace STEINWURF_BOURNE_VERSION
{
json::json() : m_internal(), m_type(class_type::null)
{
}

json::json(class_type type) : json()
{
    set_type(type);
}

json::json(std::initializer_list<json> list) : json()
{
    assert(list.size() % 2 == 0 && "Missing value for key value pair.");
    set_type(class_type::object);
    for (auto i = list.begin(); i != list.end(); i += 2)
    {
        operator[](i->to_string()) = *std::next(i);
    }
}

json::json(json&& other) : m_internal(other.m_internal), m_type(other.m_type)
{
    other.m_type = class_type::null;
    other.m_internal.m_map = nullptr;
}

json::json(std::nullptr_t) : m_internal(), m_type(class_type::null)
{
}

json::json(const json& other)
{
    clear();
    switch (other.m_type)
    {
    case class_type::object:
        m_internal.m_map = new json::object_type(
            other.m_internal.m_map->begin(), other.m_internal.m_map->end());
        break;
    case class_type::array:
        m_internal.m_array = new json::array_type(
            other.m_internal.m_array->begin(), other.m_internal.m_array->end());
        break;
    case class_type::string:
        m_internal.m_string = new std::string(*other.m_internal.m_string);
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
    if (this == &other)
        return *this;

    switch (other.m_type)
    {
    case class_type::object:
        m_internal.m_map = new json::object_type(
            other.m_internal.m_map->begin(), other.m_internal.m_map->end());
        break;
    case class_type::array:
        m_internal.m_array = new json::array_type(
            other.m_internal.m_array->begin(), other.m_internal.m_array->end());
        break;
    case class_type::string:
        m_internal.m_string = new std::string(*other.m_internal.m_string);
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

const json& json::operator[](const std::string& key) const
{
    assert(is_object());
    return m_internal.m_map->operator[](key);
}

json& json::operator[](std::size_t index)
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

const json& json::operator[](std::size_t index) const
{
    assert(is_array());
    assert(index < m_internal.m_array->size());
    return m_internal.m_array->operator[](index);
}

bool json::operator==(const json& other) const
{
    if (m_type != other.m_type)
    {
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
    assert(has_key(key));
    return m_internal.m_map->at(key);
}

const json& json::at(const std::string& key) const
{
    assert(is_object());
    assert(has_key(key));
    return m_internal.m_map->at(key);
}

json& json::at(std::size_t index)
{
    assert(is_array());
    assert(index < size());
    return m_internal.m_array->at(index);
}

const json& json::at(std::size_t index) const
{
    assert(is_array());
    assert(index < size());
    return m_internal.m_array->at(index);
}

bool json::has_key(const std::string& key) const
{
    assert(is_object());
    return m_internal.m_map->find(key) != m_internal.m_map->end();
}

std::vector<std::string> json::keys() const
{
    assert(is_object());
    std::vector<std::string> keys;

    std::transform(std::begin(*m_internal.m_map), std::end(*m_internal.m_map),
                   std::back_inserter(keys),
                   [](const std::pair<std::string, json>& pair)
                   { return pair.first; });
    return keys;
}

std::size_t json::size() const
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
    return m_type == class_type::floating || m_type == class_type::integral;
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
        return (double)m_internal.m_int;
    }
}

std::string json::to_string() const
{
    assert(is_string());
    const std::string& str = *m_internal.m_string;
    std::string output;
    for (std::size_t i = 0; i < str.length(); ++i)
    {
        switch (str[i])
        {
        case '\"':
            output += "\\\"";
            break;
        case '\\':
            output += "\\";
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

detail::json_wrapper<json::object_type> json::object_range()
{
    assert(is_object());
    return detail::json_wrapper<json::object_type>(m_internal.m_map);
}

detail::json_const_wrapper<json::object_type> json::object_range() const
{
    assert(is_object());
    return detail::json_const_wrapper<json::object_type>(m_internal.m_map);
}

detail::json_wrapper<json::array_type> json::array_range()
{
    assert(is_array());
    return detail::json_wrapper<json::array_type>(m_internal.m_array);
}

detail::json_const_wrapper<json::array_type> json::array_range() const
{
    assert(is_array());
    return detail::json_const_wrapper<json::array_type>(m_internal.m_array);
}

std::string json::dump(uint32_t depth, std::string tab) const
{
    std::string pad = "";

    for (uint32_t i = 0; i < depth; ++i, pad += tab)
        ;

    switch (m_type)
    {
    case class_type::null:
        return "null";
    case class_type::object:
    {
        std::stringstream s;
        s << "{\n";
        const char* sep = "";
        for (auto& p : *m_internal.m_map)
        {
            s << sep << pad << "\"" << p.first << "\" : ";
            s << p.second.dump(depth + 1, tab);
            sep = ",\n";
        }
        s << "\n" << pad.erase(0, tab.size()) << "}";
        return s.str();
    }
    case class_type::array:
    {
        std::stringstream s;
        s << "[";
        const char* sep = "";
        for (auto& p : *m_internal.m_array)
        {
            s << sep << p.dump(depth + 1, tab);
            sep = ", ";
        }
        s << "]";
        return s.str();
    }
    case class_type::string:
        return "\"" + to_string() + "\"";
    case class_type::floating:
        return std::to_string(m_internal.m_float);
    case class_type::integral:
        return std::to_string(m_internal.m_int);
    case class_type::boolean:
        return m_internal.m_bool ? "true" : "false";
    default:
        return "";
    }
}
std::string json::dump_min() const
{
    switch (m_type)
    {
    case class_type::object:
    {
        std::stringstream s;
        s << "{";
        const char* sep = "";
        for (auto& p : *m_internal.m_map)
        {
            s << sep << "\"" << p.first << "\":" << p.second.dump_min();
            sep = ",";
        }
        s << "}";
        return s.str();
    }
    case class_type::array:
    {
        std::stringstream s;
        s << "[";
        const char* sep = "";
        for (auto& p : *m_internal.m_array)
        {
            s << sep << p.dump_min();
            sep = ",";
        }
        s << "]";
        return s.str();
    }
    case class_type::null:
    case class_type::string:
    case class_type::floating:
    case class_type::integral:
    case class_type::boolean:
    default:
        return dump(0, "");
    }
}

json json::parse(const std::string& input, std::error_code& error)
{
    assert(!error);
    return detail::parser::parse(input, error);
}

json json::parse(const std::string& input)
{
    return detail::parser::parse(input);
}

json json::array()
{
    return json(class_type::array);
}

json json::null()
{
    return json(nullptr);
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
}
