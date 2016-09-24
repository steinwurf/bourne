// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <sstream>
#include <string>

namespace stdfix
{
int64_t stol(const std::string& s)
{
    std::istringstream str(s);
    int64_t i;
    str >> i;
    return i;
}

double stod(const std::string& s)
{
    std::istringstream str(s);
    double d;
    str >> d;
    return d;
}
}
