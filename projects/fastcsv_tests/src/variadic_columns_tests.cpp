#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;
using namespace std::string_view_literals;


namespace fastcsv
{

    TEST(variadic_columns_tests, read)
    {
        // Arrange
        auto str = "1,two,ab\n3,four,cd\n"s;
        auto expected1 = std::vector{ 1, 3 };
        auto expected2 = std::vector{ "two"s, "four"s };
        auto expected3 = std::vector{ "ab"s, "cd"s };

        // Act
        auto [result1, result2, result3] = read_csv<int, std::string, std::string>(str, no_header);

        // Assert
        EXPECT_EQ(result1.size(), expected1.size());
        EXPECT_EQ(result2.size(), expected2.size());
        EXPECT_EQ(result3.size(), expected3.size());

        for (auto i = 0ul; i < result1.size(); ++i)
        {
            EXPECT_EQ(result1[i], expected1[i]);
            EXPECT_EQ(result2[i], expected2[i]);
            EXPECT_EQ(result3[i], expected3[i]);
        }
    }

    TEST(variadic_columns_tests, read_with_headers)
    {
        // Arrange
        auto str = "col1,col2,col3\n1,two,ab\n3,four,cd\n"s;
        auto expected1 = std::vector{ 1, 3 };
        auto expected2 = std::vector{ "two"s, "four"s };
        auto expected3 = std::vector{ "ab"s, "cd"s };

        // Act
        auto [result1, result2, result3] = read_csv<int, std::string, std::string>(str);

        // Assert
        EXPECT_EQ(result1.size(), expected1.size());
        EXPECT_EQ(result2.size(), expected2.size());
        EXPECT_EQ(result3.size(), expected3.size());

        for (auto i = 0ul; i < result1.size(); ++i)
        {
            EXPECT_EQ(result1[i], expected1[i]);
            EXPECT_EQ(result2[i], expected2[i]);
            EXPECT_EQ(result3[i], expected3[i]);
        }
    }

    TEST(variadic_columns_tests, write)
    {
        // Arrange
        auto data1 = std::vector{ 1, 3 };
        auto data2 = std::vector{ "two"s, "four"s };
        auto data3 = std::vector{ "ab"s, "cd"s };
        auto expected = "1,two,ab\n3,four,cd\n"s;

        // Act
        auto result = write_csv(data1, data2, data3);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(variadic_columns_tests, write_with_headers)
    {
        // Arrange
        auto headers = std::vector{ "col1"sv, "col2"sv, "col3"sv };
        auto data1 = std::vector{ 1, 3 };
        auto data2 = std::vector{ "two"s, "four"s };
        auto data3 = std::vector{ "ab"s, "cd"s };
        auto expected = "col1,col2,col3\n1,two,ab\n3,four,cd\n"s;

        // Act
        auto result = write_csv(headers, data1, data2, data3);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(variadic_columns_tests, write_with_missing_headers)
    {
        // Arrange
        auto headers = std::vector{ "col1"sv };
        auto data1 = std::vector{ 1, 3 };
        auto data2 = std::vector{ "two"s, "four"s };
        auto data3 = std::vector{ "ab"s, "cd"s };
        auto expected = "col1,,\n1,two,ab\n3,four,cd\n"s;

        // Act
        auto result = write_csv(headers, data1, data2, data3);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(variadic_columns_tests, write_with_extra_headers)
    {
        // Arrange
        auto headers = std::vector{ "col1"sv, "col2"sv, "col3"sv, "col4"sv, "col5"sv };
        auto data1 = std::vector{ 1, 3 };
        auto data2 = std::vector{ "two"s, "four"s };
        auto data3 = std::vector{ "ab"s, "cd"s };
        auto expected = "col1,col2,col3\n1,two,ab\n3,four,cd\n"s;

        // Act
        auto result = write_csv(headers, data1, data2, data3);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(variadic_columns_tests, round_trip)
    {
        // Arrange
        auto headers = std::vector{ "col1"sv, "col2"sv, "col3"sv };
        auto expected1 = std::vector{ 1, 3 };
        auto expected2 = std::vector{ "two"s, "four"s };
        auto expected3 = std::vector{ "ab"s, "cd"s };

        // Act
        auto csvStringWithHeaders = write_csv(headers, expected1, expected2, expected3);
        auto [intermediate1, intermediate2, intermediate3]
            = read_csv<int, std::string, std::string>(csvStringWithHeaders);
        auto csvString = write_csv(intermediate1, intermediate2, intermediate3);
        auto [result1, result2, result3]
            = read_csv<int, std::string, std::string>(csvString, no_header);

        // Assert
        EXPECT_EQ(result1.size(), expected1.size());
        EXPECT_EQ(result2.size(), expected2.size());
        EXPECT_EQ(result3.size(), expected3.size());

        for (auto i = 0ul; i < result1.size(); ++i)
        {
            EXPECT_EQ(result1[i], expected1[i]);
            EXPECT_EQ(result2[i], expected2[i]);
            EXPECT_EQ(result3[i], expected3[i]);
        }
    }

}  // namespace fastcsv