// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include "parser.hpp"
#include "../json.hpp"
#include "throw_if_error.hpp"
#include "../error.hpp"

#include <string>
#include <cmath>
#include <iostream>
#include <cctype>
#include <system_error>

namespace bourne
{
inline namespace STEINWURF_BOURNE_VERSION
{
namespace detail
{
json parser::parse(const std::string& input)
{
    std::error_code error;
    std::size_t offset = 0;
    auto result = parse_next(input, offset, error);
    throw_if_error(error);
    return result;
}
json parser::parse(const std::string& input, std::error_code& error)
{
    assert(!error);
    std::size_t offset = 0;
    auto result = parse_next(input, offset, error);
    if (error)
        return result;
    consume_white_space(input, offset);
    if (offset != input.size())
    {
        error = bourne::error::parse_found_multiple_unstructured_elements;
        return json(class_type::null);
    }
    assert(offset == input.size());
    return result;
}

void parser::consume_white_space(const std::string& input, size_t& offset)
{
    while (offset < input.size() && isspace(input[offset]))
    {
        offset++;
    }
}

json parser::parse_object(
    const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    json object = json(class_type::object);

    offset++;
    consume_white_space(input, offset);
    if (input[offset] == '}')
    {
        offset++;
        return object;
    }

    while (true)
    {
        json key = parse_next(input, offset, error);
        if (error)
            return json(class_type::null);

        consume_white_space(input, offset);
        if (input[offset] != ':')
        {
            error = bourne::error::parse_object_expected_colon;
            return json(class_type::null);
        }
        offset++;
        consume_white_space(input, offset);
        json value = parse_next(input, offset, error);
        if (error)
            return json(class_type::null);


        object[key.to_string()] = value;

        consume_white_space(input, offset);
        if (input[offset] == ',')
        {
            offset++;
            continue;
        }
        else if (input[offset] == '}')
        {
            offset++;
            break;
        }
        else
        {
            error = bourne::error::parse_object_expected_comma;
            return json(class_type::null);
        }
    }

    return object;
}

json parser::parse_array(
    const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    json array = json(class_type::array);
    uint32_t index = 0;

    offset++;
    consume_white_space(input, offset);
    if (input[offset] == ']')
    {
        offset++;
        return array;
    }

    while (true)
    {
        array[index++] = parse_next(input, offset, error);
        if (error)
            return json(class_type::null);
        consume_white_space(input, offset);

        if (input[offset] == ',')
        {
            offset++; continue;
        }
        else if (input[offset] == ']')
        {
            offset++; break;
        }
        else
        {
            error = bourne::error::parse_array_expected_comma_or_closing_bracket;
            return json(class_type::null);
        }
    }

    return array;
}

json parser::parse_string(
    const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    json string;
    std::string val;
    for (char c = input[++offset]; c != '\"' ; c = input[++offset])
    {
        if (c == '\\')
        {
            switch (input[ ++offset ])
            {
            case '\"':
                val += '\"';
                break;
            case '\\':
                val += '\\';
                break;
            case '/' :
                val += '/' ;
                break;
            case 'b' :
                val += '\b';
                break;
            case 'f' :
                val += '\f';
                break;
            case 'n' :
                val += '\n';
                break;
            case 'r' :
                val += '\r';
                break;
            case 't' :
                val += '\t';
                break;
            case 'u' :
            {
                val += "\\u" ;
                for (uint32_t i = 1; i <= 4; ++i)
                {
                    c = input[offset + i];
                    if ((c >= '0' && c <= '9') ||
                        (c >= 'a' && c <= 'f') ||
                        (c >= 'A' && c <= 'F'))
                    {
                        val += c;
                    }
                    else
                    {
                        error = bourne::error::parse_string_expected_hex_char;
                        return json(class_type::null);
                    }
                }
                offset += 4;
                break;
            }
            default:
                val += '\\';
                break;
            }
        }
        else
        {
            val += c;
        }
    }
    offset++;
    string = val;
    return string;
}

json parser::parse_number(
    const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    json number;
    std::string val;
    char c;
    bool is_floating = false;
    int64_t exp = 0;
    while (true)
    {
        c = input[offset++];
        if ((c == '-') || (c >= '0' && c <= '9'))
        {
            val += c;
        }
        else if (c == '.' && !is_floating)
        {
            val += c;
            is_floating = true;
        }
        else
        {
            break;
        }
    }
    if (tolower(c) == 'e')
    {
        std::string exp_str;

        c = input[offset++];
        if (c == '-')
        {
            offset++;
            exp_str += '-';
        }

        while (true)
        {
            c = input[offset++];
            if (c >= '0' && c <= '9')
            {
                exp_str += c;
            }
            else if (!isspace(c) && c != ',' && c != ']' && c != '}')
            {
                error = bourne::error::parse_number_expected_number_for_component;
                return json(class_type::null);
            }
            else
            {
                break;
            }
        }
        exp = std::stoll(exp_str);
    }
    else if (!isspace(c) && c != ',' && c != ']' && c != '}')
    {
        error = bourne::error::parse_number_unexpected_char;
        return json(class_type::null);
    }
    --offset;

    if (is_floating)
    {
        number = std::stod(val) * std::pow(10, exp);
    }
    else
    {
        number = std::stoll(val) * (uint64_t)std::pow(10, exp);
    }
    return number;
}

json parser::parse_bool(
    const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    json boolean;
    if (input.substr(offset, 4) == "true")
    {
        offset += 4;
        return true;
    }
    else if (input.substr(offset, 5) == "false")
    {
        offset += 5;
        return false;
    }
    else
    {
        error = bourne::error::parse_boolean_expected_true_or_false;
        return json(class_type::null);
    }

}

json parser::parse_null(
    const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    if (input.substr(offset, 4) != "null")
    {
        error = bourne::error::parse_null_expected_null;
        return json(class_type::null);
    }
    offset += 4;
    return json(class_type::null);
}

json parser::parse_next(
    const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    char value;
    consume_white_space(input, offset);
    value = input[offset];
    switch (value)
    {
    case '[': return parse_array(input, offset, error);
    case '{': return parse_object(input, offset, error);
    case '\"': return parse_string(input, offset, error);
    case 't':
    case 'f': return parse_bool(input, offset, error);
    case 'n': return parse_null(input, offset, error);
    default:
    {
        if ((value <= '9' && value >= '0') || value == '-')
        {
            return parse_number(input, offset, error);
        }
    }
    }
    error = bourne::error::parse_next_unexpected_char;
    return json(class_type::null);
}
}
}
}
