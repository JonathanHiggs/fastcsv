#include <fastcsv/fastcsv.hpp>
#include <fastcsv/locator.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;
using namespace std::string_view_literals;


namespace fastcsv::detail::tests
{

    TEST(w_csv_parser_tests, parse_line)
    {
        // Arrange
        auto content = L"1,2,3\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"1"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"2"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"3"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_line_n)
    {
        // Arrange
        auto content = L"1,2,3\n4,5,6\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"1"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"2"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"3"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // new line
        assertColumn(parser, L"4"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"5"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"6"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_line_r)
    {
        // Arrange
        auto content = L"1,2,3\r4,5,6\r"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"1"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"2"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"3"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // new line
        assertColumn(parser, L"4"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"5"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"6"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_line_rn)
    {
        // Arrange
        auto content = L"1,2,3\r\n4,5,6\r\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"1"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"2"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"3"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // new line
        assertColumn(parser, L"4"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"5"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"6"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_non_existent_columns)
    {
        // Arrange
        auto content = L"1,2,3\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        // Row 1
        assertColumn(parser, L"1"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"2"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"3"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_column();  // advance to non-existent column
        assertColumn(parser, L""sv, true, false);

        parser.advance_column();  // advance to non-existent column again
        assertColumn(parser, L""sv, true, false);

        // End
        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);

        parser.advance_column();  // move to end of file again
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, skip_row)
    {
        // Arrange
        auto content = L"1,2,3\n4,5,6\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        parser.advance_line();  // skip row
        assertColumn(parser, L"4"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"5"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"6"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_quoted_strings)
    {
        // Arrange
        auto content = L"\"one\",\"two\",\"three\"\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"\"one\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"two\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"three\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_quoted_empty_strings)
    {
        // Arrange
        auto content = L"\"\",\"\",\"\"\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"\"\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_quoted_strings_with_delimiter)
    {
        // Arrange
        auto content = L"\"one,\",\",two\",\"th,ree\"\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"\"one,\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\",two\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"th,ree\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_quoted_strings_with_new_line)
    {
        // Arrange
        auto content = L"\"one\n\",\"\rtwo\",\"th\r\nree\"\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"\"one\n\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"\rtwo\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"th\r\nree\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_multiple_quoted_string_in_column)
    {
        // Arrange
        auto content = L"s\"one\",\"two\"e,s\"three\"e,\"four\"a\"five\"\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"s\"one\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"two\"e"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"s\"three\"e"sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"four\"a\"five\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

    TEST(w_csv_parser_tests, parse_quoted_strings_with_escaped_quote)
    {
        // Arrange
        auto content = L"\"one\\\"\",\"\\\"two\",\"\\\"th\\\"ree\\\"\"\n"sv;

        auto parser = w_csv_parser(content, default_column_delimiter<char>, quote<char>, escape<char>);

        // Helpers
        auto assertColumn = [](w_csv_parser & parser, std::wstring_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, L"\"one\\\"\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"\\\"two\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L"\"\\\"th\\\"ree\\\"\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, L""sv, true, false);

        parser.advance_line();  // move to end of file
        assertColumn(parser, L""sv, true, true);
    }

}  // namespace fastcsv::detail::tests