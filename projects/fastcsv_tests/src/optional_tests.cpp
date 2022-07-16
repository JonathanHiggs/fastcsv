#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{

    struct optionals
    {
        std::string name;
        std::optional<int> opt_int;
        std::optional<std::string> opt_str;

        inline FASTCSV_CONSTEXPR bool operator==(const optionals & other) const noexcept
        {
            return std::tie(name, opt_int, opt_str) == std::tie(other.name, other.opt_int, other.opt_str);
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const optionals & other) const noexcept { return !(*this == other); }
    };

    template <>
    struct from_csv<optionals> final : csv_reader
    {
        optionals operator()() const
        {
            return optionals{ read<std::string>(), read_opt<int>(), read_opt<std::string>() };
        }
    };

    template <>
    struct to_csv<optionals> final : csv_writer
    {
        void operator()(const optionals & value)
        {
            write(value.name);
            write(value.opt_int);
            write(value.opt_str);
        }
    };


    TEST(optional_tests, read)
    {
        // Arrange
        auto str = "all-values,1,str\nno-values,,\nint-values,1,\nstr-values,,str\n"s;
        auto expected = std::vector<optionals>{ optionals{ "all-values", 1, "str" },
                                                optionals{ "no-values", std::nullopt, std::nullopt },
                                                optionals{ "int-values", 1, std::nullopt },
                                                optionals{ "str-values", std::nullopt, "str" } };

        // Act
        auto result = read_csv<optionals>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(optional_tests, write_string)
    {
        // Arrange
        auto data = std::vector<optionals>{ optionals{ "all-values", 1, "str" },
                                            optionals{ "no-values", std::nullopt, std::nullopt },
                                            optionals{ "int-values", 1, std::nullopt },
                                            optionals{ "str-values", std::nullopt, "str" } };
        auto expected = "all-values,1,str\nno-values,,\nint-values,1,\nstr-values,,str\n"s;

        // Act
        auto result = write_csv(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(optional_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<optionals>{ optionals{ "all-values", 1, "str" },
                                                optionals{ "no-values", std::nullopt, std::nullopt },
                                                optionals{ "int-values", 1, std::nullopt },
                                                optionals{ "str-values", std::nullopt, "str" } };

        // Act
        auto csvString = write_csv(expected);
        auto result = read_csv<optionals>(csvString, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv