#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

#include <variant>

using namespace std::string_literals;


namespace fastcsv
{

    struct intermediate
    {
        std::optional<int> optInt;
        std::optional<std::string> optString;
    };

    template <>
    struct from_csv<intermediate> final : csv_reader
    {
        FASTCSV_NO_DISCARD intermediate operator()() const
        {
            return intermediate{ read_opt<int>(), read_opt<std::string>() };
        }
    };

    template <>
    struct to_csv<intermediate> final : csv_writer
    {
        void operator()(const intermediate & value)
        {
            write(value.optInt);
            write(value.optString);
        }
    };

    struct adapted
    {
        std::variant<int, std::string> data;

        inline FASTCSV_CONSTEXPR bool operator==(const adapted & other) const noexcept { return data == other.data; }

        inline FASTCSV_CONSTEXPR bool operator!=(const adapted & other) const noexcept { return !(*this == other); }
    };

    struct adapter
    {
        inline FASTCSV_CONSTEXPR intermediate operator()(const adapted & value) const noexcept
        {
            return intermediate{
                value.data.index() == 0ul ? std::make_optional(std::get<int>(value.data)) : std::nullopt,
                value.data.index() == 1ul ? std::make_optional(std::get<std::string>(value.data)) : std::nullopt
            };
        }

        inline FASTCSV_CONSTEXPR adapted operator()(intermediate && value) const noexcept
        {
            return adapted{ value.optInt ? std::variant<int, std::string>(*value.optInt)
                                         : std::variant<int, std::string>(*value.optString) };
        }
    };

    TEST(adapter_tests, is_adapter)
    {
        auto result1 = detail::is_adapter_v<adapter, adapted, intermediate>;
        EXPECT_TRUE(result1);

        auto result2 = detail::is_adapter_v<adapter, intermediate, adapted>;
        EXPECT_TRUE(result2);
    }

    TEST(adapter_tests, read_with_adapter)
    {
        // Arrange
        auto str = "1,\n2,\n,three\n,four\n"s;
        auto expected = std::vector{ adapted{ 1 }, adapted{ 2 }, adapted{ "three" }, adapted{ "four" } };

        // Act
        auto result = read_csv<adapted, intermediate>(str, adapter{}, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(adapter_tests, read_with_lambda_adapter)
    {
        // Arrange
        auto str = "1,\n2,\n,three\n,four\n"s;
        auto expected = std::vector{ adapted{ 1 }, adapted{ 2 }, adapted{ "three" }, adapted{ "four" } };

        auto adapter = [](intermediate && value) {
            return adapted{ value.optInt ? std::variant<int, std::string>(*value.optInt)
                                         : std::variant<int, std::string>(*value.optString) };
        };

        // Act
        auto result = read_csv<adapted, intermediate>(str, adapter, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(adapter_tests, write_with_adapter)
    {
        // Arrange
        auto data = std::vector{ adapted{ 1 }, adapted{ 2 }, adapted{ "three" }, adapted{ "four" } };
        auto expected = "1,\n2,\n,three\n,four\n"s;

        // Act
        auto result = write_csv<adapted, intermediate>(data, adapter{});

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(adapter_tests, write_with_lambda_adapter)
    {
        // Arrange
        auto data = std::vector{ adapted{ 1 }, adapted{ 2 }, adapted{ "three" }, adapted{ "four" } };
        auto expected = "1,\n2,\n,three\n,four\n"s;

        auto adapter = [](const adapted & value) {
            return intermediate{
                value.data.index() == 0ul ? std::make_optional(std::get<int>(value.data)) : std::nullopt,
                value.data.index() == 1ul ? std::make_optional(std::get<std::string>(value.data)) : std::nullopt
            };
        };

        // Act
        auto result = write_csv<adapted, intermediate>(data, adapter);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(adapter_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector{ adapted{ 1 }, adapted{ 2 }, adapted{ "three" }, adapted{ "four" } };

        // Act
        auto csvString = write_csv<adapted, intermediate>(expected, adapter{});
        auto result = read_csv<adapted, intermediate>(csvString, adapter{}, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv