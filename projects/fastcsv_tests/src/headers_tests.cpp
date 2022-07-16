#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;
using namespace std::string_view_literals;


namespace fastcsv
{

    struct headers
    {
        int val1;
        std::string val2;

        inline FASTCSV_CONSTEXPR bool operator==(const headers & other) const noexcept
        {
            return std::tie(val1, val2) == std::tie(other.val1, other.val2);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const headers & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct csv_headers<headers> final
    {
        FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR std::vector<std::string_view> operator()() const noexcept
        {
            return std::vector<std::string_view>{ "val1"sv, "val2"sv };
        }
    };

    template <>
    struct from_csv<headers> final : csv_reader
    {
        FASTCSV_NO_DISCARD headers operator()() const { return headers{ read<int>(), read<std::string>() }; }
    };

    template <>
    struct to_csv<headers> final : csv_writer
    {
        void operator()(const headers & value)
        {
            write(value.val1);
            write(value.val2);
        }
    };


    TEST(headers_tests, has_csv_headers_is_true)
    {
        auto result = has_csv_headers<headers>;
        EXPECT_TRUE(result);
    }

    TEST(headers_tests, read)
    {
        // Arrange
        auto str = "1,two\n3,four\n"s;
        auto expected = std::vector<headers>{ headers{ 1, "two" }, headers{ 3, "four" } };

        // Act
        auto result = read_csv<headers>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(headers_tests, read_with_headers)
    {
        // Arrange
        auto str = "val1,val2\n1,two\n3,four\n"s;
        auto expected = std::vector<headers>{ headers{ 1, "two" }, headers{ 3, "four" } };

        // Act
        auto result = read_csv<headers>(str);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(headers_tests, write)
    {
        // Arrange
        auto data = std::vector<headers>{ headers{ 1, "two" }, headers{ 3, "four" } };
        auto expected = "1,two\n3,four\n"s;

        // Act
        auto result = write_csv(data, no_header);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(headers_tests, write_headers)
    {
        // Arrange
        auto data = std::vector<headers>{ headers{ 1, "two" }, headers{ 3, "four" } };
        auto expected = "val1,val2\n1,two\n3,four\n"s;

        // Act
        auto result = write_csv(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(headers_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<headers>{ headers{ 1, "two" }, headers{ 3, "four" } };

        // Act
        auto csvStringWithHeaders = write_csv(expected);
        auto result1 = read_csv<headers>(csvStringWithHeaders);
        auto csvString = write_csv(result1, no_header);
        auto result2 = read_csv<headers>(csvString, no_header);

        // Assert
        EXPECT_EQ(result2.size(), expected.size());

        for (auto i = 0ul; i < result2.size(); ++i)
        {
            EXPECT_EQ(result2[i], expected[i]);
        }
    }

}  // namespace fastcsv