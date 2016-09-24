// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include "json.hpp"

#include <string>

namespace bourne
{
class parser
{

public:

    parser(const std::string& string);
    json parse();

private:

    void consume_white_space();
    json parse_object();
    json parse_array();
    json parse_string();
    json parse_number();
    json parse_bool();
    json parse_null();
    json parse_next();

private:

    std::string m_string;
    size_t m_offset;
};
}
