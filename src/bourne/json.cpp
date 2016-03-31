// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include "json.hpp"

#include "parser.hpp"
#include "class_type.hpp"

#include <iostream>
#include <string>

namespace bourne
{

    json::json() : m_internal(), m_type(class_type::null)
    {}

    json::json(class_type type):
        json()
    {
        set_type(type);
    }

    json::json(std::initializer_list<json> list):
        json()
    {
        set_type(class_type::object);
        for (auto i = list.begin(), e = list.end(); i != e; ++i, ++i)
        {
            operator[](i->to_string()) = *std::next(i);
        }
    }

    json::json(json&& other):
        m_internal(other.m_internal),
        m_type(other.m_type)
    {
        other.m_type = class_type::null;
        other.m_internal.m_map = nullptr;
    }

    json::json(std::nullptr_t):
        m_internal(),
        m_type(class_type::null)
    {}

    json::json(const json &other)
    {
        switch(other.m_type)
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
            m_internal.m_string = new std::string(
                *other.m_internal.m_string);
            break;
        default:
            m_internal = other.m_internal;
        }
        m_type = other.m_type;
    }

    json::~json()
    {
        switch(m_type)
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
    }

    json& json::operator=(json&& other)
    {
        m_internal = other.m_internal;
        m_type = other.m_type;
        other.m_internal.m_map = nullptr;
        other.m_type = class_type::null;
        return *this;
    }

    json& json::operator=(const json &other)
    {
        switch(other.m_type)
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
            m_internal.m_string = new std::string(
                *other.m_internal.m_string);
            break;
        default:
            m_internal = other.m_internal;
        }
        m_type = other.m_type;
        return *this;
    }

    json& json::operator[](const std::string &key)
    {
        set_type(class_type::object);
        return m_internal.m_map->operator[](key);
    }

    json& json::operator[](uint32_t index)
    {
        set_type(class_type::array);
        if (index >= m_internal.m_array->size())
        {
            m_internal.m_array->resize(index + 1);
        }
        return m_internal.m_array->operator[](index);
    }

    json& json::at(const std::string &key)
    {
        return operator[](key);
    }

    const json& json::at(const std::string &key) const
    {
        return m_internal.m_map->at(key);
    }

    json& json::at(uint32_t index)
    {
        return operator[](index);
    }

    const json& json::at(uint32_t index) const
    {
        return m_internal.m_array->at(index);
    }

    int json::length() const
    {
        if (m_type == class_type::array)
            return m_internal.m_array->size();
        else
            return -1;
    }

    bool json::has_key(const std::string &key) const
    {
        if (m_type == class_type::object)
            return m_internal.m_map->find(key) != m_internal.m_map->end();
        return false;
    }

    int json::size() const
    {
        if (m_type == class_type::object)
            return m_internal.m_map->size();
        else if (m_type == class_type::array)
            return m_internal.m_array->size();
        else
            return -1;
    }

    class_type json::json_type() const
    {
        return m_type;
    }

    /// Functions for getting primitives from the json object.
    bool json::is_null() const
    {
        return m_type == class_type::null;
    }

    long json::to_int() const
    {
        bool b;
        return to_int(b);
    }

    bool json::to_bool() const
    {
        bool b;
        return to_bool(b);
    }

    bool json::to_bool(bool &ok) const
    {
        ok = (m_type == class_type::boolean);
        return ok ? m_internal.m_bool : false;
    }

    long json::to_int(bool &ok) const
    {
        ok = (m_type == class_type::integral);
        return ok ? m_internal.m_int : 0;
    }

    double json::to_float() const
    {
        bool b;
        return to_float(b);
    }

    double json::to_float(bool &ok) const
    {
        ok = (m_type == class_type::floating);
        return ok ? m_internal.m_float : 0.0;
    }

    std::string json::to_string() const
    {
        bool b;
        return std::move(to_string(b));
    }

    std::string json::to_string(bool &ok) const
    {
        ok = (m_type == class_type::string);
        if (ok)
        {
            const std::string& str = *m_internal.m_string;
            std::string output;
            for (uint32_t i = 0; i < str.length(); ++i)
            {
                switch(str[i])
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

            return std::move(output);
        }
        else
        {
            return std::string("");
        }
    }

    json_wrapper<json::object_type> json::object_range()
    {
        if (m_type != class_type::object)
        {
            return json_wrapper<json::object_type>(nullptr);
        }
        return json_wrapper<json::object_type>(m_internal.m_map);
    }

    json_const_wrapper<json::object_type> json::object_range() const
    {
        if (m_type != class_type::object)
        {
            return json_const_wrapper<json::object_type>(nullptr);
        }
        return json_const_wrapper<json::object_type>(m_internal.m_map);
    }

    json_wrapper<json::array_type> json::array_range()
    {
        if (m_type != class_type::array)
        {
            return json_wrapper<json::array_type>(nullptr);
        }
        return json_wrapper<json::array_type>(m_internal.m_array);
    }

    json_const_wrapper<json::array_type> json::array_range() const
    {
        if (m_type != class_type::array)
        {
            return json_const_wrapper<json::array_type>(nullptr);
        }
        return json_const_wrapper<json::array_type>(m_internal.m_array);
    }

    std::string json::dump(int depth, std::string tab) const
    {
        std::string pad = "";

        for (int i = 0; i < depth; ++i, pad += tab);

        switch(m_type)
        {
        case class_type::null:
            return "null";
        case class_type::object:
            {
                std::string s = "{\n";
                bool skip = true;
                for (auto &p : *m_internal.m_map) {
                    if (!skip) s += ",\n";
                    s += (pad + "\"" + p.first + "\" : " + p.second.dump(depth + 1, tab));
                    skip = false;
                }
                s += ("\n" + pad.erase(0, 2) + "}") ;
                return s;
            }
        case class_type::array:
            {
                std::string s = "[";
                bool skip = true;
                for (auto &p : *m_internal.m_array)
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
            return std::to_string(m_internal.m_float);
        case class_type::integral:
            return std::to_string(m_internal.m_int);
        case class_type::boolean:
            return m_internal.m_bool ? "true" : "false";
        default:
            return "";
        }
    }

    json json::parse(const std::string& string)
    {
        auto p = parser(string);
        return std::move(p.parse());
    }

    json json::array()
    {
        return std::move(json(class_type::array));
    }

    json json::object()
    {
        return std::move(json(class_type::object));
    }


    json json::object(std::initializer_list<json> list)
    {
        return std::move(json(list));
    }

    void json::set_type(class_type type)
    {
        if (type == m_type)
            return;

        switch(m_type)
        {
        case class_type::object:
            delete m_internal.m_map;
            break;
        case class_type::array:
            delete m_internal.m_array;
            break;
        case class_type::string:
            delete m_internal.m_string;
            break;
        default:;
        }

        switch(type)
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

    std::ostream& operator<<(std::ostream &os, const json &json)
    {
        os << json.dump();
        return os;
    }
}
