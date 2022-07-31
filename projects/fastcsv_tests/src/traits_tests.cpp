#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>


namespace fastcsv::tests
{
    namespace
    {
        struct Unmapped {};
    }

    TEST(csv_traits_tests, has_from_csv)
    {
        EXPECT_TRUE(fastcsv::has_from_csv<char>);
        EXPECT_TRUE(fastcsv::has_from_csv<int>);
        EXPECT_TRUE(fastcsv::has_from_csv<std::string>);

        EXPECT_FALSE(fastcsv::has_from_csv<Unmapped>);
    }

    TEST(csv_traits_tests, has_to_csv)
    {
        EXPECT_TRUE(fastcsv::has_to_csv<char>);
        EXPECT_TRUE(fastcsv::has_to_csv<int>);
        EXPECT_TRUE(fastcsv::has_to_csv<std::string>);

        EXPECT_FALSE(fastcsv::has_to_csv<Unmapped>);
    }

}  // namespace fastcsv::tests