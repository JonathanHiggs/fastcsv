#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

#include <chrono>

using namespace std::string_literals;


namespace fastcsv
{

    TEST(string_view_stream_tests, parse_year_month_data)
    {
        // Arrange
        std::string_view v = "2019-12-04";
        std::string_view fmt = "%Y-%m-%d";
        std::chrono::year_month_day result;

        // Act
        auto in = detail::string_view_stream(v);
        std::chrono::from_stream<char>(in, fmt.data(), result);

        // Assert
        EXPECT_EQ((result.year().operator int()), 2019);
        EXPECT_EQ((result.month().operator unsigned int()), 12u);
        EXPECT_EQ((result.day().operator unsigned int()), 4u);
    }

}