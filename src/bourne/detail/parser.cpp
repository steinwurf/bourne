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
#include <iomanip>
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
    auto result = parse_next(input_stream, error);
    throw_if_error(error);
    return result;
}
json parser::parse(const std::string& input, std::error_code& error)
{
    auto input_stream = std::istringstream{input};
    assert(!error);
    auto result = parse_next(input_stream, error);
    if (error)
        return result;
    consume_white_space(input_stream);
    std::cout << "input.size: " << input.size() << std::endl;
    std::cout << "input.peek: " << std::char_traits<char>::eof() << std::endl;

    if (input_stream.peek() != std::char_traits<char>::eof())
    {
        error = bourne::error::parse_found_multiple_unstructured_elements;
        return json(class_type::null);
    }
    assert(input_stream.peek() == std::char_traits<char>::eof());
    return result;
}

json parser::parse(std::istream& input)
{
    std::error_code error;
    auto result = parse_next(input, error);
    throw_if_error(error);
    return result;
}

json parser::parse(std::istream& input, std::error_code& error)
{
    assert(!error);
    auto result = parse_next(input, error);
    if (error)
        return result;
    consume_white_space(input);
    std::cout << "input.peek '" << input.peek() << "'" << std::endl;

    if (!input.eof())
    {
        error = bourne::error::parse_found_multiple_unstructured_elements;
        return json(class_type::null);
    }
    assert(input.eof());
    return result;
}

void parser::consume_white_space(std::istream& input)
{
    char c;
    while (std::isspace(input.peek()))
    {
        std::cout << "consume_white_space: '" << static_cast<char>(input.peek()) << "'" << std::endl;
        input.get(c);
    }
}

bool string_exists(std::istream& input, std::string str)
{
    auto old_position = input.tellg();
    for (size_t i = 0; i < str.size(); i++)
    {
        std::cout << "input.peek: " << static_cast<char>(input.peek()) << std::endl;
        if (input.peek() != str[i])
        {
            input.seekg(old_position); // rewind
            return false;
        }
        input.get();
    }
    return true;
}

json parser::parse_object(std::istream& input, std::error_code& error)
{
    assert(!error);
    json object = json(class_type::object);

    char head;
    input.get(head);
    if (head != '{')
    {
        error = bourne::error::parse_object_expected_head;
        return json(class_type::null);
    }
    std::cout << "object peek 0'" << static_cast<char>(input.peek()) << "'" << std::endl;

    consume_white_space(input);
    head = input.peek();

    if (head == '}')
    {
        input.get(head);
        std::cout << "return object" << std::endl;

        return object;
    }
    std::cout << "forbi if" << head << std::endl;

    while (true)
    {
        std::cout << "object peek 1'" << static_cast<char>(input.peek()) << "'"
                  << std::endl;
        json key = parse_next(input, error);
        std::cout << "object peek 2'" << static_cast<char>(input.peek()) << "'"
                  << std::endl;
        if (error)
            return json(class_type::null);
        consume_white_space(input);

        std::cout << "object peek is in string parse 3'"
                  << static_cast<char>(input.peek()) << "'" << std::endl;

        head = input.peek();
        if (head != ':')
        {
            error = bourne::error::parse_object_expected_colon;
            return json(class_type::null);
        }
        input.get(head);
        std::cout << "head '" << head << "'" << std::endl;
        std::cout << "peek is in string parse 4'"
                  << static_cast<char>(input.peek()) << "'" << std::endl;

        consume_white_space(input);
        std::cout << "peek is in string parse'"
                  << static_cast<char>(input.peek()) << "'" << std::endl;

        json value = parse_next(input, error);
        if (error)
            return json(class_type::null);

        object[key.to_string()] = value;

        consume_white_space(input);
        input.get(head);
        if (head == ',')
        {

            continue;
        }
        else if (head == '}')
        {

            break;
        }
        else
        {
            error = bourne::error::parse_object_expected_comma;
            return json(class_type::null);
        }
    }

    std::cout << "return object" << head << std::endl;

    return object;
}

