#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{

    TEST(bool_tests, read_true)
    {
        // Arrange
        auto str = "true\nTRUE\nTrue\nt\n"s;

        // Act
        auto result = read_csv<bool>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 4ul);

        for (auto value : result)
        {
            EXPECT_TRUE(value);
        }
    }

    TEST(bool_tests, read_false)
    {
        // Arrange
        auto str = "false\nFALSE\nFalse\nf\n"s;

        // Act
        auto result = read_csv<bool>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), 4ul);

        for (auto value : result)
        {
            EXPECT_FALSE(value);
        }
    }

    TEST(bool_tests, write_string)
    {
        // Arrange
        auto data = std::vector<bool>{ true, false };
        auto expected = "true\nfalse\n"s;

        // Act
        auto result = write_csv(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(bool_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<bool>{ true, false };

        // Act
        auto str = write_csv(expected);
        auto result = read_csv<bool>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv