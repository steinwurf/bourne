// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

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