json parser::parse_array(std::istream& input, std::error_code& error)
{
    assert(!error);
    json array = json(class_type::array);
    std::size_t index = 0;

    char head;
    // consume opening bracket
    input.get(head);

    consume_white_space(input);
    if (head == ']')
    {

        return array;
    }

    while (true)
    {
        array[index++] = parse_next(input, error);
        std::cout << "index " << index << std::endl;
        if (error)
        {
            std::cout << "error:" << error.message() << std::endl;
            return json(class_type::null);
        }
        consume_white_space(input);
        std::cout << "peek is '" << static_cast<char>(input.peek()) << "'"
                  << std::endl;
        input.get(head);
        if (head == ',')
        {

            continue;
        }
        else if (head == ']')
        {

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

json parser::parse_string(std::istream& input, std::error_code& error)
{
    assert(!error);
    json string;
    std::string val;
    char c;
    input.get(c);

    std::cout << "c is '" << static_cast<char>(input.peek()) << "'"
              << std::endl;
    for (input.get(c); c != '\"'; input.get(c))
    {
        std::cout << "c is for loop '" << c << "'" << std::endl;
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
                for (std::size_t i = 1; i <= 4; ++i)
                {
                    //c = input.peek();
                    std::cout << "hex is '" << c << "'" << std::endl;
                    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
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
                // TODO Fix hex
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

    std::cout << "ce is '" << c << "'" << std::endl;
    std::cout << "val is '" << val << "'" << std::endl;

    string = val;
    return string;
}

json parser::parse_number(std::istream& input, std::error_code& error)
{
    std::cout << "parse number '" << static_cast<char>(input.peek()) << "'"
              << std::endl;

    assert(!error);
    json number;
    std::string val;
    char c;
    bool is_floating = false;
    int64_t exp = 0;
    while (true)
    {
        input.get(c);

        std::cout << "number is '" << c << "'" << std::endl;
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
            else if (input.fail() || (!isspace(c) && c != ',' && c != ']' && c != '}'))
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
        std::cout << "is space? '" << c << "'" << std::endl;
        error = bourne::error::parse_number_unexpected_char;
        return json(class_type::null);
    }

    input.putback(c);

    if (is_floating)
    {
        number = std::stod(val) * std::pow(10, exp);
    }
    else
    {
        number = std::stoll(val) * (uint64_t)std::pow(10, exp);
    }
    std::cout << "number is '" << number.dump() << "'" << std::endl;
    std::cout << "parse number end '" << static_cast<char>(input.peek()) << "'"
              << std::endl;
    return number;
}

json parser::parse_bool(std::istream& input, std::error_code& error)
{
    assert(!error);
    json boolean;

    if (string_exists(input, "true"))
    {
        return json(true);
    }
    else if (string_exists(input, "false"))
    {
        return json(false);
    }
    else
    {
        error = bourne::error::parse_boolean_expected_true_or_false;
        return json(class_type::null);
    }
}

json parser::parse_null(std::istream& input, std::error_code& error)
{
    assert(!error);
    if (!string_exists(input, "null"))
    {
        error = bourne::error::parse_null_expected_null;
        return json(class_type::null);
    }
    return json(class_type::null);
}

json parser::parse_next(std::istream& input, std::error_code& error)
{
    std::cout << "parse next" << std::endl;
    assert(!error);
    consume_white_space(input);
    char value = input.peek();
    std::cout << "parse next head: " << value << std::endl;

    switch (value)
    {
    case '[':
        return parse_array(input, error);
    case '{':
        return parse_object(input, error);
    case '\"':
        return parse_string(input, error);
    case 't':
    case 'f':
        return parse_bool(input, error);
    case 'n':
        return parse_null(input, error);
    default:
    {
        if ((value <= '9' && value >= '0') || value == '-')
        {
            auto number = parse_number(input, error);
            std::cout << "parse next number: " << number.dump() << std::endl;
            return number;
        }
    }
    }
    std::cout << "parse next end" << std::endl;
    error = bourne::error::parse_next_unexpected_char;
    return json(class_type::null);
}
}
}
}
