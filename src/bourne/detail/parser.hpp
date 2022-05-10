// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include "../error.hpp"
#include "../json.hpp"

#include <string>
#include <system_error>

namespace bourne
{
inline namespace STEINWURF_BOURNE_VERSION
{
namespace detail
{
class parser
{

public:
    static json parse(const std::string& input);
    static json parse(const std::string& input, std::error_code& error);
    static json parse(std::istream& input, std::error_code& error);
    static json parse(std::istream& input);

private:
    static void consume_white_space(std::istream& input, size_t& offest);
    static json parse_object(std::istream& input, size_t& offest,
                             std::error_code& error);
    static json parse_array(std::istream& input, size_t& offest,
                            std::error_code& error);
    static json parse_string(std::istream& input, size_t& offest,
                             std::error_code& error);
    static json parse_number(std::istream& input, size_t& offest,
                             std::error_code& error);
    static json parse_bool(std::istream& input, size_t& offest,
                           std::error_code& error);
    static json parse_null(std::istream& input, size_t& offest,
                           std::error_code& error);
    static json parse_next(std::istream& input, size_t& offest,
                           std::error_code& error);
};
}
}
}
