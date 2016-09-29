// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <fstream>

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
    test_parser("[1, 2,3]", bourne::json::array(1, 2, 3));

    bourne::json expected_json =
        {
            "key1", "value",
            "key2", true,
            "key3", 1234,
            "key4", -42,
            "key5", nullptr
        };
    
    test_parser(
        "{ \"key1\" : \"value\","
        "  \"key2\" : true, "
        "  \"key3\" : 1234, "
        "  \"key4\" : -42, "
        "  \"key5\" : null }",
        expected_json);
}

TEST(test_parser, test_parse_file)
{
    std::ifstream test_json("test.json");
    EXPECT_TRUE(test_json.is_open());

    std::stringstream buffer;
    buffer << test_json.rdbuf();

    bourne::parser p(buffer.str());
    auto json = p.parse();

    EXPECT_EQ("László", json["hungarian_name"].to_string());
    EXPECT_EQ("Jørgen", json["danish_name"].to_string());
    EXPECT_EQ("秀英", json["chinese_name"].to_string());
    EXPECT_EQ("John", json["english_name"].to_string());

    EXPECT_EQ("værdi", json["nøgle"].to_string());
    EXPECT_EQ("值", json["键"].to_string());
}
