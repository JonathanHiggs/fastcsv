#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;


namespace fastcsv
{

    struct all_primatives
    {
        bool b;
        char c;
        unsigned char uc;
        short s;
        unsigned short us;
        int i;
        unsigned int ui;
        long l;
        unsigned u;
        unsigned long ul;
        long long ll;
        unsigned long long ull;
        float f;
        double d;
        long double ld;

        inline FASTCSV_CONSTEXPR bool operator==(const all_primatives & other) const noexcept
        {
            return b == other.b && c == other.c && uc == other.uc && s == other.s && us == other.us && i == other.i
                   && ui == other.ui && l == other.l && u == other.u && ul == other.ul && ll == other.ll
                   && ull == other.ull && f == other.f && d == other.d && ld == other.ld;
        }

        inline FASTCSV_CONSTEXPR bool operator!=(const all_primatives & other) const noexcept
        {
            return !(*this == other);
        }
    };

    template <>
    struct from_csv<all_primatives> final : csv_reader
    {
        all_primatives operator()() const
        {
            return all_primatives{ read<bool>(),          read<char>(),           read<unsigned char>(),
                                   read<short>(),         read<unsigned short>(), read<int>(),
                                   read<unsigned int>(),  read<long>(),           read<unsigned>(),
                                   read<unsigned long>(), read<long long>(),      read<unsigned long long>(),
                                   read<float>(),         read<double>(),         read<long double>() };
        }
    };

    template <>
    struct to_csv<all_primatives> final : csv_writer
    {
        void operator()(const all_primatives & value)
        {
            write(value.b);
            write(value.c);
            write(value.uc);
            write(value.s);
            write(value.us);
            write(value.i);
            write(value.ui);
            write(value.l);
            write(value.u);
            write(value.ul);
            write(value.ll);
            write(value.ull);
            write(value.f);
            write(value.d);
            write(value.ld);
        }
    };


    TEST(all_primatives_tests, read)
    {
        // Arrange
        auto str
            = "true,\x8A,241,-32668,63535,-2037483647,4284967295,-2136483647,4284967295,4294937295,-9223371036254775807,18446744072705551615,3.1415927,2.718281828459045,2.718281828459045\n"s;
        auto expected = std::vector<all_primatives>{ all_primatives{
            true,                     // bool
            -118,                     // char
            241,                      // unsigned char
            -32668,                   // short
            63535,                    // unsigned short
            -2037483647,              // int
            4284967295u,              // unsigned int
            -2136483647l,             // long
            4284967295u,              // unsigned
            4294937295ul,             // unsigned long
            -9223371036254775807ll,   // long long
            18446744072705551615ull,  // unsigned long long
            3.1415927f,               // float
            2.718281828459045,        // double
            2.718281828459045         // long double
        } };

        // Act
        auto result = read_csv<all_primatives>(str, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

    TEST(all_primatives_tests, write_string)
    {
        // Arrange
        auto data = std::vector<all_primatives>{ all_primatives{
            true,                     // bool
            -118,                     // char
            241,                      // unsigned char
            -32668,                   // short
            63535,                    // unsigned short
            -2037483647,              // int
            4284967295u,              // unsigned int
            -2136483647l,             // long
            4284967295u,              // unsigned
            4294937295ul,             // unsigned long
            -9223371036254775807ll,   // long long
            18446744072705551615ull,  // unsigned long long
            3.1415927f,               // float
            2.718281828459045,        // double
            2.718281828459045         // long double
        } };

#if defined(FASTCSV_PLATFORM_WIN)
        auto expected
            = "true,\x8A,241,-32668,63535,-2037483647,4284967295,-2136483647,4284967295,4294937295,-9223371036254775807,18446744072705551615,3.1415927,2.718281828459045,2.718281828459045\r\n"s;
#else
        auto expected
            = "true,\x8A,241,-32668,63535,-2037483647,4284967295,-2136483647,4284967295,4294937295,-9223371036254775807,18446744072705551615,3.1415927,2.718281828459045,2.718281828459045\n"s;
#endif

        // Act
        auto result = write_csv(data);

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(all_primatives_tests, round_trip)
    {
        // Arrange
        auto expected = std::vector<all_primatives>{ all_primatives{
            true,                     // bool
            -118,                     // char
            241,                      // unsigned char
            -32668,                   // short
            63535,                    // unsigned short
            -2037483647,              // int
            4284967295u,              // unsigned int
            -2136483647l,             // long
            4284967295u,              // unsigned
            4294937295ul,             // unsigned long
            -9223371036254775807ll,   // long long
            18446744072705551615ull,  // unsigned long long
            3.1415927f,               // float
            2.718281828459045,        // double
            2.718281828459045         // long double
        } };

        // Act
        auto csvString = write_csv(expected);
        auto result = read_csv<all_primatives>(csvString, no_header);

        // Assert
        EXPECT_EQ(result.size(), expected.size());

        for (auto i = 0ul; i < result.size(); ++i)
        {
            EXPECT_EQ(result[i], expected[i]);
        }
    }

}  // namespace fastcsv