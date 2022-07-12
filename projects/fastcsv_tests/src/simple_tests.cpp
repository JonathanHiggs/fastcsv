#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{

    struct simple_struct
    {
        std::string name;
        int number;

        inline FASTCSV_CONSTEXPR bool operator==(const simple_struct & other) const noexcept
        {
            return std::tie(name, number) == std::tie(other.name, other.number);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const simple_struct & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct from_csv<simple_struct> final : csv_reader
    {
        simple_struct operator()() const { return simple_struct{ read<std::string>(), read<int>() }; }
    };

    template <>
    struct to_csv<simple_struct> final : csv_writer
    {
        void operator()(const simple_struct & value)
        {
            write(value.name);
            write(value.number);
        }
    };


    TEST(simple_tests, read)
    {
        // Arrange
        auto str = "One,1\nTwo,2\nThree,3\n"s;
        auto expected = std::vector<simple_struct>{ simple_struct{ "One", 1 }, simple_struct{ "Two", 2 }, simple_struct{ "Three", 3 } };

        // Act
        auto result = read_csv<simple_struct>(str);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(simple_tests, write_string)
    {
        // Arrange
        auto data = std::vector<simple_struct>{ simple_struct{ "One", 1 }, simple_struct{ "Two", 2 }, simple_struct{ "Three", 3 } };
        auto expected = "One,1\nTwo,2\nThree,3\n"s;

        // Act
        auto result = write_csv(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(simple_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<simple_struct>{ simple_struct{ "One", 1 }, simple_struct{ "Two", 2 }, simple_struct{ "Three", 3 } };

        // Act
        auto csvString = write_csv(expected);
        auto result = read_csv<simple_struct>(csvString);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv