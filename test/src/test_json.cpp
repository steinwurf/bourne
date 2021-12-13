// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <bourne/json.hpp>
#include <gtest/gtest.h>

TEST(test_json, test_dump)
{
    bourne::json object({"k1",
                         1,
                         "k2",
                         true,
                         "k3",
                         nullptr,
                         "k4",
                         {"k5", "some string", "some_array",
                          bourne::json::array(9, "some other string", false)}});

    std::string expected =
        "{\n"
        "  \"k1\" : 1,\n"
        "  \"k2\" : true,\n"
        "  \"k3\" : null,\n"
        "  \"k4\" : {\n"
        "    \"k5\" : \"some string\",\n"
        "    \"some_array\" : [9, \"some other string\", false]\n"
        "  }\n"
        "}";

    EXPECT_EQ(expected, object.dump());
}

TEST(test_json, test_dump_min)
{
    bourne::json object({"k1",
                         1,
                         "k2",
                         true,
                         "k3",
                         nullptr,
                         "k4",
                         {"k5", "some string", "some_array",
                          bourne::json::array(9, "some other string", false)}});

    std::string expected = "{\"k1\":1,\"k2\":true,\"k3\":null,\"k4\":{\"k5\":"
                           "\"some string\",\"some_array\":[9,"
                           "\"some other string\",false]}}";

    EXPECT_EQ(expected, object.dump_min());
}
TEST(test_json, test_assignment)
{
    auto object = bourne::json::object();
    object["a"] = true;
    object["b"] = 0;
    object["c"] = 0.0f;
    object["d"] = "";
    object["e"] = nullptr;

    EXPECT_TRUE(object["a"].is_bool());
    EXPECT_TRUE(object["b"].is_int());
    EXPECT_TRUE(object["c"].is_float());
    EXPECT_TRUE(object["d"].is_string());
    EXPECT_TRUE(object["e"].is_null());
}

TEST(test_json, test_retrival_of_primitives)
{
    bourne::json element;

    element = true;
    EXPECT_TRUE(element.is_bool());
    EXPECT_FALSE(element.is_int());
    EXPECT_FALSE(element.is_float());
    EXPECT_FALSE(element.is_string());
    EXPECT_FALSE(element.is_object());
    EXPECT_FALSE(element.is_array());
    EXPECT_FALSE(element.is_null());
    EXPECT_EQ(true, element.to_bool());
    {
        bool result = false;
        element.to(result);
        EXPECT_EQ(element.to_bool(), result);
    }

    element = 42;
    EXPECT_FALSE(element.is_bool());
    EXPECT_TRUE(element.is_int());
    EXPECT_TRUE(element.is_float());
    EXPECT_FALSE(element.is_string());
    EXPECT_FALSE(element.is_object());
    EXPECT_FALSE(element.is_array());
    EXPECT_FALSE(element.is_null());
    EXPECT_EQ(42, element.to_int());
    EXPECT_EQ(42, element.to_float());

    {
        uint32_t result = 0;
        element.to(result);
        EXPECT_EQ(element.to_int(), result);
    }

    {
        float result = 0.0;
        element.to(result);
        EXPECT_FLOAT_EQ(element.to_float(), result);
    }

    element = 13.37;
    EXPECT_FALSE(element.is_bool());
    EXPECT_FALSE(element.is_int());
    EXPECT_TRUE(element.is_float());
    EXPECT_FALSE(element.is_string());
    EXPECT_FALSE(element.is_object());
    EXPECT_FALSE(element.is_array());
    EXPECT_FALSE(element.is_null());
    EXPECT_EQ(13.37, element.to_float());
    {
        float result = 0.0;
        element.to(result);
        EXPECT_FLOAT_EQ(element.to_float(), result);
    }

    element = "test";
    EXPECT_FALSE(element.is_bool());
    EXPECT_FALSE(element.is_int());
    EXPECT_FALSE(element.is_float());
    EXPECT_TRUE(element.is_string());
    EXPECT_FALSE(element.is_object());
    EXPECT_FALSE(element.is_array());
    EXPECT_FALSE(element.is_null());
    EXPECT_EQ("test", element.to_string());
    {
        std::string result = "";
        element.to(result);
        EXPECT_EQ(element.to_string(), result);
    }

    element = bourne::json::object();
    EXPECT_FALSE(element.is_bool());
    EXPECT_FALSE(element.is_int());
    EXPECT_FALSE(element.is_float());
    EXPECT_FALSE(element.is_string());
    EXPECT_TRUE(element.is_object());
    EXPECT_FALSE(element.is_array());
    EXPECT_FALSE(element.is_null());

    element = bourne::json::array();
    EXPECT_FALSE(element.is_bool());
    EXPECT_FALSE(element.is_int());
    EXPECT_FALSE(element.is_float());
    EXPECT_FALSE(element.is_string());
    EXPECT_FALSE(element.is_object());
    EXPECT_TRUE(element.is_array());
    EXPECT_FALSE(element.is_null());
    EXPECT_EQ(bourne::json::array(), element);
}

