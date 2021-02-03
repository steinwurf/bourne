// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <string>

#pragma once

namespace cpuid
{
/// Here we define the STEINWURF_BOURNE_VERSION this should be updated on each
/// release
#define STEINWURF_BOURNE_VERSION v8_0_0

inline namespace STEINWURF_BOURNE_VERSION
{
/// @return The version of the library as string
std::string version();
}
}