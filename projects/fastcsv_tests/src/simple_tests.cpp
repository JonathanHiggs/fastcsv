#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{

    struct simple
    {
        std::string name;
        int number;

        inline FASTCSV_CONSTEXPR bool operator==(const simple & other) const noexcept
        {
            return std::tie(name, number) == std::tie(other.name, other.number);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const simple & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct from_csv<simple> final : csv_reader
    {
        simple operator()() const { return simple{ read<std::string>(), read<int>() }; }
    };

    template <>
    struct to_csv<simple> final : csv_writer
    {
        void operator()(const simple & value)
        {
            write(value.name);
            write(value.number);
        }
    };


    TEST(simple_tests, read)
    {
        // Arrange
        auto str = "One,1\nTwo,2\nThree,3\n"s;
        auto expected = std::vector<simple>{ simple{ "One", 1 }, simple{ "Two", 2 }, simple{ "Three", 3 } };

        // Act
        auto result = read_csv<simple>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(simple_tests, write)
    {
        // Arrange
        auto data = std::vector<simple>{ simple{ "One", 1 }, simple{ "Two", 2 }, simple{ "Three", 3 } };

#if defined(FASTCSV_PLATFORM_WIN)
        auto expected = "One,1\r\nTwo,2\r\nThree,3\r\n"s;
#else
        auto expected = "One,1\nTwo,2\nThree,3\n"s;
#endif

        // Act
        auto result = write_csv(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(simple_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<simple>{ simple{ "One", 1 }, simple{ "Two", 2 }, simple{ "Three", 3 } };

        // Act
        auto csvString = write_csv(expected);
        auto result = read_csv<simple>(csvString, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv