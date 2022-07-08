#include <fastcsv.hpp>
#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{
    struct Foo
    {
        std::string name;
        int number;
    };

    template <>
    struct from_csv<Foo> : csv_reader
    {
        [[nodiscard]] Foo operator()() const { return Foo{ read<std::string>(), read<int>() }; }
    };

    template <>
    struct to_csv<Foo> : csv_writer
    {
        void operator()(const Foo & value)
        {
            write(value.name);
            write(value.number);
        }
    };


    TEST(SimpleTests, ParseCsv)
    {
        // Arrange
        auto str = "One,1\nTwo,2\nThree,3\n"s;
        auto expected = std::vector<Foo>{ Foo{ "One", 1 }, Foo{ "Two", 2 }, Foo{ "Three", 3 } };

        // Act
        auto result = parse_csv<Foo>(str);

        // Assert
        EXPECT_EQ(result.size(), 3ul);

        EXPECT_EQ(result[0].name, expected[0].name);
        EXPECT_EQ(result[0].number, expected[0].number);

        EXPECT_EQ(result[1].name, expected[1].name);
        EXPECT_EQ(result[1].number, expected[1].number);

        EXPECT_EQ(result[2].name, expected[2].name);
        EXPECT_EQ(result[2].number, expected[2].number);
    }

    TEST(SimpleTests, ToCsvString)
    {
        // Arrange
        auto data = std::vector<Foo>{ Foo{ "One", 1 }, Foo{ "Two", 2 }, Foo{ "Three", 3 } };
        auto expected = "One,1\nTwo,2\nThree,3\n"s;

        // Act
        auto result = to_csv_string(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

}  // namespace fastcsv