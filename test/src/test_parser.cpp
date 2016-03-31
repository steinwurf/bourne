// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <bourne/parser.hpp>
#include <gtest/gtest.h>

namespace
{
    void test_parser(const std::string& json_string, const bourne::json& expected)
    {
        bourne::parser p(json_string);
        EXPECT_EQ(expected.dump(), p.parse().dump());
    }
}

TEST(test_parser, test_parse)
{
    test_parser(" 753 ", bourne::json(753));
    test_parser(" 90200.10 ", bourne::json(90200.10));
    test_parser("\"Text String\"", bourne::json("Text String"));
    test_parser(
        "\"you are a \\\"great\\\" agent\\/spy\"",
        bourne::json("you are a \"great\" agent/spy"));
    test_parser("[1, 2,3]", bourne::json(1, 2, 3));
    test_parser(
        "{ \"key1\" : \"value\","
        "  \"key2\" : true, "
        "  \"key3\" : 1234, "
        "  \"key4\" : null }",
        bourne::json(
        {
            "key1", "value",
            "key2", true,
            "key3", 1234,
            "key4", nullptr
        }
    ));
}
