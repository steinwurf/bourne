// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <fstream>

#include <bourne/detail/parser.hpp>
#include <gtest/gtest.h>

namespace
{
void test_parser(const std::string& json_string, const bourne::json& expected)
{
    std::error_code error;
    EXPECT_EQ(expected.dump(),
              bourne::detail::parser::parse(json_string, error).dump())
        << "Input: '" << json_string << "'";
}
}

TEST(test_parser, test_parse)
{
    test_parser("753 ", bourne::json(753));
    test_parser(" 75..3 ", bourne::json::null());
    test_parser(" 753 ", bourne::json(753));
    test_parser(" 90200.10 ", bourne::json(90200.10));
    test_parser(" 90200..10 ", bourne::json::null());
    test_parser("\"Text String\"", bourne::json("Text String"));
    test_parser("\"you are a \\\"great\\\" agent\\/spy\"",
                bourne::json("you are a \"great\" agent/spy"));
    test_parser("[1, 2,3]", bourne::json::array(1, 2, 3));
    test_parser("{\"value\":3}", bourne::json{"value", 3});

    bourne::json expected_json = {"key1", "value", "key2", true,   "key3",
                                  1234,   "key4",  -42,    "key5", nullptr};

    test_parser("{ \"key1\" : \"value\","
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

    std::error_code error;
    auto json = bourne::detail::parser::parse(buffer.str(), error);

    EXPECT_EQ("László", json["hungarian_name"].to_string());
    EXPECT_EQ("Jørgen", json["danish_name"].to_string());
    EXPECT_EQ("秀英", json["chinese_name"].to_string());
    EXPECT_EQ("John", json["english_name"].to_string());

    EXPECT_EQ("værdi", json["nøgle"].to_string());
    EXPECT_EQ("值", json["键"].to_string());
}

TEST(test_parser, test_parse_out_of_object_structure_elements_error)
{
    auto expected_error =
        bourne::error::parse_found_multiple_unstructured_elements;
    std::error_code error;
    std::string json_string = "{} 1  ";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_object_expected_colon_error)
{
    auto expected_error = bourne::error::parse_object_expected_colon;
    std::error_code error;
    std::string json_string = "{\"bourne\"}";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_object_expected_comma_error)
{
    auto expected_error = bourne::error::parse_object_expected_comma;
    std::error_code error;
    std::string json_string = "{\"bourne\": 1 :}";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_array_expected_comma_or_closing_bracket_error)
{
    auto expected_error =
        bourne::error::parse_array_expected_comma_or_closing_bracket;
    std::error_code error;
    std::string json_string = "{\"bourne\": [0 0}";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_string_expected_hex_char_error)
{
    auto expected_error = bourne::error::parse_string_expected_hex_char;
    std::error_code error;
    std::string json_string = "{\"bourne\": \"\\uG\"}";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_number_expected_number_for_component_error)
{
    auto expected_error =
        bourne::error::parse_number_expected_number_for_component;
    std::error_code error;
    std::string json_string = "{\"bourne\": 1e}";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_number_unexpected_char_error)
{
    auto expected_error = bourne::error::parse_number_unexpected_char;
    std::error_code error;
    std::string json_string = "{\"bourne\": 123a}";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_boolean_expected_true_or_false_error)
{
    auto expected_error = bourne::error::parse_boolean_expected_true_or_false;
    std::error_code error;
    std::string json_string = "{\"bourne\": foo}";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_null_expected_null_error)
{
    auto expected_error = bourne::error::parse_null_expected_null;
    std::error_code error;
    std::string json_string = "{\"bourne\": nut}";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_parse_next_unexpected_char_error)
{
    auto expected_error = bourne::error::parse_next_unexpected_char;
    std::error_code error;
    std::string json_string = "]";
    auto result = bourne::detail::parser::parse(json_string, error);
    EXPECT_EQ(expected_error, error) << error.message();
    ASSERT_EQ(bourne::json::null(), result);
}

TEST(test_parser, test_exception_throw)
{
    std::string json_string = "]";
    bool caught = false;
    try
    {
        auto result = bourne::detail::parser::parse(json_string);
    }
    catch (const std::exception& e)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);
}
