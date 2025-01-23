// Copyright (c) Steinwurf ApS 2016.
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <bourne/json.hpp>

#include <iostream>

int main()
{
    // This is how each type can be created:
    // default is null
    bourne::json null;
    bourne::json boolean(true);
    bourne::json string("Raw string");
    bourne::json cpp_string(std::string("C++ string"));
    bourne::json integer(1);
    bourne::json floating_point(1.2);
    bourne::json array = bourne::json::array();
    bourne::json object = bourne::json::object();

    // types can always be overwritten by supported types
    bourne::json value;
    value = false;
    value = "c-string";
    value = 1;
    value = 1.1;
    value = std::string("asd");

    // This is how to append to arrays.
    // By appending to a non-array value, that object will be turned into an
    // array with itself as the the first element.
    array.append(1);
    array.append("test");
    array.append(false);

    // Array elements are accessed with operator[](std::size_t).
    bourne::json array_element = array[0];

    // Arrays can also be intialized with values
    bourne::json::array(2, "test", true);

    // This is how to add values to objects:
    object["key1"] = 1.0;
    object["key2"] = "some value";

    // Object elements are accessed using operator[](std::string).
    bourne::json object_element = object["key2"];

    // Both objects and arrays can be nested
    bourne::json other_object = bourne::json::object();
    other_object["nested"] = object;
    array[0] = other_object;

    // Lets print what we've made
    std::cout << "nested objects and arrays:" << std::endl;
    std::cout << array << std::endl;

    // Another, more json-like syntax, can be used to create json objects.
    std::cout << std::endl << "json like syntax:" << std::endl;

    // clang-format off
    bourne::json json =
        {
            "key1", "Value",
            "key2", true,
            "key3", {
                "nested array", bourne::json::array(
                    "look",
                    "an",
                    "array",
                    6,
                    true,
                    bourne::json({"key", "value"})),
                "nested objects", {
                    "are_cool", true
                }
            }
        };
    // clang-format on

    // This results in the following:
    std::cout << json << std::endl;

    // Dump the data to a string.
    std::string json_dump = json.dump();

    // Parse the dumped json string.
    std::error_code error;
    bourne::json parsed_json = bourne::json::parse(json_dump, error);
    // Use the error code to determine if the parsing was successful.
    assert(!error);

    // Nested elements can be accessed like so:
    std::cout << std::endl << "accessing nested keys" << std::endl;
    std::cout << "json[\"key3\"][\"nested objects\"][\"are_cool\"]: "
              << json["key3"]["nested objects"]["are_cool"].to_bool()
              << std::endl;

    // Elements can be iterated.
    std::cout << std::endl << "iterating arrays:" << std::endl;
    std::size_t index = 0;
    for (auto& v : array.array_range())
    {
        std::cout << "array[" << index++ << "] = " << v << std::endl;
    }

    std::cout << std::endl << "iterating objects:" << std::endl;
    for (auto& v : object.object_range())
    {
        std::cout << "object[" << v.first << "] = " << v.second << std::endl;
    }

    return 0;
}
