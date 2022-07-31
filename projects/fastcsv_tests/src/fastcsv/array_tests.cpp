#include <fastcsv/fastcsv.hpp>
#include <fastcsv/locator.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv::tests
{

    TEST(array_tests, read)
    {
        // Arrange
        auto str = "1,2,3,4\n5,6,7,8\n"s;
        auto expected = std::vector{ std::array<int, 4ul>{ 1, 2, 3, 4 }, std::array<int, 4ul>{ 5, 6, 7, 8 } };

        // Act
        auto result = read_csv<std::array<int, 4ul>>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i].size(), expected[i].size());

            for (auto j = 0ul; j < result[i].size(); ++j)
            {
                EXPECT_EQ(result[i][j], expected[i][j]);
            }
        }
    }

    TEST(array_tests, write)
    {
        // Arrange
        auto data = std::vector{ std::array<int, 4ul>{ 1, 2, 3, 4 }, std::array<int, 4ul>{ 5, 6, 7, 8 } };

#if defined(FASTCSV_PLATFORM_WIN)
        auto expected = "1,2,3,4\r\n5,6,7,8\r\n"s;
#else
        auto expected = "1,2,3,4\n5,6,7,8\n"s;
#endif

        // Act
        auto result = write_csv(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(array_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector{ std::array<int, 4ul>{ 1, 2, 3, 4 }, std::array<int, 4ul>{ 5, 6, 7, 8 } };

        // Act
        auto csvString = write_csv(expected);
        auto result = read_csv<std::array<int, 4ul>>(csvString, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(array_tests, load_csv)
    {
        // Arrange
        auto expected = std::vector{ std::array<int, 4ul>{ 1, 2, 3, 4 }, std::array<int, 4ul>{ 5, 6, 7, 8 } };

        // Act
        auto result = load_csv<std::array<int, 4ul>>(data_path() / "array.csv", no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv::tests