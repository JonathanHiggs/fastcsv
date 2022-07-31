#include <fastcsv/fastcsv.hpp>
#include <fastcsv/locator.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv::tests
{

    TEST(string_tests, read_simple_string)
    {
        // Arrange
        auto str = "some string\n"s;

        // Act
        auto result = read_csv<std::string>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 1ul);
        EXPECT_EQ(result[0], "some string"s);
    }

    TEST(string_tests, read_quoted_string)
    {
        // Arrange
        auto str = "\"some string\"\n"s;

        // Act
        auto result = read_csv<std::string>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 1ul);
        EXPECT_EQ(result[0], "some string"s);
    }

    TEST(string_tests, read_two_strings)
    {
        // Arrange
        auto str = "some string,other string\n"s;

        // Act
        auto result = read_csv<std::tuple<std::string, std::string>>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 1ul);
        EXPECT_EQ(std::get<0>(result[0]), "some string"s);
        EXPECT_EQ(std::get<1>(result[0]), "other string"s);
    }

    TEST(string_tests, read_two_quoted_strings)
    {
        // Arrange
        auto str = "\"some string\",\"other string\"\n"s;

        // Act
        auto result = read_csv<std::tuple<std::string, std::string>>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 1ul);
        EXPECT_EQ(std::get<0>(result[0]), "some string"s);
        EXPECT_EQ(std::get<1>(result[0]), "other string"s);
    }

    TEST(string_tests, read_two_quoted_with_commas_strings)
    {
        // Arrange
        auto str = "\"some, string\",\"other, string\"\n"s;

        // Act
        auto result = read_csv<std::tuple<std::string, std::string>>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 1ul);
        EXPECT_EQ(std::get<0>(result[0]), "some, string"s);
        EXPECT_EQ(std::get<1>(result[0]), "other, string"s);
    }

    TEST(string_tests, read_escaped_quoted_string)
    {
        // Arrange
        auto str = "\"\\\"some string\\\"\"\n"s;

        // Act
        auto result = read_csv<std::string>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 1ul);
        EXPECT_EQ(result[0], "\"some string\""s);
    }

}  // namespace fastcsv::tests