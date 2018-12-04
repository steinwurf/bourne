// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include "../json.hpp"

#include <string>
#include <map>
#include <deque>
#include <cstdint>

namespace bourne
{
// handle circular dependency
class json;

namespace detail
{

union backing_data
{
    using object_type = std::map<std::string, json>;
    using array_type = std::deque<json>;

    backing_data(double d) :
        m_float(d)
    {}
    backing_data(int64_t l) :
        m_int(l)
    {}
    backing_data(bool b) :
        m_bool(b)
    {}
    backing_data(std::string s) :
        m_string(new std::string(s))
    {}
    backing_data() :
        m_int(0)
    {}

    array_type* m_array;
    object_type* m_map;
    std::string* m_string;
    double m_float;
    int64_t m_int;
    bool m_bool;
};
}
}
