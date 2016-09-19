// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>

namespace bourne
{
    template <typename Container>
    class json_wrapper
    {

    private:

        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;

    public:

        json_wrapper(Container* val) :
            m_object(val)
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

        const_iterator begin() const
        {
            return m_object ? m_object->begin() : const_iterator();
        }

        const_iterator end() const
        {
            return m_object ? m_object->end() : const_iterator();
        }

    private:

        Container* m_object;

    };
}
