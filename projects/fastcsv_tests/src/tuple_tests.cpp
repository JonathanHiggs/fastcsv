#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{

    TEST(tuple_tests, read)
    {
        // Arrange
        auto str = "1,one\n2,two\n"s;
        auto expected
            = std::vector<std::tuple<int, std::string>>{ std::make_tuple(1, "one"), std::make_tuple(2, "two") };

        // Act
        auto result = read_csv<std::tuple<int, std::string>>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 2ul);

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(tuple_tests, write)
    {
        // Arrange
        auto expected = "1,one\n2,two\n"s;
        auto data = std::vector<std::tuple<int, std::string>>{ std::make_tuple(1, "one"), std::make_tuple(2, "two") };

        // Act
        auto result = write_csv<std::tuple<int, std::string>>(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(tuple_tests, round_trip)
    {
        // Arrange
        auto expected
            = std::vector<std::tuple<int, std::string>>{ std::make_tuple(1, "one"), std::make_tuple(2, "two") };

        // Act
        auto csvString = write_csv<std::tuple<int, std::string>>(expected);
        auto result = read_csv<std::tuple<int, std::string>>(csvString, no_header);

        // Assert
        EXPECT_EQ(result.size(), 2ul);

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(tuple_tests, read_repeated_type)
    {
        // Arrange
        auto str = "1,one\n2,two\n"s;
        auto expected = std::vector<std::tuple<std::string, std::string>>{ std::make_tuple("1", "one"),
                                                                           std::make_tuple("2", "two") };

        // Act
        auto result = read_csv<std::tuple<std::string, std::string>>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 2ul);

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(tuple_tests, write_repeated_type)
    {
        // Arrange
        auto expected = "1,one\n2,two\n"s;
        auto data = std::vector<std::tuple<std::string, std::string>>{ std::make_tuple("1", "one"),
                                                                       std::make_tuple("2", "two") };

        // Act
        auto result = write_csv<std::tuple<std::string, std::string>>(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(tuple_tests, round_trip_repeated_type)
    {
        // Arrange
        auto expected = std::vector<std::tuple<std::string, std::string>>{ std::make_tuple("1", "one"),
                                                                           std::make_tuple("2", "two") };

        // Act
        auto csvString = write_csv<std::tuple<std::string, std::string>>(expected);
        auto result = read_csv<std::tuple<std::string, std::string>>(csvString, no_header);

        // Assert
        EXPECT_EQ(result.size(), 2ul);

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv