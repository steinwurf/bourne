// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#pragma once

#include "json_const_wrapper.hpp"

namespace bourne
{
    template <typename Container>
    class json_wrapper : public json_const_wrapper<Container>
    {

    private:

        using iterator = typename Container::iterator;

    public:

        json_wrapper(Container *val):
            json_const_wrapper<Container>(val),
            m_object(val)
        {}

        json_wrapper(std::nullptr_t):
            json_const_wrapper<Container>(nullptr),
            m_object(nullptr)
        {}

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
