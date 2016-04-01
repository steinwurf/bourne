// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#pragma once

#include <cassert>

namespace bourne
{
    template <typename Container>
    class json_const_wrapper
    {

    private:

        using iterator = typename Container::const_iterator;

    public:

        json_const_wrapper(const Container* val):
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
