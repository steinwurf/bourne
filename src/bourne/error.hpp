// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>
#include <string>
#include <system_error>

namespace bourne
{
/// Enumeration of different error codes, we use a bit of macro
/// uglyness to makes this easy. PHK says this is ok so if you have a
/// problem with it - take it up with him :)
///
/// http://phk.freebsd.dk/time/20141116.html
enum class error
{
#define BOURNE_ERROR_TAG(id,msg) id,
#include "detail/error_tags.hpp"
#undef BOURNE_ERROR_TAG
};

/// The error category
class error_category : public std::error_category
{
public:

    /// @return The name of the category
    const char* name() const noexcept override
    {
        return "bourne";
    }

    /// @return Human readable message corresponding to an error code
    std::string message(int error_value) const override
    {
        switch (static_cast<error>(error_value))
        {
#define BOURNE_ERROR_TAG(id, message) \
        case error::id:                \
            return message;
#include "detail/error_tags.hpp"
#undef BOURNE_ERROR_TAG
        }

        assert(0 && "Invalid error code received!");
        return "";
    }

    /// @return A reference to the error category
    static const std::error_category& instance()
    {
        static error_category instance;
        return instance;
    }
};

inline std::error_code make_error_code(error e)
{
    return std::error_code(static_cast<int>(e), error_category::instance());
}
}

namespace std
{
/// Reference: http://bit.ly/1RCq7ts
template <>
struct is_error_code_enum<bourne::error> : public true_type {};
}
