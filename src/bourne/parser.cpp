// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include "parser.hpp"
#include "json.hpp"
#include "stdfix.hpp"
#include <string>
#include <cmath>
#include <iostream>

namespace bourne
{
    parser::parser(const std::string& string):
        m_string(string),
        m_offset(0)
    { }

    json parser::parse()
    {
        m_offset = 0;
        return parse_next();
    }

    void parser::consume_white_space()
    {
        while(isspace(m_string[m_offset]))
        {
            m_offset++;
        }
    }

    json parser::parse_object()
    {
        json object = json(class_type::object);

        m_offset++;
        consume_white_space();
        if (m_string[m_offset] == '}')
        {
            m_offset++;
            return std::move(object);
        }

        while(true)
        {
            json Key = parse_next();
            consume_white_space();
            if (m_string[m_offset] != ':')
            {
                std::cerr << "Error: object: Expected colon, found '" << m_string[m_offset] << "'\n";
                break;
            }
            m_offset++;
            consume_white_space();
            json Value = parse_next();
            object[Key.to_string()] = Value;

            consume_white_space();
            if (m_string[m_offset] == ',')
            {
                m_offset++;
                continue;
            }
            else if (m_string[m_offset] == '}')
            {
                m_offset++;
                break;
            }
            else
            {
                std::cerr << "ERROR: object: Expected comma, found '" << m_string[m_offset] << "'\n";
                break;
            }
        }

        return std::move(object);
    }

    json parser::parse_array()
    {
        json array = json(class_type::array);
        unsigned index = 0;

        m_offset++;
        consume_white_space();
        if (m_string[m_offset] == ']')
        {
            m_offset++;
            return std::move(array);
        }

        while (true)
        {
            array[index++] = parse_next();
            consume_white_space();

            if (m_string[m_offset] == ',')
            {
                m_offset++; continue;
            }
            else if (m_string[m_offset] == ']')
            {
                m_offset++; break;
            }
            else
            {
                std::cerr << "ERROR: array: Expected ',' or ']', found '" << m_string[m_offset] << "'\n";
                return std::move(json(class_type::array));
            }
        }

        return std::move(array);
    }

    json parser::parse_string()
    {
        json string;
        std::string val;
        for (char c = m_string[++m_offset]; c != '\"' ; c = m_string[++m_offset])
        {
            if (c == '\\')
            {
                switch(m_string[ ++m_offset ])
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
                    for (unsigned i = 1; i <= 4; ++i)
                    {
                        c = m_string[m_offset + i];
                        if ((c >= '0' && c <= '9') ||
                            (c >= 'a' && c <= 'f') ||
                            (c >= 'A' && c <= 'F'))
                        {
                            val += c;
                        }
                        else
                        {
                            std::cerr << "ERROR: string: Expected hex character in unicode escape, found '" << c << "'\n";
                            return std::move(json(class_type::string));
                        }
                    }
                    m_offset += 4;
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
        m_offset++;
        string = val;
        return std::move(string);
    }

    json parser::parse_number()
    {
        json number;
        std::string val, exp_str;
        char c;
        bool is_floating = false;
        int64_t exp = 0;
        while(true) {
            c = m_string[m_offset++];
            if ((c == '-') || (c >= '0' && c <= '9'))
                val += c;
            else if (c == '.') {
                val += c;
                is_floating = true;
            }
            else
                break;
        }
        if (c == 'E' || c == 'e')
        {
            c = m_string[m_offset++];
            if (c == '-')
            {
                m_offset++;
                exp_str += '-';
            }

            while(true)
            {
                c = m_string[m_offset++];
                if (c >= '0' && c <= '9')
                {
                    exp_str += c;
                }
                else if (!isspace(c) && c != ',' && c != ']' && c != '}')
                {
                    std::cerr << "ERROR: number: Expected a number for exponent, found '" << c << "'\n";
                    return std::move(json(class_type::null));
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
            std::cerr << "ERROR: number: unexpected character '" << c << "'\n";
            return std::move(json(class_type::null));
        }
        --m_offset;

        if (is_floating)
        {
            number = std::stod(val) * std::pow(10, exp);
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
        return std::move(number);
    }

    json parser::parse_bool()
    {
        json boolean;
        if (m_string.substr(m_offset, 4) == "true")
        {
            boolean = true;
        }
        else if (m_string.substr(m_offset, 5) == "false")
        {
            boolean = false;
        }
        else
        {
            std::cerr << "ERROR: bool: Expected 'true' or 'false', found '" << m_string.substr(m_offset, 5) << "'\n";
            return std::move(json(class_type::null));
        }
        m_offset += (boolean.to_bool() ? 4 : 5);
        return std::move(boolean);
    }

    json parser::parse_null()
    {
        json null;
        if (m_string.substr(m_offset, 4) != "null")
        {
            std::cerr << "ERROR: null: Expected 'null', found '" << m_string.substr(m_offset, 4) << "'\n";
            return std::move(json(class_type::null));
        }
        m_offset += 4;
        return std::move(null);
    }

    json parser::parse_next()
    {
        char value;
        consume_white_space();
        value = m_string[m_offset];
        switch(value)
        {
            case '[': return std::move(parse_array());
            case '{': return std::move(parse_object());
            case '\"': return std::move(parse_string());
            case 't':
            case 'f': return std::move(parse_bool());
            case 'n': return std::move(parse_null());
            default:
            {
                if ((value <= '9' && value >= '0') || value == '-')
                {
                    return std::move(parse_number());
                }
            }
        }
        std::cerr << "ERROR: parse: Unknown starting character '" << value << "'\n";
        return json();
    }
}
