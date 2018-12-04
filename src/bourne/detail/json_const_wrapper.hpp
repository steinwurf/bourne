// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>

namespace bourne
{
namespace detail
{
template <typename Container>
class json_const_wrapper
{

private:

    using iterator = typename Container::const_iterator;

public:

    json_const_wrapper(const Container* val) :
        m_object(val)
    {
        assert(m_object);
    }

    iterator begin() const
    {
        return m_object ? m_object->begin() : iterator();
    }

    iterator end() const
    {
        return m_object ? m_object->end() : iterator();
    }

private:

    const Container* m_object;
};
}
}
