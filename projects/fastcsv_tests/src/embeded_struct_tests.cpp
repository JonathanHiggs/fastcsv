#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{
    struct embeded_struct
    {
        std::string name;
        int number;

        inline FASTCSV_CONSTEXPR bool operator==(const embeded_struct & other) const noexcept
        {
            return std::tie(name, number) == std::tie(other.name, other.number);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const embeded_struct & other) const noexcept
        {
            return !(*this == other);
        }
    };

    template <>
    struct from_csv<embeded_struct> final : csv_reader
    {
        embeded_struct operator()() const { return embeded_struct{ read<std::string>(), read<int>() }; }
    };

    template <>
    struct to_csv<embeded_struct> final : csv_writer
    {
        void operator()(const embeded_struct & value)
        {
            write(value.name);
            write(value.number);
        }
    };

    struct composite_struct
    {
        std::string name;
        embeded_struct value;

        inline FASTCSV_CONSTEXPR bool operator==(const composite_struct & other) const noexcept
        {
            return std::tie(name, value) == std::tie(other.name, other.value);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const composite_struct & other) const noexcept
        {
            return !(*this == other);
        }
    };

    template <>
    struct from_csv<composite_struct> : csv_reader
    {
        composite_struct operator()() const { return { read<std::string>(), read<embeded_struct>() }; }
    };

    template <>
    struct to_csv<composite_struct> : csv_writer
    {
        void operator()(const composite_struct & value)
        {
            write(value.name);
            write(value.value);
        }
    };

    TEST(embeded_struct_tests, parse)
    {
        // Arrange
        auto str = "a,one,1\nb,two,2\n"s;
        auto expected = std::vector<composite_struct>{ composite_struct{ "a", embeded_struct{ "one", 1 } },
                                                       composite_struct{ "b", embeded_struct{ "two", 2 } } };

        // Act
        auto result = parse_csv<composite_struct>(str);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(embeded_struct_tests, write_string)
    {
        // Arrange
        auto data = std::vector<composite_struct>{ composite_struct{ "a", embeded_struct{ "one", 1 } },
                                                   composite_struct{ "b", embeded_struct{ "two", 2 } } };
        auto expected = "a,one,1\nb,two,2\n"s;

        // Act
        auto result = to_csv_string(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(embeded_struct_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<composite_struct>{ composite_struct{ "a", embeded_struct{ "one", 1 } },
                                                       composite_struct{ "b", embeded_struct{ "two", 2 } } };

        // Act
        auto csvString = to_csv_string(expected);
        auto result = parse_csv<composite_struct>(csvString);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv