#include <fastcsv/fastcsv.hpp>
#include <fastcsv/locator.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{

    struct embeded
    {
        std::string name;
        int number;

        inline FASTCSV_CONSTEXPR bool operator==(const embeded & other) const noexcept
        {
            return std::tie(name, number) == std::tie(other.name, other.number);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const embeded & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct from_csv<embeded> final : csv_reader
    {
        embeded operator()() const { return embeded{ read<std::string>(), read<int>() }; }
    };

    template <>
    struct to_csv<embeded> final : csv_writer
    {
        void operator()(const embeded & value)
        {
            write(value.name);
            write(value.number);
        }
    };

    struct composite
    {
        std::string name;
        embeded value;

        inline FASTCSV_CONSTEXPR bool operator==(const composite & other) const noexcept
        {
            return std::tie(name, value) == std::tie(other.name, other.value);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const composite & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct from_csv<composite> : csv_reader
    {
        composite operator()() const { return { read<std::string>(), read<embeded>() }; }
    };

    template <>
    struct to_csv<composite> : csv_writer
    {
        void operator()(const composite & value)
        {
            write(value.name);
            write(value.value);
        }
    };

}  // namespace fastcsv

namespace fastcsv::tests
{

    TEST(embeded_struct_tests, read)
    {
        // Arrange
        auto str = "a,one,1\nb,two,2\n"s;
        auto expected
            = std::vector<composite>{ composite{ "a", embeded{ "one", 1 } }, composite{ "b", embeded{ "two", 2 } } };

        // Act
        auto result = read_csv<composite>(str, no_header);

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
        auto data
            = std::vector<composite>{ composite{ "a", embeded{ "one", 1 } }, composite{ "b", embeded{ "two", 2 } } };

#if defined(FASTCSV_PLATFORM_WIN)
        auto expected = "a,one,1\r\nb,two,2\r\n"s;
#else
        auto expected = "a,one,1\nb,two,2\n"s;
#endif

        // Act
        auto result = write_csv(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(embeded_struct_tests, round_trip)
    {
        // Arrange
        auto expected
            = std::vector<composite>{ composite{ "a", embeded{ "one", 1 } }, composite{ "b", embeded{ "two", 2 } } };

        // Act
        auto csvString = write_csv(expected);
        auto result = read_csv<composite>(csvString, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(embeded_struct_tests, load_csv)
    {
        // Arrange
        auto expected
            = std::vector<composite>{ composite{ "a", embeded{ "one", 1 } }, composite{ "b", embeded{ "two", 2 } } };

        // Act
        auto result = load_csv<composite>(data_path() / "embeded-struct.csv", no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv::tests