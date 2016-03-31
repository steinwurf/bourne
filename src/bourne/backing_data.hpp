// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#pragma once

#include "json.hpp"

#include <string>
#include <map>
#include <deque>

namespace bourne
{
    // handle circular dependency
    class json;

    union backing_data
    {
        using object_type = std::map<std::string, json>;
        using array_type = std::deque<json>;

        backing_data(double d):
            m_float(d)
        {}
        backing_data(long l):
            m_int(l)
        {}
        backing_data(bool b):
            m_bool(b)
        {}
        backing_data(std::string s):
            m_string(new std::string(s))
        {}
        backing_data():
            m_int(0)
        {}

        array_type* m_array;
        object_type* m_map;
        std::string* m_string;
        double m_float;
        long m_int;
        bool m_bool;
    };
}
