#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{
    struct foo
    {
        std::string name;
        int number;

        inline FASTCSV_CONSTEXPR bool operator==(const foo & other) const noexcept
        {
            return std::tie(name, number) == std::tie(other.name, other.number);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const foo & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct from_csv<foo> : csv_reader
    {
        foo operator()() const { return foo{ read<std::string>(), read<int>() }; }
    };

    template <>
    struct to_csv<foo> : csv_writer
    {
        void operator()(const foo & value)
        {
            write(value.name);
            write(value.number);
        }
    };


    TEST(simple_tests, parse)
    {
        // Arrange
        auto str = "One,1\nTwo,2\nThree,3\n"s;
        auto expected = std::vector<foo>{ foo{ "One", 1 }, foo{ "Two", 2 }, foo{ "Three", 3 } };

        // Act
        auto result = parse_csv<foo>(str);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        EXPECT_EQ(result[0], expected[0]);
        EXPECT_EQ(result[1], expected[1]);
        EXPECT_EQ(result[2], expected[2]);
    }

    TEST(simple_tests, write_string)
    {
        // Arrange
        auto data = std::vector<foo>{ foo{ "One", 1 }, foo{ "Two", 2 }, foo{ "Three", 3 } };
        auto expected = "One,1\nTwo,2\nThree,3\n"s;

        // Act
        auto result = to_csv_string(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(simple_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<foo>{ foo{ "One", 1 }, foo{ "Two", 2 }, foo{ "Three", 3 } };

        // Act
        auto csvString = to_csv_string(expected);
        auto result = parse_csv<foo>(csvString);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        EXPECT_EQ(result[0], expected[0]);
        EXPECT_EQ(result[1], expected[1]);
        EXPECT_EQ(result[2], expected[2]);
    }

}  // namespace fastcsv