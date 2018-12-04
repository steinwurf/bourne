// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include "parser.hpp"
#include "../json.hpp"
#include "../stdfix.hpp"

#include <string>
#include <cmath>
#include <iostream>
#include <cctype>

namespace bourne
{
namespace detail
{
json parser::parse(const std::string& input, std::error_code& error)
{
    assert(!error);
    std::size_t offset = 0;
    return parse_next(input, offset, error);
}

void parser::consume_white_space(const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    while (isspace(input[offset]))
    {
        offset++;
    }
}

json parser::parse_object(const std::string& input, size_t& offset, std::error_code& error)
{
    assert(!error);
    json object = json(class_type::object);

    offset++;
    consume_white_space(input, offset, error);
    if (input[offset] == '}')
    {
        offset++;
        return object;
    }

    while (true)
    {
        json Key = parse_next(input, offset, error);
        consume_white_space(input, offset, error);
        if (input[offset] != ':')
        {
            error = std::make_error_code(std::errc::invalid_argument);
            std::cerr << "Error: object: Expected colon, found '"
                      << input[offset] << "'\n";
            break;
        }
        offset++;
        consume_white_space(input, offset, error);
        json Value = parse_next(input, offset, error);
        object[Key.to_string()] = Value;

        consume_white_space(input, offset, error);
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
            error = std::make_error_code(std::errc::invalid_argument);
            std::cerr << "ERROR: object: Expected comma, found '"
                      << input[offset] << "'\n";
            break;
        }
    }

    return object;
}

json parser::parse_array(const std::string& input, size_t& offset, std::error_code& error)
{
    json array = json(class_type::array);
    uint32_t index = 0;

    offset++;
    consume_white_space(input, offset, error);
    if (input[offset] == ']')
    {
        offset++;
        return array;
    }

    while (true)
    {
        array[index++] = parse_next(input, offset, error);
        consume_white_space(input, offset, error);

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
            error = std::make_error_code(std::errc::invalid_argument);
            std::cerr << "ERROR: array: Expected ',' or ']', found '"
                      << input[offset] << "'\n";
            return json(class_type::array);
        }
    }

    return array;
}

json parser::parse_string(const std::string& input, size_t& offset, std::error_code& error)
{
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
                        error = std::make_error_code(std::errc::invalid_argument);
                        std::cerr << "ERROR: string: Expected hex "
                                  << "character in unicode escape, found '"
                                  << c << "'\n";
                        return json(class_type::string);
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

json parser::parse_number(const std::string& input, size_t& offset, std::error_code& error)
{
    json number;
    std::string val, exp_str;
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
    if (c == 'E' || c == 'e')
    {
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
                // Error: expected a number for exponent
                error = std::make_error_code(std::errc::invalid_argument);
                return json(class_type::null);
            }
            else
            {
                break;
            }
        }
        exp = stdfix::stol(exp_str);
    }
    else if (!isspace(c) && c != ',' && c != ']' && c != '}')
    {
        error = std::make_error_code(std::errc::invalid_argument);
        // Error: unexpected character
        return json(class_type::null);
    }
    --offset;

    if (is_floating)
    {
        number = stdfix::stod(val) * std::pow(10, exp);
    }
    else
    {
        if (!exp_str.empty())
        {
            number = stdfix::stol(val) * std::pow(10, exp);
        }
        else
        {
            number = stdfix::stol(val);
        }
    }
    return number;
}

json parser::parse_bool(const std::string& input, size_t& offset, std::error_code& error)
{
    json boolean;
    if (input.substr(offset, 4) == "true")
    {
        boolean = true;
    }
    else if (input.substr(offset, 5) == "false")
    {
        boolean = false;
    }
    else
    {
        error = std::make_error_code(std::errc::invalid_argument);
        // Error: expected bool ('true' or 'false')
        return json(class_type::null);
    }
    offset += boolean.to_bool() ? 4 : 5;
    return boolean;
}

json parser::parse_null(const std::string& input, size_t& offset, std::error_code& error)
{
    json null;
    if (input.substr(offset, 4) != "null")
    {
        error = std::make_error_code(std::errc::invalid_argument);
        // "Error: expected 'null
        return null;
    }
    offset += 4;
    return null;
}

json parser::parse_next(const std::string& input, size_t& offset, std::error_code& error)
{
    char value;
    consume_white_space(input, offset, error);
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
    // Error: Unknown starting character
    error = std::make_error_code(std::errc::invalid_argument);
    return json();
}
}
}
