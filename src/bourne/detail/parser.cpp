// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include "parser.hpp"
#include "../error.hpp"
#include "../json.hpp"
#include "throw_if_error.hpp"

#include <cctype>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

namespace bourne
{
inline namespace STEINWURF_BOURNE_VERSION
{
namespace detail
{
json parser::parse(const std::string& input)
{
    auto input_stream = std::istringstream{input};
    std::error_code error;
    std::size_t offset = 0;
    auto result = parse_next(input_stream, offset, error);
    throw_if_error(error);
    return result;
}
json parser::parse(const std::string& input, std::error_code& error)
{
    auto input_stream = std::istringstream{input};
    assert(!error);
    std::size_t offset = 0;
    auto result = parse_next(input_stream, offset, error);
    if (error)
        return result;
    consume_white_space(input_stream, offset);
    if (offset != input.size())
    {
        error = bourne::error::parse_found_multiple_unstructured_elements;
        return json(class_type::null);
    }
    assert(offset == input.size());
    return result;
}

json parser::parse(std::istream& input)
{
    std::error_code error;
    std::size_t offset = 0;
    auto result = parse_next(input, offset, error);
    throw_if_error(error);
    return result;
}

json parser::parse(std::istream& input, std::error_code& error)
{
    assert(!error);
    std::size_t offset = 0;
    auto result = parse_next(input, offset, error);
    if (error)
        return result;
    consume_white_space(input, offset);

    if (!input.eof())
    {
        error = bourne::error::parse_found_multiple_unstructured_elements;
        return json(class_type::null);
    }
    assert(input.eof());
    return result;
}

void parser::consume_white_space(std::istream& input, size_t& offset)
{
    char c;
    while (input.get(c) && std::isspace(c))
    {
        offset++;
    }
}

bool string_exists(std::istream& input, std::string str){
    auto old_position = input.tellg();

    for (size_t i = 0; i < str.size(); i++)
    {
        if (input.peek() != str[i])
        {
            input.seekg (old_position); // rewind
            return false;
        }
        input.get();
    }
    return true;
}

json parser::parse_object(std::istream& input, size_t& offset,
                          std::error_code& error)
{
    assert(!error);
    json object = json(class_type::object);

    char head;
    input.get(head);
    consume_white_space(input, offset);
    if (head == '}')
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
        input.get(head);
        if (head != ':')
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
        input.get(head);
        if (head == ',')
        {
            offset++;
            continue;
        }
        else if (head == '}')
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

json parser::parse_array(std::istream& input, size_t& offset,
                         std::error_code& error)
{
    assert(!error);
    json array = json(class_type::array);
    std::size_t index = 0;

    char head;
    // consume opening bracket
    input.get(head);
    consume_white_space(input, offset);
    input.get(head);
    if (head == ']')
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

        input.get(head);
        if (head == ',')
        {
            offset++;
            continue;
        }
        else if (head == ']')
        {
            offset++;
            break;
        }
        else
        {
            error =
                bourne::error::parse_array_expected_comma_or_closing_bracket;
            return json(class_type::null);
        }
    }

    return array;
}

json parser::parse_string(std::istream& input, size_t& offset,
                          std::error_code& error)
{
    assert(!error);
    json string;
    std::string val;
    char c;
    for (input.get(c); c != '\"'; input.get(c))
    {
        if (c == '\\')
        {
            input.get(c);
            switch (c)
            {
            case '\"':
                val += '\"';
                break;
            case '\\':
                val += '\\';
                break;
            case '/':
                val += '/';
                break;
            case 'b':
                val += '\b';
                break;
            case 'f':
                val += '\f';
                break;
            case 'n':
                val += '\n';
                break;
            case 'r':
                val += '\r';
                break;
            case 't':
                val += '\t';
                break;
            case 'u':
            {
                val += "\\u";
                auto old_position = input.tellg();
                for (std::size_t i = 1; i <= 4; ++i)
                {
                    input.get(c);
                    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
                        (c >= 'A' && c <= 'F'))
                    {
                        val += c;
                    }
                    else
                    {
                        input.seekg (old_position); // rewind
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

json parser::parse_number(std::istream& input, size_t& offset,
                          std::error_code& error)
{
    assert(!error);
    json number;
    std::string val;
    char c;
    bool is_floating = false;
    int64_t exp = 0;
    while (true)
    {
        input.get(c);
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

        input.get(c);
        if (c == '-')
        {
            input.get(c);
            exp_str += '-';
        }

        while (true)
        {
            input.get(c);
            if (c >= '0' && c <= '9')
            {
                exp_str += c;
            }
            else if (!isspace(c) && c != ',' && c != ']' && c != '}')
            {
                error =
                    bourne::error::parse_number_expected_number_for_component;
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

json parser::parse_bool(std::istream& input, size_t& offset,
                        std::error_code& error)
{
    assert(!error);
    json boolean;

    if (string_exists(input, "true"))
    {
        offset += 4;
        return json(true);
    }
    else if (string_exists(input, "false"))
    {
        offset += 5;
        return json(false);
    }
    else
    {
        error = bourne::error::parse_boolean_expected_true_or_false;
        return json(class_type::null);
    }
}

json parser::parse_null(std::istream& input, size_t& offset,
                        std::error_code& error)
{
    assert(!error);
    if (!string_exists(input, "null"))
    {
        error = bourne::error::parse_null_expected_null;
        return json(class_type::null);
    }
    offset += 4;
    return json(class_type::null);
}

json parser::parse_next(std::istream& input, size_t& offset,
                        std::error_code& error)
{
    assert(!error);
    char value;
    std::cout << "first char is: " << input.peek() << std::endl;
    //consume_white_space(input, offset);
    input.get(value);
    switch (value)
    {
    case '[':
        return parse_array(input, offset, error);
    case '{':
        return parse_object(input, offset, error);
    case '\"':
        return parse_string(input, offset, error);
    case 't':
    case 'f':
        return parse_bool(input, offset, error);
    case 'n':
        return parse_null(input, offset, error);
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
