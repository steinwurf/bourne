// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>
#include <ostream>

#include "class_type.hpp"
#include "backing_data.hpp"
#include "json_const_wrapper.hpp"
#include "json_wrapper.hpp"

namespace bourne
{
    class json
    {
    private:

        template<class T, class R = void>
        using check_is_bool = std::enable_if<std::is_same<T, bool>::value, R>;

        template<class T, class R = void>
        using check_is_integral = std::enable_if<
                                  std::is_integral<T>::value && !std::is_same<T, bool>::value, R>;

        template<class T, class R = void>
        using check_is_floating_point = std::enable_if<
                                        std::is_floating_point<T>::value, R>;

        template<class T, class R = void>
        using check_is_string = std::enable_if<
                                std::is_convertible<T, std::string>::value, R>;

        using object_type = backing_data::object_type;

        using array_type = backing_data::array_type;

    public:

        json();

        json(class_type type);

        json(std::initializer_list<json> list);

        json(json&& other);
        json(const json& other);

        template <typename T>
        json(T b, typename check_is_bool<T>::type* = 0) :
            m_internal(b),
            m_type(class_type::boolean)
        {}

        template <typename T>
        json(T i, typename check_is_integral<T>::type* = 0) :
            m_internal((int64_t)i),
            m_type(class_type::integral)
        {}

        template <typename T>
        json(T f, typename check_is_floating_point<T>::type* = 0) :
            m_internal((double)f),
            m_type(class_type::floating)
        {}

        template <typename T>
        json(T s, typename check_is_string<T>::type* = 0) :
            m_internal(std::string(s)),
            m_type(class_type::string)
        {}

        json(std::nullptr_t);


        ~json();

    public:

        json& operator=(json&& other);
        json& operator=(const json& other);

        template <typename T>
        typename check_is_bool<T, json&>::type operator=(T b)
        {
            set_type(class_type::boolean);
            m_internal.m_bool = b;
            return *this;
        }

        template <typename T>
        typename check_is_integral<T, json&>::type operator=(T i)
        {
            set_type(class_type::integral);
            m_internal.m_int = i;
            return *this;
        }

        template <typename T>
        typename check_is_floating_point<T, json&>::type operator=(T f)
        {
            set_type(class_type::floating); m_internal.m_float = f;
            return *this;
        }

        template <typename T>
        typename check_is_string<T, json&>::type operator=(T s)
        {
            set_type(class_type::string); *m_internal.m_string = std::string(s);
            return *this;
        }

        json& operator[](const std::string& key);
        json& operator[](uint32_t index);

        bool operator==(const json& other) const;
        bool operator!=(const json& other) const;

    public:

        template <typename T>
        void append(T arg)
        {
            set_type(class_type::array);
            m_internal.m_array->emplace_back(arg);
        }

        template <typename T, typename... U>
        void append(T arg, U... args)
        {
            append(arg);
            append(args...);
        }

        json& at(const std::string& key);
        const json& at(const std::string& key) const;

        json& at(uint32_t index);
        const json& at(uint32_t index) const;

        int32_t length() const;

        bool has_key(const std::string& key) const;

        int32_t size() const;

        class_type json_type() const;

    public:

        // Functions for checking the type of the json attribute
        bool is_null() const;
        bool is_bool() const;
        bool is_int() const;
        bool is_float() const;
        bool is_string() const;
        bool is_object() const;
        bool is_array() const;

        /// Functions for getting primitives from the json object.
        bool to_bool() const;
        int64_t to_int() const;
        double to_float() const;
        std::string to_string() const;

        json_wrapper<object_type> object_range();
        json_const_wrapper<object_type> object_range() const;

        json_wrapper<array_type> array_range();
        json_const_wrapper<array_type> array_range() const;

        std::string dump(uint32_t depth = 1, std::string tab = "  ") const;

        friend std::ostream& operator<<(std::ostream&, const json&);

    public:

        /// Static API
        static json parse(const std::string& string);

        static json array();
        template <typename... T>
        static json array(T... args)
        {
            json array = json(class_type::array);
            array.append(args...);
            return array;
        }

        static json object();
        static json object(std::initializer_list<json> list);

    private:

        void clean_up();
        void set_type(class_type type);

    private:

        backing_data m_internal;
        class_type m_type = class_type::null;
    };

    std::ostream& operator<<(std::ostream& os, const json& json);
}
