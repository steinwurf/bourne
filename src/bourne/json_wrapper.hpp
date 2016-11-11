// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include "json_const_wrapper.hpp"

#include <cassert>

namespace bourne
{
template <class Container>
class json_wrapper : public json_const_wrapper<Container>
{

private:

    using iterator = typename Container::iterator;

public:

    json_wrapper(Container* json) :
        json_const_wrapper<Container>(json),
        m_object(json)
    {
        assert(m_object);
    }

    iterator begin()
    {
        return m_object ? m_object->begin() : iterator();
    }

    iterator end()
    {
        return m_object ? m_object->end() : iterator();
    }

private:

    Container* m_object;

};
}
