// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

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
