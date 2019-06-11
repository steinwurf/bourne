// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include "version.hpp"

namespace bourne
{
inline namespace STEINWURF_BOURNE_VERSION
{
enum class class_type
{
    null,
    object,
    array,
    string,
    floating,
    integral,
    boolean
};
}
}
