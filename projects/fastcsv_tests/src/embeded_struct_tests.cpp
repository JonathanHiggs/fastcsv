#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{
    struct baz
    {
        std::string name;
        int number;

        inline FASTCSV_CONSTEXPR bool operator==(const baz & other) const noexcept
        {
            return std::tie(name, number) == std::tie(other.name, other.number);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const baz & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct from_csv<baz> : csv_reader
    {
        baz operator()() const { return baz{ read<std::string>(), read<int>() }; }
    };

    template <>
    struct to_csv<baz> : csv_writer
    {
        void operator()(const baz & value)
        {
            write(value.name);
            write(value.number);
        }
    };

    struct bar
    {
        std::string name;
        baz value;

        inline FASTCSV_CONSTEXPR bool operator==(const bar & other) const noexcept
        {
            return std::tie(name, value) == std::tie(other.name, other.value);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const bar & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct from_csv<bar> : csv_reader
    {
        bar operator()() const { return { read<std::string>(), read<baz>() }; }
    };

    template <>
    struct to_csv<bar> : csv_writer
    {
        void operator()(const bar & value)
        {
            write(value.name);
            write(value.value);
        }
    };

    TEST(embeded_struct_tests, parse)
    {
        // Arrange
        auto str = "a,one,1\nb,two,2\n"s;
        auto expected = std::vector<bar>{ bar{ "a", baz{ "one", 1 } }, bar{ "b", baz{ "two", 2 } } };

        // Act
        auto result = parse_csv<bar>(str);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        EXPECT_EQ(result[0], expected[0]);
        EXPECT_EQ(result[1], expected[1]);
    }

    TEST(embeded_struct_tests, write_string)
    {
        // Arrange
        auto data = std::vector<bar>{ bar{ "a", baz{ "one", 1 } }, bar{ "b", baz{ "two", 2 } } };
        auto expected = "a,one,1\nb,two,2\n"s;

        // Act
        auto result = to_csv_string(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(embeded_struct_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<bar>{ bar{ "a", baz{ "one", 1 } }, bar{ "b", baz{ "two", 2 } } };

        // Act
        auto csvString = to_csv_string(expected);
        auto result = parse_csv<bar>(csvString);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        EXPECT_EQ(result[0], expected[0]);
        EXPECT_EQ(result[1], expected[1]);
    }

}  // namespace fastcsv