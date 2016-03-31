// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

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
