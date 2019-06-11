// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

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

    static json parse(const std::string& input, std::error_code& error);

private:

    static void consume_white_space(
        const std::string& input, size_t& offest, std::error_code& error);
    static json parse_object(
        const std::string& input, size_t& offest, std::error_code& error);
    static json parse_array(
        const std::string& input, size_t& offest, std::error_code& error);
    static json parse_string(
        const std::string& input, size_t& offest, std::error_code& error);
    static json parse_number(
        const std::string& input, size_t& offest, std::error_code& error);
    static json parse_bool(
        const std::string& input, size_t& offest, std::error_code& error);
    static json parse_null(
        const std::string& input, size_t& offest, std::error_code& error);
    static json parse_next(
        const std::string& input, size_t& offest, std::error_code& error);
};
}
}
}