TEST(test_json, test_array)
{
    bourne::json array;

    array[2] = 2;
    EXPECT_EQ("[null, null, 2]", array.dump());
    array[1] = 1;
    EXPECT_EQ("[null, 1, 2]", array.dump());
    array[0] = 0;
    EXPECT_EQ("[0, 1, 2]", array.dump());
    array[3] = 3;
    EXPECT_EQ("[0, 1, 2, 3]", array.dump());
}

TEST(test_json, test_nested_array)
{
    bourne::json array;

    array[2][0][1] = true;
    EXPECT_EQ("[null, null, [[null, true]]]", array.dump());
}

TEST(test_json, test_iterator)
{
    std::vector<uint32_t> expected = {100, 12, 123, 56, 53, 4, 356, 6, 76, 5};

    std::size_t index = 0;

    auto array = bourne::json(bourne::class_type::array);
    auto object = bourne::json(bourne::class_type::object);
    for (auto expect : expected)
    {
        array[index] = expect;
        object[std::to_string(index)] = expect;
        index++;
    }

    index = 0;
    for (auto& v : array.array_range())
    {
        EXPECT_EQ(expected[index++], v.to_int());
    }
    EXPECT_EQ(expected.size(), index);

    index = 0;
    for (auto& v : object.object_range())
    {
        EXPECT_EQ(std::to_string(index), v.first);
        EXPECT_EQ(expected[index], v.second.to_int());
        index++;
    }
    EXPECT_EQ(expected.size(), index);
}

TEST(test_json, test_equality)
{
    // Test null equality
    EXPECT_EQ(bourne::json(), bourne::json());

    bourne::json object1({"k1",
                          1,
                          "k2",
                          true,
                          "k3",
                          nullptr,
                          "k4",
                          {"k5", "some string", "some_array",
                           bourne::json::array(9, "some other string", false)},
                          "none",
                          bourne::json()});

    bourne::json object2({"k2",
                          true,
                          "k4",
                          {"k5", "some string", "some_array",
                           bourne::json::array(9, "some other string", false)},
                          "k3",
                          nullptr,
                          "k1",
                          1,
                          "none",
                          bourne::json()});

    // check equality
    EXPECT_EQ(object1, object2);
    EXPECT_EQ(object2, object1);

    object1["different"] = true;

    // check inequality (object 1 is different)
    EXPECT_NE(object1, object2);

    object2["different"] = true;

    // check that object 2 is different too
    EXPECT_EQ(object1, object2);

    auto object3 = object2;

    EXPECT_EQ(object1, object3);
}

TEST(test_json, test_const_retrival)
{
    auto obj = bourne::json::object();
    obj["test"] = 4;
    auto const_retrival = [](const bourne::json& json)
    {
        ASSERT_TRUE(json.has_key("test"));
        ASSERT_TRUE(json["test"].is_int());
        EXPECT_EQ(4, json["test"].to_int());
    };
    const_retrival(obj);
}

TEST(test_json, test_keys)
{
    auto obj = bourne::json::object();
    obj["key1"] = 1;
    obj["key2"] = 2;
    obj["key3"] = 3;
    obj["key4"] = 4;

    for (const auto& key : obj.keys())
    {
        EXPECT_TRUE(obj.has_key(key));
    }
    EXPECT_EQ(4U, obj.keys().size());
}
