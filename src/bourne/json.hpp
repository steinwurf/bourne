// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <set>
#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

#include "class_type.hpp"
#include "detail/backing_data.hpp"
#include "detail/json_const_wrapper.hpp"
#include "detail/json_wrapper.hpp"
#include "version.hpp"

namespace bourne
{
inline namespace STEINWURF_BOURNE_VERSION
{
/// A json object
class json
{
private:
    template <class T, class R = void>
    using check_is_bool = std::enable_if<std::is_same<T, bool>::value, R>;

    template <class T, class R = void>
    using check_is_integral = std::enable_if<
        std::is_integral<T>::value && !std::is_same<T, bool>::value, R>;

    template <class T, class R = void>
    using check_is_floating_point =
        std::enable_if<std::is_floating_point<T>::value, R>;

    template <class T, class R = void>
    using check_is_string =
        std::enable_if<std::is_convertible<T, std::string>::value, R>;

private:
    using object_type = detail::backing_data::object_type;

    using array_type = detail::backing_data::array_type;

public:
    /// Default constructor, creates a null value.
    json();

    /// Creates an empty json value of the given type
    json(class_type type);

    /// Creates a json object.
    /// Odd indexed items in the list will serve as keys for their even counter
    /// part.
    json(std::initializer_list<json> list);

    /// Move constructor.
    /// Ensures the backing_data is owned my the correct object.
    json(json&& other);

    /// Copy constructor.
    json(const json& other);

    /// Constructor for creating a boolean value.
    template <typename T>
    json(T b, typename check_is_bool<T>::type* = 0) :
        m_internal(b), m_type(class_type::boolean)
    {
    }

    /// Constructor for creating an integral value.
    template <typename T>
    json(T i, typename check_is_integral<T>::type* = 0) :
        m_internal((int64_t)i), m_type(class_type::integral)
    {
    }

    /// Constructor for creating a floating point value.
    template <typename T>
    json(T f, typename check_is_floating_point<T>::type* = 0) :
        m_internal((double)f), m_type(class_type::floating)
    {
    }

    /// Constructor for creating a string value.
    template <typename T>
    json(T s, typename check_is_string<T>::type* = 0) :
        m_internal(std::string(s)), m_type(class_type::string)
    {
    }

    /// Constructor for creating a null value.
    json(std::nullptr_t);

    /// Destructor
    ~json();

    /// Assignment operator for json
    json& operator=(json&& other);

    /// Assignment operator for json
    json& operator=(const json& other);

    /// Assignment operator for boolean
    template <typename T>
    typename check_is_bool<T, json&>::type operator=(T b)
    {
        set_type(class_type::boolean);
        m_internal.m_bool = b;
        return *this;
    }

    /// Assignment operator for integral
    template <typename T>
    typename check_is_integral<T, json&>::type operator=(T i)
    {
        set_type(class_type::integral);
        m_internal.m_int = i;
        return *this;
    }

    /// Assignment operator for floating point
    template <typename T>
    typename check_is_floating_point<T, json&>::type operator=(T f)
    {
        set_type(class_type::floating);
        m_internal.m_float = f;
        return *this;
    }

    /// Assignment operator for string value
    template <typename T>
    typename check_is_string<T, json&>::type operator=(T s)
    {
        set_type(class_type::string);

        // use assignment operator to copy over the string
        *m_internal.m_string = std::string(s);
        return *this;
    }

    /// Assignment operator for null value
    json& operator=(std::nullptr_t);

    /// Access operator for keys this assumes the json value is of type object
    json& operator[](const std::string& key);

    /// Access operator for keys this assumes the json value is of type object
    const json& operator[](const std::string& key) const;

    /// Access operator for index this assumes the json value is of type array
    /// given index
    json& operator[](std::size_t index);

    /// Access operator for keys this assumes the json value is of type array
    /// given index
    const json& operator[](std::size_t index) const;

    /// Returns a reference to the json value of the element identified with the
    /// given key.
    json& at(const std::string& key);

    /// Returns a reference to the json value of the element identified with the
    /// given key.
    const json& at(const std::string& key) const;

    /// Returns a reference to the json value of the element identified with the
    /// given key.
    json& at(std::size_t index);

    /// Returns a reference to the json value of the element identified with the
    /// given key.
    const json& at(std::size_t index) const;

    /// Append a json value to this json array. If this object is not a json
    /// array, an assert will be triggered.
    template <typename T>
    void append(T arg)
    {
        assert(is_array());
        m_internal.m_array->emplace_back(arg);
    }

    /// Append multiple json values to this json array. If this object is not a
    /// json array, an assert will be triggered.
    template <typename T, typename... U>
    void append(T arg, U... args)
    {
        assert(is_array());
        append(arg);
        append(args...);
    }

    /// Returns true if the key is available. This functions assumes this object
    /// is a json object value.
    bool has_key(const std::string& key) const;

    /// Returns the available set of keys. This functions assumes this object
    /// is a json object value.
    std::vector<std::string> keys() const;

    /// Returns the number of elements in this object or array. If this is not
    /// an object or array, an assert will be triggered.
    std::size_t size() const;

    /// Returns the type of this object.
    class_type json_type() const;

    /// Returns true if this object matches the given type
    template <class T>
    bool is() const
    {
        if (std::is_same<T, bool>::value)
        {
            return is_bool();
        }
        else if (std::is_integral<T>::value)
        {
            return is_int();
        }
        else if (std::is_floating_point<T>::value)
        {
            return is_float();
        }
        else if (std::is_convertible<T, std::string>::value)
        {
            return is_string();
        }
        return false;
    }

    /// Returns true if this object is a null value
    bool is_null() const;

    /// Returns true if this object is a boolean value
    bool is_bool() const;

    /// Returns true if this object is a integer value
    bool is_int() const;

    /// Returns true if this object is a floating point value
    bool is_float() const;

    /// Returns true if this object is a string value
    bool is_string() const;

    /// Returns true if this object is a object value
    bool is_object() const;

    /// Returns true if this object is a array value
    bool is_array() const;

    /// Converter for extracting the boolean value.
    template <typename T>
    void to(T& t, typename check_is_bool<T>::type* = 0) const
    {
        t = (T)to_bool();
    }

    /// Converter for extracting the integral value.
    template <typename T>
    void to(T& t, typename check_is_integral<T>::type* = 0) const
    {
        t = (T)to_int();
    }

    /// Converter for extracting the floating point value.
    template <typename T>
    void to(T& t, typename check_is_floating_point<T>::type* = 0) const
    {
        t = (T)to_float();
    }

    /// Converter for extracting the string value.
    template <typename T>
    void to(T& t, typename check_is_string<T>::type* = 0) const
    {
        t = (T)to_string();
    }

    /// Returns the underlying boolean value of this object. If this is not a
    /// boolean value an assert is triggered.
    bool to_bool() const;

    /// Returns the underlying integer value of this object. If this is not a
    /// interger value an assert is triggered.
    int64_t to_int() const;

    /// Returns the underlying floating point value of this object. If this is
    /// not a floating point value an assert is triggered.
    double to_float() const;

    /// Returns the underlying string value of this object. If this is not a
    /// string value an assert is triggered.
    std::string to_string() const;

    /// Returns an iterable object range. If this is not an object value an
    /// assert is triggered.
    detail::json_wrapper<object_type> object_range();

    /// Returns an const iterable object range. If this is not an object value
    /// an assert is triggered.
    detail::json_const_wrapper<object_type> object_range() const;

    /// Returns an iterable array range. If this is not an array value an
    /// assert is triggered.
    detail::json_wrapper<array_type> array_range();

    /// Returns an const iterable array range. If this is not an array value
    /// an assert is triggered.
    detail::json_const_wrapper<array_type> array_range() const;

    /// Dumps this object as a json string.
    std::string dump(uint32_t depth = 1, std::string tab = "  ") const;

    /// Dumps this object as a minified json string.
    std::string dump_min() const;

    /// Friend function for the insertion operator. This will insert the json
    /// string of this object to the ostream.
    friend std::ostream& operator<<(std::ostream&, const json&);

    /// Equality operator, this compares if the content of two json objects are
    /// the same.
    bool operator==(const json& other) const;

    /// Inequality operator, negated equality operator.
    bool operator!=(const json& other) const;

    /// Check if this object contains the other object, i.e., if the other
    /// object is the same or nested in this object based on pointer equality.
    /// @param other The object to check if it is contained in this object.
    /// @return true if the other object is contained in this object, otherwise
    /// false.
    bool contains(const json& other) const;

    /// Parse a string as a json object.
    static json parse(const std::string& input, std::error_code& error);

    /// Parse a string as a json object.
    static json parse(const std::string& input);

    /// Create a json array
    static json array();
    template <typename... T>
    /// Create a json array
    static json array(T... args)
    {
        json array = json(class_type::array);
        array.append(args...);
        return array;
    }

    /// Create an empty json object
    static json null();

    /// Create an empty json object
    static json object();

    /// Create a json object
    static json object(std::initializer_list<json> list);

private:
    /// Clears this object - deletes the backing data and sets the type to null
    void clear();

    /// Updates the type of this object, and initializes the backing data based
    /// on this.
    void set_type(class_type type);

private:
    /// The object containing the underlying data
    detail::backing_data m_internal;

    /// The type of this object
    class_type m_type = class_type::null;
};

std::ostream& operator<<(std::ostream& os, const json& json);
}
}
