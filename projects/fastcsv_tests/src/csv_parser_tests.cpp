#include <fastcsv/fastcsv.hpp>

#include <gtest/gtest.h>

using namespace std::string_literals;
using namespace std::string_view_literals;


namespace fastcsv::detail::tests
{

    TEST(csv_parser_tests, parse_row)
    {
        // Arrange
        auto content = "1,2,3\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, "1"sv, false, false);

        parser.advance_column();
        assertColumn(parser, "2"sv, false, false);

        parser.advance_column();
        assertColumn(parser, "3"sv, true, false);

        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);
    }

    TEST(csv_parser_tests, parse_non_existent_columns)
    {
        // Arrange
        auto content = "1,2,3\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        // Row 1
        assertColumn(parser, "1"sv, false, false);

        parser.advance_column();
        assertColumn(parser, "2"sv, false, false);

        parser.advance_column();
        assertColumn(parser, "3"sv, true, false);

        parser.advance_column(); // advance to non-existent column
        assertColumn(parser, ""sv, true, false);

        parser.advance_column(); // advance to non-existent column again
        assertColumn(parser, ""sv, true, false);

        // End
        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);

        parser.advance_column(); // move to end of file again
        assertColumn(parser, ""sv, true, true);
    }

    TEST(csv_parser_tests, skip_row)
    {
        // Arrange
        auto content = "1,2,3\n4,5,6\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        parser.advance_line(); // skip row
        assertColumn(parser, "4"sv, false, false);

        parser.advance_column();
        assertColumn(parser, "5"sv, false, false);

        parser.advance_column();
        assertColumn(parser, "6"sv, true, false);

        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);
    }

    TEST(csv_parser_tests, parse_quoted_strings)
    {
        // Arrange
        auto content = "\"one\",\"two\",\"three\"\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, "\"one\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"two\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"three\""sv, true, false);

        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);
    }

    TEST(csv_parser_tests, parse_quoted_empty_strings)
    {
        // Arrange
        auto content = "\"\",\"\",\"\"\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, "\"\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"\""sv, true, false);

        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);
    }

    TEST(csv_parser_tests, parse_quoted_strings_with_delimiter)
    {
        // Arrange
        auto content = "\"one,\",\",two\",\"th,ree\"\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, "\"one,\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\",two\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"th,ree\""sv, true, false);

        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);
    }

    TEST(csv_parser_tests, parse_quoted_strings_with_new_line)
    {
        // Arrange
        auto content = "\"one\n\",\"\rtwo\",\"th\r\nree\"\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, "\"one\n\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"\rtwo\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"th\r\nree\""sv, true, false);

        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);
    }

    TEST(csv_parser_tests, parse_multiple_quoted_string_in_column)
    {
        // Arrange
        auto content = "s\"one\",\"two\"e,s\"three\"e,\"four\"a\"five\"\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, "s\"one\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"two\"e"sv, false, false);

        parser.advance_column();
        assertColumn(parser, "s\"three\"e"sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"four\"a\"five\""sv, true, false);

        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);
    }

    TEST(csv_parser_tests, parse_quoted_strings_with_escaped_quote)
    {
        // Arrange
        auto content = "\"one\\\"\",\"\\\"two\",\"\\\"th\\\"ree\\\"\"\n"sv;

        auto parser = csv_parser(content, default_column_delimiter, quote_char, escape_char);

        // Helpers
        auto assertColumn = [](csv_parser& parser, std::string_view expected, bool isEndOfLine, bool isEndOfFile) {
            EXPECT_EQ(parser.end_of_line(), isEndOfLine);
            EXPECT_EQ(parser.end_of_file(), isEndOfFile);

            EXPECT_EQ(parser.current_column_size(), expected.size());
            EXPECT_EQ(parser.current_column_empty(), expected.size() == 0ul);

            EXPECT_EQ(parser.current_column(), expected);
        };

        // Act & Assert
        assertColumn(parser, "\"one\\\"\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"\\\"two\""sv, false, false);

        parser.advance_column();
        assertColumn(parser, "\"\\\"th\\\"ree\\\"\""sv, true, false);

        parser.advance_line(); // move to end of file
        assertColumn(parser, ""sv, true, true);
    }

}