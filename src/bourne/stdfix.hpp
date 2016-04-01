// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <sstream>
#include <string>

#pragma once

namespace stdfix
{
    template <typename T>
    std::string to_string(const T& n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }

    int64_t stol(const std::string& s);

    double stod(const std::string& s);
}
