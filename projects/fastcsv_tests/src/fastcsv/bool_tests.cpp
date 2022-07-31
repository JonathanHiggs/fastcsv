#include <fastcsv/fastcsv.hpp>
#include <fastcsv/locator.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv::tests
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

#if defined(FASTCSV_PLATFORM_WIN)
        auto expected = "true\r\nfalse\r\n"s;
#else
        auto expected = "true\nfalse\n"s;
#endif

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

    TEST(bool_tests, load_csv)
    {
        // Arrange
        auto expected = std::vector<bool>{ true, false };

        // Act
        auto result = load_csv<bool>(data_path() / "bool.csv", no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv::tests