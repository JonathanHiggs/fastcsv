#pragma once


// https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html

// Detect the host platform
#if defined(__linux__)
    #define FASTCSV_PLATFORM_LINUX
#elif (defined(_WIN32) || defined(_WIN64))
    #define FASTCSV_PLATFORM_WIN
#else
    #error "Platform not supported"
#endif

// Detect the compiler
#if (defined(_MSC_VER) && !defined(__clang))
    #define FASTCSV_COMPILER_MSVC
    #define FASTCSV_COMPILER_MSVC_VERSION _MSC_VER
#elif defined(__GNUC__)
    #define FASTCSV_COMPILER_GCC
    #define FASTCSV_COMPILER_GCC_MAJOR __GNUC__
    #define FASTCSV_COMPILER_GCC_MINOR __GNUC_MINOR__
#elif defined(__clang__)
    #define FASTCSV_COMPILER_CLANG
#else
    #error "Compiler not supported"
#endif

// Detect the standard version
#if !defined(FASTCSV_HAS_CXX17)
    #if defined(_MSVC_LANG)
        #define FASTCSV_STD_LANG _MSVC_LANG
    #elif defined(__cplusplus)
        #define FASTCSV_STD_LANG __cplusplus
    #else
        #define FASTCSV_STD_LANG 0L
    #endif  // ^^^ no C++ support ^^^

    #if defined(FASTCSV_COMPILER_GCC)
        #define FASTCSV_CXX20_VERSION 201709L
    #else
        #define FASTCSV_CXX20_VERSION 201703L
    #endif

    #if FASTCSV_STD_LANG > FASTCSV_CXX20_VERSION
        #define FASTCSV_HAS_CXX20
    #endif
    #if FASTCSV_STD_LANG > 201402L
        #define FASTCSV_HAS_CXX17
    #endif

    #undef FASTCSV_CXX20_VERSION
    #undef FASTCSV_STD_LANG
#endif

#ifndef FASTCSV_CONSTEXPR
    #if defined(FASTCSV_HAS_CXX17)
        #define FASTCSV_CONSTEXPR constexpr
    #else
        #define FASTCSV_CONSTEXPR
    #endif
#endif

#ifndef FASTCSV_NO_DISCARD
    #if defined(FASTCSV_HAS_CXX17)
        #define FASTCSV_NO_DISCARD [[nodiscard]]
    #else
        #define FASTCSV_NO_DISCARD
    #endif
#endif

#if defined(FASTCSV_HAS_CXX17) && (!defined(FASTCSV_COMPILER_GCC) || FASTCSV_COMPILER_GCC_MAJOR > 9)
    #define FASTCSV_HAS_FROM_CHAR
#endif


#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>

#include <array>
#if defined(FASTCSV_HAS_FROM_CHAR)
    #include <charconv>
#endif
#if defined(FASTCSV_HAS_CXX20)
    #include <chrono>
#endif
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>


using namespace std::string_view_literals;


namespace fastcsv
{

    template <typename T, typename = void, typename TElem = char, typename TTraits = std::char_traits<TElem>>
    struct from_csv;

    template <typename T, typename = void, typename TElem = char, typename TTraits = std::char_traits<char>>
    struct to_csv;

    template <typename T>
    struct csv_headers;

    template <typename T, typename = void>
    inline constexpr bool has_from_csv = false;

    template <typename T>
    inline constexpr bool has_from_csv<T, decltype(void(sizeof(from_csv<T>)))> = true;

    template <typename T, typename = void>
    inline constexpr bool has_to_csv = false;

    template <typename T>
    inline constexpr bool has_to_csv<T, decltype(void(sizeof(to_csv<T>)))> = true;

    template <typename T, typename = void>
    inline constexpr bool has_csv_headers = false;

    template <typename T>
    inline constexpr bool has_csv_headers<
        T,
        std::enable_if_t<
            std::is_same_v<decltype(std::declval<csv_headers<T>>()()), std::vector<std::string_view>>>> = true;


    namespace detail
    {

        // clang-format off
        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr std::basic_string_view<TElem, TTraits> csv_extension;

        template <> inline constexpr std::string_view csv_extension<char> = ".csv"sv;
        template <> inline constexpr std::wstring_view csv_extension<wchar_t> = LR"(.csv)";


        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr TElem quote;

        template <> inline constexpr char quote<char> = '"';
        template <> inline constexpr wchar_t quote<wchar_t> = L'"';


        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr TElem escape;

        template <> inline constexpr char escape<char> = '\\';
        template <> inline constexpr wchar_t escape<wchar_t> = L'\\';


        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr TElem default_column_delimiter;

        template <> inline constexpr char default_column_delimiter<char> = ',';
        template <> inline constexpr wchar_t default_column_delimiter<wchar_t> = L',';


        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr TElem new_line;

        template <> inline constexpr char new_line<char> = '\n';
        template <> inline constexpr wchar_t new_line<wchar_t> = L'\n';


        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr TElem caridge_return;

        template <> inline constexpr char caridge_return<char> = '\r';
        template <> inline constexpr wchar_t caridge_return<wchar_t> = L'\r';


        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr std::basic_string_view<TElem, TTraits> caridge_return_new_line;

        template <> inline constexpr std::string_view caridge_return_new_line<char> = "\r\n"sv;
        template <> inline constexpr std::wstring_view caridge_return_new_line<wchar_t> = LR"(\r\n)";


#if defined(FASTCSV_PLATFORM_WIN)
        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr std::basic_string_view<TElem, TTraits> default_line_delimiter = caridge_return_new_line<TElem, TTraits>;
#else
        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        inline constexpr std::basic_string_view<TElem, TTraits> default_line_delimiter;

        template <>
        inline constexpr std::basic_string_view<char, std::char_traits<char>> default_line_delimiter<char, std::char_traits<char>> = "\n"sv;

        template <>
        inline constexpr std::basic_string_view<wchar_t, std::char_traits<wchar_t>> default_line_delimiter<wchar_t, std::char_traits<wchar_t>> = LR"(\n)";
#endif
        // clang-format on

        template <typename TElem, typename TTraits = std::char_traits<TElem>>
        class basic_csv_parser final
        {
        private:
            std::basic_string_view<TElem, TTraits> content_;
            TElem columnDelimiter_;
            TElem stringDelimiter_;
            TElem escapeChar_;
            size_t lineStart_;
            size_t columnStart_;
            size_t columnEnd_;

        public:
            FASTCSV_CONSTEXPR basic_csv_parser(
                std::basic_string_view<TElem, TTraits> content,
                TElem columnDelimiter = default_column_delimiter<TElem>,
                TElem stringDelimiter = quote<TElem>,
                TElem escapeChar = escape<TElem>) noexcept
              : content_(content)
              , columnDelimiter_(columnDelimiter)
              , stringDelimiter_(stringDelimiter)
              , escapeChar_(escapeChar)
              , lineStart_(0ul)
              , columnStart_(0ul)
              , columnEnd_(find_column_end(columnStart_))
            { }

            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR bool end_of_line() const
            {
                return columnStart_ == content_.size()
                       || content_[columnStart_] == new_line<TElem> || content_[columnStart_] == caridge_return<TElem>;
            }

            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR bool is_final_column() const
            {
                return columnStart_ == content_.size()
                       || content_[columnEnd_] == new_line<TElem> || content_[columnEnd_] == caridge_return<TElem>;
            }

            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR bool end_of_file() const { return lineStart_ == content_.size(); }

            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR size_t current_column_size() const { return columnEnd_ - columnStart_; }

            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR bool current_column_empty() const { return current_column_size() == 0ul; }

            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR std::basic_string_view<TElem, TTraits> current_column() const
            {
                if (columnStart_ == content_.size()) { return std::basic_string_view<TElem, TTraits>(); }

                return std::basic_string_view<TElem, TTraits>(
                    content_.data() + columnStart_,
                    columnEnd_ - columnStart_);
            }

            FASTCSV_CONSTEXPR bool advance_column()
            {
                if (columnEnd_ == content_.size()
                    || content_[columnEnd_] == new_line<TElem> || content_[columnEnd_] == caridge_return<TElem>)
                {
                    columnStart_ = columnEnd_;
                    return false;
                }

                columnStart_ = std::min(columnEnd_ + 1ul, content_.size());
                columnEnd_ = find_column_end(columnStart_);
                return true;
            }

            FASTCSV_CONSTEXPR bool advance_line()
            {
                while (advance_column()) { }

                if (columnStart_ == content_.size())
                {
                    lineStart_ = columnStart_ = columnEnd_ = content_.size();
                    return false;
                }

                const auto advanceChars
                    = 1ul
                      + static_cast<size_t>(
                          content_[columnEnd_] == caridge_return<TElem> && columnEnd_ + 1ul < content_.size()
                          && content_[columnEnd_ + 1ul] == new_line<TElem>);

                lineStart_ = columnStart_ = std::min(columnEnd_ + advanceChars, content_.size());
                columnEnd_ = find_column_end(columnStart_);

                return true;
            }

            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR std::basic_string_view<TElem, TTraits> consume_column()
            {
                auto value = current_column();
                advance_column();
                return value;
            }

        private:
            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR size_t find_column_end(size_t pos) const
            {
                if (pos >= content_.size()) { return content_.size(); }

                auto current = content_[pos];

                while (pos < content_.size())
                {
                    if (current == columnDelimiter_) { return pos; }

                    // quoted strings
                    else if (current == stringDelimiter_)
                    {
                        ++pos;
                        if (pos == content_.size()) { return pos; }
                        current = content_[pos];
                        bool stringFinished = false;

                        while (!stringFinished && pos < content_.size())
                        {
                            if (current == stringDelimiter_)
                            {
                                // quoted string end
                                ++pos;
                                stringFinished = true;
                            }
                            else if (current == escapeChar_)
                            {
                                // advance two chars if next is an escaped quote
                                pos += 1 + bool(pos + 1ul < content_.size() && content_[pos + 1ul] == stringDelimiter_);
                                current = content_[pos];
                            }
                            else
                            {
                                // advance
                                ++pos;
                            }

                            if (pos == content_.size()) { return pos; }
                            current = content_[pos];
                        }
                    }
                    else
                    {
                        switch (current)
                        {
                        // line of end
                        case new_line<TElem>:
                        case caridge_return<TElem>: return pos;

                        // advance
                        default: {
                            ++pos;
                            if (pos == content_.size()) { return pos; }
                            current = content_[pos];
                        }
                        break;
                        }
                    }
                }

                return pos;
            }
        };

        using csv_parser = basic_csv_parser<char>;
        using w_csv_parser = basic_csv_parser<wchar_t>;


        template <typename T, typename = void>
        inline constexpr bool has_from_chars_integral_v = false;

        template <typename T>
        inline constexpr bool has_from_chars_integral_v<
            T,
            std::void_t<decltype(std::from_chars(
                std::declval<const char *>(),
                std::declval<const char *>(),
                std::declval<T &>(),
                std::declval<const int>()))>> = true;

        template <typename T, typename = void>
        inline constexpr bool has_from_chars_floating_point_v = false;

        template <typename T>
        inline constexpr bool has_from_chars_floating_point_v<
            T,
            std::void_t<decltype(std::from_chars(
                std::declval<const char *>(),
                std::declval<const char *>(),
                std::declval<T &>(),
                std::declval<const std::chars_format>()))>> = true;

        template <typename T, typename... Ts>
        constexpr bool is_one_of_v()
        {
            return (std::is_same_v<T, Ts> || ...);
        }

        template <
            size_t INDEX = 0u,
            typename TTuple,
            size_t SIZE = std::tuple_size_v<std::remove_reference_t<TTuple>>,
            typename TCallable,
            typename... TArgs>
        void for_each(TTuple && tuple, TCallable && callable, TArgs &&... args)
        {
            if constexpr (INDEX >= SIZE) { return; }
            else
            {
                std::invoke(callable, args..., std::get<INDEX>(tuple));
                for_each<INDEX + 1ul>(
                    std::forward<TTuple>(tuple),
                    std::forward<TCallable>(callable),
                    std::forward<TArgs>(args)...);
            }
        }

        template <typename... Ts>
        std::tuple<std::vector<Ts>...> make_reserved_vectors(size_t size)
        {
            auto vectors = std::tuple<std::vector<Ts>...>();
            for_each(
                vectors,
                [](size_t size, auto & vec) { vec.reserve(size); },
                size);
            return vectors;
        }

        template <typename TAdapter, typename TIn>
        using adapter_result_t = std::invoke_result_t<TAdapter, TIn>;

        template <typename TAdapter, typename TOut, typename TIn, typename = void>
        inline constexpr bool is_adapter_v = false;

        template <typename TAdapter, typename TOut, typename TIn>
        inline constexpr bool is_adapter_v<
            TAdapter,
            TOut,
            TIn,
            std::enable_if_t<std::is_convertible_v<adapter_result_t<TAdapter, TIn>, TOut>>> = true;

#if defined(FASTCSV_HAS_CXX20)
        template <typename TAdapter, typename TOut, typename TIn>
        concept adapter = is_adapter_v<TAdapter, TOut, TIn>;

        using identity = std::identity;
#else
        struct identity
        {
            template <typename T>
            FASTCSV_CONSTEXPR T && operator()(T && t) const noexcept
            {
                return std::forward<T>(t);
            }
        };
#endif

    }  // namespace detail

    class fastcsv_exception final : public std::exception
    {
    private:
        std::string what_;

    public:
        explicit fastcsv_exception(std::string message) noexcept : what_(std::move(message)) { }

        const char * what() const noexcept override { return what_.c_str(); }
    };

    template <typename TElem, typename TTraits = std::char_traits<TElem>>
    struct basic_csv_reader
    {
        detail::basic_csv_parser<TElem, TTraits> & parser;

        template <typename TRead, typename... TArgs>
        FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR inline TRead read(TArgs &&... args) const
        {
            if constexpr (std::is_same_v<from_csv<TRead, void, TElem, TTraits>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            return from_csv<TRead, void, TElem, TTraits>{ parser }(std::forward<TArgs>()...);
        }

        template <typename TRead, typename... TArgs>
        FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR inline std::optional<TRead> read_opt(TArgs &&... args) const
        {
            if constexpr (std::is_same_v<from_csv<std::optional<TRead>, void, TElem, TTraits>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            return from_csv<std::optional<TRead>, void, TElem, TTraits>{ parser }(std::forward<TArgs>(args)...);
        }
    };

    using csv_reader = basic_csv_reader<char>;


    template <typename T, typename TElem, typename TTraits>
    struct from_csv<T, std::enable_if_t<detail::has_from_chars_integral_v<T>>, TElem, TTraits> final
      : basic_csv_reader<TElem, TTraits>
    {
        FASTCSV_NO_DISCARD inline T operator()([[maybe_unused]] const int base = 10) const
        {
            auto element = this->parser.consume_column();
            return parse(element);
        }

        FASTCSV_NO_DISCARD inline static T parse(std::basic_string_view<TElem, TTraits> element, const int base = 10)
        {
#if defined(FASTCSV_HAS_FROM_CHAR)
            T result;
            auto [ptr, errorCode] = std::from_chars(element.data(), element.data() + element.size(), result, base);
            if (errorCode != std::errc())
            {
                throw fastcsv_exception(fmt::format(
                    "Failed to parse {} from '{}', errorCode: {}  {} {}",
                    typeid(T).name(),
                    element,
                    errorCode,
                    __FILE__,
                    __LINE__));
            }
            return result;
#else
            if constexpr (detail::is_one_of_v<T, char, short, int>)
            {
                return static_cast<T>(std::stoi(std::string(element)));
            }
            if constexpr (detail::is_one_of_v<T, unsigned char, unsigned short, unsigned int>)
            {
                return static_cast<T>(std::stou(std::string(element)));
            }
            if constexpr (std::is_same_v<T, long>) { return std::stol(std::string(element)); }
            if constexpr (std::is_same_v<T, long long>) { return std::stoll(std::string(element)); }
            if constexpr (std::is_same_v<T, unsigned long>) { return std::stoul(std::string(element)); }
            if constexpr (std::is_same_v<T, unsigned long long>) { return std::stoull(std::string(element)); }
#endif
        }
    };

    template <typename T, typename TElem, typename TTraits>
    struct from_csv<T, std::enable_if_t<detail::has_from_chars_floating_point_v<T>>, TElem, TTraits> final
      : basic_csv_reader<TElem, TTraits>
    {
        FASTCSV_NO_DISCARD inline T operator()(const std::chars_format fmt = std::chars_format::general) const
        {
            auto element = this->parser.consume_column();
            return parse(element, fmt);
        }

        FASTCSV_NO_DISCARD inline static T parse(
            std::basic_string_view<TElem, TTraits> element, const std::chars_format fmt = std::chars_format::general)
        {
#if defined(FASTCSV_HAS_FROM_CHAR)
            T result;
            auto [ptr, errorCode] = std::from_chars(element.data(), element.data() + element.size(), result, fmt);
            if (errorCode != std::errc())
            {
                throw fastcsv_exception(fmt::format(
                    "Failed to parse {} from '{}', errorCode: {}  {} {}",
                    typeid(T).name(),
                    element,
                    errorCode,
                    __FILE__,
                    __LINE__));
            }
            return result;
#else
            if constexpr (std::is_same_v<T, float>) { return std::stof(std::string(element)); }
            if constexpr (std::is_same_v<T, double>) { return std::stod(std::string(element)); }
            if constexpr (std::is_same_v<T, long double>) { return std::stold(std::string(element)); }
#endif
        }
    };

    template <typename TElem, typename TTraits>
    struct from_csv<char, void, TElem, TTraits> final : basic_csv_reader<TElem, TTraits>
    {
        FASTCSV_NO_DISCARD inline TElem operator()() const
        {
            auto value = this->parser.consume_column();
            if (value.size() != 1ul)
            {
                // ToDo:
                throw fastcsv_exception("");
            }

            return *value.data();
        }
    };

    template <typename TElem, typename TTraits>
    struct from_csv<std::string, void, TElem, TTraits> final : basic_csv_reader<TElem, TTraits>
    {
        FASTCSV_NO_DISCARD inline std::basic_string<TElem, TTraits> operator()() const
        {
            auto value = this->parser.consume_column();
            if (value.empty()) { return std::basic_string<TElem, TTraits>(); }

            if (value.size() != 1ul
                && value[0] == detail::quote<TElem> && value[value.size() - 1ul] == detail::quote<TElem>)
            {
                std::basic_stringstream<TElem, TTraits> ss;

                for (auto pos = 1ul; pos < value.size() - 1ul; ++pos)
                {
                    if (value[pos] == detail::escape<TElem> && value[pos + 1ul] == detail::quote<TElem>)
                    {
                        ss << detail::quote<TElem>;
                        ++pos;
                    }
                    else
                    {
                        ss << value[pos];
                    }
                }

                return ss.str();
            }

            return std::basic_string<TElem, TTraits>(value);
        }
    };

    template <typename TElem, typename TTraits>
    struct from_csv<bool, void, TElem, TTraits> final : basic_csv_reader<TElem, TTraits>
    {
        FASTCSV_NO_DISCARD inline bool operator()() const
        {
            auto value = this->parser.consume_column();

            if (value == "true" || value == "TRUE" || value == "True" || value == "t") { return true; }
            if (value == "false" || value == "FALSE" || value == "False" || value == "f") { return false; }

            throw fastcsv_exception(fmt::format("Unable to parse bool from '{}'  {} {}", value, __FILE__, __LINE__));
        }
    };

    template <typename T, typename TElem, typename TTraits>
    struct from_csv<std::optional<T>, void, TElem, TTraits> final : basic_csv_reader<TElem, TTraits>
    {
        FASTCSV_NO_DISCARD inline std::optional<T> operator()() const
        {
            if (this->parser.current_column_empty())
            {
                this->parser.advance_column();
                return std::nullopt;
            }

            return from_csv<T, void, TElem, TTraits>{ this->parser }();
        }
    };

#if defined(FASTCSV_HAS_CXX20)
    template <typename TElem, typename TTraits>
    struct from_csv<std::chrono::year_month_day, void, TElem, TTraits> final : basic_csv_reader<TElem, TTraits>
    {
        FASTCSV_NO_DISCARD inline std::chrono::year_month_day operator()() const
        {
            auto element = this->parser.consume_column();

            if (element.size() != 10ul) { throw fastcsv_exception(""); }

            return std::chrono::year_month_day{
                std::chrono::year(from_csv<int, void, TElem, TTraits>::parse(element.substr(0ul, 4ul))),
                std::chrono::month(from_csv<int, void, TElem, TTraits>::parse(element.substr(5ul, 2ul))),
                std::chrono::day(from_csv<int, void, TElem, TTraits>::parse(element.substr(8ul, 2ul)))
            };
        }
    };
#endif

    template <typename... Ts>
    struct from_csv<std::tuple<Ts...>, void, char> final : basic_csv_reader<char>
    {
        FASTCSV_NO_DISCARD inline std::tuple<Ts...> operator()() const { return apply<Ts...>(); }

    private:
        template <typename T, typename... Tss>
        FASTCSV_NO_DISCARD inline std::tuple<T, Tss...> apply() const
        {
            if constexpr (sizeof...(Tss) == 0ul) { return std::make_tuple<T>(read<T>()); }
            else
            {
                auto leftArg = read<T>();
                return std::make_tuple<T, Tss...>(std::move(leftArg), std::get<Tss>(apply<Tss...>())...);
            }
        }
    };

    template <typename T, size_t N, typename TElem, typename TTraits>
    struct from_csv<std::array<T, N>, void, TElem, TTraits> final : basic_csv_reader<TElem, TTraits>
    {
        FASTCSV_NO_DISCARD inline std::array<T, N> operator()() const
        {
            auto result = std::array<T, N>{};
            for (auto i = 0ul; i < N; ++i)
            {
                result[i] = this->template read<T>();
            }
            return result;
        }
    };

    template <typename TElem, typename TTraits = std::char_traits<TElem>>
    struct basic_csv_writer
    {
        std::basic_ostream<TElem, TTraits> & stream;
        bool first;

        template <typename TWrite, typename... TArgs>
        inline void write(const TWrite & value, TArgs &&... args)
        {
            if constexpr (std::is_same_v<to_csv<TWrite, void, TElem, TTraits>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            if (!first) { stream << detail::default_column_delimiter<TElem>; }

            to_csv<TWrite, void, TElem, TTraits>{ stream, true }(value, std::forward<TArgs>(args)...);

            first = false;
        }
    };

    using csv_writer = basic_csv_writer<char>;

    template <typename T, typename TElem, typename TTraits>
    struct to_csv<
        T,
        std::enable_if_t<detail::has_from_chars_integral_v<T> || detail::has_from_chars_floating_point_v<T>>,
        TElem,
        TTraits>
        final : basic_csv_writer<TElem, TTraits>
    {
        inline void operator()(T value) { fmt::print(this->stream, "{}", value); }
        inline void operator()(T value, std::basic_string_view<TElem, TTraits> fmt)
        {
            fmt::print(this->stream, fmt, value);
        }
    };

    template <typename TElem, typename TTraits>
    struct to_csv<char, void, TElem, TTraits> final : basic_csv_writer<TElem, TTraits>
    {
        inline void operator()(char value) { this->stream << value; }
    };

    template <typename TElem, typename TTraits>
    struct to_csv<std::basic_string<TElem, TTraits>, void, TElem, TTraits> final : basic_csv_writer<TElem, TTraits>
    {
        inline void operator()(const std::basic_string<TElem, TTraits> & value)
        {
            auto hasComma
                = value.find(detail::default_column_delimiter<TElem>) != std::basic_string<TElem, TTraits>::npos;
            auto quotePos = value.find(detail::quote<TElem>);

            if (quotePos != std::basic_string<TElem, TTraits>::npos)
            {
                this->stream << detail::quote<TElem>;

                size_t startPos = 0ul;
                do
                {
                    this->stream << std::basic_string_view<TElem, TTraits>(value.data() + startPos, quotePos - startPos)
                                 << detail::escape<TElem> << detail::quote<TElem>;

                    startPos = quotePos + 1ul;
                    quotePos = value.find(detail::quote<TElem>, startPos);
                }
                while (quotePos != std::basic_string<TElem, TTraits>::npos);

                this->stream << std::basic_string_view<TElem, TTraits>(value.data() + startPos, value.size() - startPos)
                             << detail::quote<TElem>;
            }
            else if (hasComma)
            {
                this->stream << detail::quote<TElem> << value << detail::quote<TElem>;
            }
            else
            {
                this->stream << value;
            }
        }
    };

    template <typename TElem, typename TTraits>
    struct to_csv<bool, void, TElem, TTraits> final : basic_csv_writer<TElem, TTraits>
    {
        inline void operator()(bool value) { fmt::print(this->stream, value ? "true" : "false"); }
    };

    template <typename T, typename TElem, typename TTraits>
    struct to_csv<std::optional<T>, void, TElem, TTraits> final : basic_csv_writer<TElem, TTraits>
    {
        inline void operator()(const std::optional<T> & value)
        {
            if (!value) { return; }

            to_csv<T, void, TElem, TTraits>{ this->stream, this->first }(*value);
        }
    };

#if defined(FASTCSV_HAS_CXX20)
    template <typename TElem, typename TTraits>
    struct to_csv<std::chrono::year_month_day, void, TElem, TTraits> final : basic_csv_writer<TElem, TTraits>
    {
        inline void operator()(std::chrono::year_month_day value)
        {
            fmt::print(
                this->stream,
                "{}-{}-{}",
                value.year().operator int(),
                value.month().operator unsigned int(),
                value.day().operator unsigned int());
        }
    };
#endif

    template <typename... Ts>
    struct to_csv<std::tuple<Ts...>, void> final : csv_writer
    {
        inline void operator()(const std::tuple<Ts...> & value) { apply(value); }

    private:
        template <size_t I = 0u, typename... Tss>
        inline void apply(const std::tuple<Tss...> & value)
        {
            if constexpr (I == sizeof...(Tss)) { return; }
            else
            {
                write(std::get<I>(value));
                apply<I + 1u, Tss...>(value);
            }
        }
    };

    template <typename T, size_t N, typename TElem, typename TTraits>
    struct to_csv<std::array<T, N>, void, TElem, TTraits> final : basic_csv_writer<TElem, TTraits>
    {
        inline void operator()(const std::array<T, N> & value)
        {
            for (auto i = 0ul; i < N; ++i)
            {
                this->template write<T>(value[i]);
            }
        }
    };

    namespace detail
    {

        template <size_t I = 0ul, typename... Ts>
        void read_line(csv_parser & parser, std::tuple<std::vector<Ts>...> & vectors)
        {
            if constexpr (I == sizeof...(Ts)) { return; }
            else
            {
                auto & vec = std::get<I>(vectors);
                using value_type = typename std::remove_reference_t<decltype(vec)>::value_type;
                vec.emplace_back(from_csv<value_type>{ parser }());

                read_line<I + 1ul, Ts...>(parser, vectors);
            }
        }

        template <typename T, std::enable_if_t<!std::is_same_v<T, void>> * = nullptr>
        void write_line(std::ostream & stream, const std::vector<T> & vector, size_t index, bool & first)
        {
            to_csv<T>{ stream, first }.write(vector[index]);
            first = false;
        }

        struct no_header_tag
        {
        };

    }  // namespace detail

    FASTCSV_CONSTEXPR inline detail::no_header_tag no_header{};

#if defined(FASTCSV_HAS_CXX20)
    template <typename T, typename TIntermediate, detail::adapter<T, TIntermediate> TAdapter>
#else
    template <
        typename T,
        typename TIntermediate,
        typename TAdapter,
        std::enable_if_t<detail::is_adapter_v<TAdapter, T, TIntermediate>> * = nullptr>
#endif
    FASTCSV_NO_DISCARD std::vector<T> read_csv(
        const std::string & content,
        TAdapter adapter,
        std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        if (content.empty()) { return std::vector<T>(); }

        auto parser = detail::csv_parser(
            content,
            detail::default_column_delimiter<char>,
            detail::quote<char>,
            detail::escape<char>);

        if (!noHeaderOption.has_value()) { parser.advance_line(); }

        auto data = std::vector<T>();
        // if (linesIterator.current_element_size() != 0ul)
        // {
        //     // Simple heuristic to estimate the total number of lines to avoid lots of vector resizing
        //     auto estimatedNumberOfLines = content.size() / linesIterator.current_element_size();
        //     data.reserve(estimatedNumberOfLines);
        // }

        while (!parser.end_of_file())
        {
            data.emplace_back(adapter(from_csv<TIntermediate>{ parser }()));
            parser.advance_line();
        }

        return data;
    }

    template <typename T>
    FASTCSV_NO_DISCARD std::vector<T> read_csv(
        const std::string & content, std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        return read_csv<T, T, detail::identity>(content, detail::identity{}, noHeaderOption);
    }

#if defined(FASTCSV_HAS_CXX20)
    template <typename T, typename TIntermediate, detail::adapter<T, TIntermediate> TAdapter>
#else
    template <
        typename T,
        typename TIntermediate,
        typename TAdapter,
        std::enable_if_t<detail::is_adapter_v<TAdapter, T, TIntermediate>> * = nullptr>
#endif
    FASTCSV_NO_DISCARD std::vector<T> load_csv(
        const std::filesystem::path & filePath,
        TAdapter adapter,
        std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath))
        {
            throw fastcsv_exception(fmt::format("File does not exist: '{}'  {} {}", filePath, __FILE__, __LINE__));
        }

        if (filePath.extension() != detail::csv_extension<char>)
        {
            throw fastcsv_exception(fmt::format(
                "File does not have the required '.{}' extension: {}  {} {}",
                detail::csv_extension<char>,
                __FILE__,
                __LINE__));
        }

        // ToDo: use memory mapped files
        auto file = std::ifstream(filePath, std::ios::in | std::ios::binary);

        if (!file)
        {
            throw fastcsv_exception(fmt::format("Cannot open file: {}  {} {}", filePath, __FILE__, __LINE__));
        }

        std::string content;
        content.resize(static_cast<size_t>(std::filesystem::file_size(filePath)));
        file.read(content.data(), content.size());

        if (!file)
        {
            throw fastcsv_exception(
                fmt::format("Could not read full contents of file: {}  {} {}", filePath, __FILE__, __LINE__));
        }

        return read_csv<T, TIntermediate, TAdapter>(content, adapter, noHeaderOption);
    }

    template <typename T>
    FASTCSV_NO_DISCARD std::vector<T> load_csv(
        const std::filesystem::path & filePath, std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        return load_csv<T, T, detail::identity>(filePath, detail::identity{}, noHeaderOption);
    }


#if defined(FASTCSV_HAS_CXX20)
    template <typename T, typename TIntermediate, detail::adapter<TIntermediate, T> TAdapter>
#else
    template <
        typename T,
        typename TIntermediate,
        typename TAdapter,
        std::enable_if_t<detail::is_adapter_v<TAdapter, TIntermediate, T>> * = nullptr>
#endif
    void write_csv(
        std::ostream & os,
        const std::vector<T> & data,
        TAdapter adapter,
        std::optional<detail::no_header_tag> noHeader = std::nullopt)
    {
        if constexpr (has_csv_headers<TIntermediate>)
        {
            if (!noHeader.has_value())
            {
                auto first = true;
                auto headers = csv_headers<TIntermediate>{}();
                for (auto header : headers)
                {
                    if (!first) { os << detail::default_column_delimiter<char>; }
                    os << header;
                    first = false;
                }
                os << detail::default_line_delimiter<char>;
            }
        }

        auto writer = to_csv<TIntermediate>{ os, true };

        for (const auto & value : data)
        {
            writer.first = true;
            writer.write(adapter(value));
            os << detail::default_line_delimiter<char>;
        }
    }

    template <typename T>
    void write_csv(
        std::ostream & os,
        const std::vector<T> & data,
        std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        write_csv<T, T, detail::identity>(os, data, detail::identity{}, noHeaderOption);
    }

#if defined(FASTCSV_HAS_CXX20)
    template <typename T, typename TIntermediate, detail::adapter<TIntermediate, T> TAdapter>
#else
    template <
        typename T,
        typename TIntermediate,
        typename TAdapter,
        std::enable_if_t<detail::is_adapter_v<TAdapter, TIntermediate, T>> * = nullptr>
#endif
    FASTCSV_NO_DISCARD std::string write_csv(
        const std::vector<T> & data,
        TAdapter adapter,
        std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        std::stringstream stream;
        write_csv<T, TIntermediate, TAdapter>(stream, data, adapter, noHeaderOption);
        return stream.str();
    }

    template <typename T>
    FASTCSV_NO_DISCARD std::string write_csv(
        const std::vector<T> & data, std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        return write_csv<T, T, detail::identity>(data, detail::identity{}, noHeaderOption);
    }

#if defined(FASTCSV_HAS_CXX20)
    template <typename T, typename TIntermediate, detail::adapter<TIntermediate, T> TAdapter>
#else
    template <
        typename T,
        typename TIntermediate,
        typename TAdapter,
        std::enable_if_t<detail::is_adapter_v<TAdapter, TIntermediate, T>> * = nullptr>
#endif
    void save_csv(
        const std::filesystem::path & filePath,
        const std::vector<T> & data,
        TAdapter adapter,
        std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        if (filePath.extension() != detail::csv_extension<char>)
        {
            throw fastcsv_exception(fmt::format(
                "File does not have the required '.{}' extension: {}  {} {}",
                detail::csv_extension<char>,
                __FILE__,
                __LINE__));
        }

        auto file = std::ofstream(filePath, std::ios::out | std::ios::binary);
        write_csv<T, TIntermediate, TAdapter>(file, data, adapter, noHeaderOption);
    }

    template <typename T>
    void save_csv(
        const std::filesystem::path & filePath,
        const std::vector<T> & data,
        std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        save_csv<T, T, detail::identity>(filePath, data, detail::identity{}, noHeaderOption);
    }


    template <typename... Ts>
    FASTCSV_NO_DISCARD std::tuple<std::vector<Ts>...> read_csv_v(
        const std::string & content, std::optional<detail::no_header_tag> noHeaderOption = std::nullopt)
    {
        if (content.empty()) { return std::tuple<std::vector<Ts>...>(); }

        auto parser = detail::csv_parser(
            content,
            detail::default_column_delimiter<char>,
            detail::quote<char>,
            detail::escape<char>);
        if (!noHeaderOption.has_value()) { parser.advance_line(); }

        std::tuple<std::vector<Ts>...> data = [&]() {
            // if (parser.current_element_size() != 0ul)
            // {
            //     // Simple heuristic to estimate the total number of lines to avoid lots of vector resizing
            //     auto estimatedNumberOfLines = content.size() / parser.current_element_size();
            //     return detail::make_reserved_vectors<Ts...>(estimatedNumberOfLines);
            // }

            return std::tuple<std::vector<Ts>...>();
        }();

        while (!parser.end_of_file())
        {
            detail::read_line<0ul, Ts...>(parser, data);
            parser.advance_line();
        }

        return data;
    }

    // ToDo: load_csv_v

    template <typename... Ts>
    void write_csv_v(
        std::ostream & os, const std::vector<std::string_view> & headers, const std::vector<Ts> &... vectors)
    {
        if (!headers.empty())
        {
            auto first = true;
            for (auto i = 0ul; i < std::min(headers.size(), sizeof...(Ts)); ++i)
            {
                if (!first) { os << detail::default_column_delimiter<char>; }
                os << headers[i];
                first = false;
            }
            for (auto i = headers.size(); i < sizeof...(Ts); ++i)
            {
                os << detail::default_column_delimiter<char>;
            }
            os << detail::default_line_delimiter<char>;
        }

        // fold to get the minimal vector size: https://www.foonathan.net/2020/05/fold-tricks/
        auto minSize = (vectors, ...).size();
        ((vectors.size() < minSize ? minSize, 0ul : 0ul), ...);

        for (auto i = 0ul; i < minSize; ++i)
        {
            auto first = true;
            (detail::write_line(os, vectors, i, first), ...);
            os << detail::default_line_delimiter<char>;
        }
    }

    template <typename... Ts>
    FASTCSV_NO_DISCARD std::string write_csv_v(const std::vector<Ts> &... vectors)
    {
        std::stringstream stream;
        write_csv_v<Ts...>(stream, {}, vectors...);
        return stream.str();
    }

    template <typename... Ts>
    FASTCSV_NO_DISCARD std::string write_csv_v(
        const std::vector<std::string_view> & headers, const std::vector<Ts> &... vectors)
    {
        std::stringstream stream;
        write_csv_v<Ts...>(stream, headers, vectors...);
        return stream.str();
    }

    template <typename... Ts>
    void save_csv_v(const std::filesystem::path & filePath, const std::vector<Ts> &... vectors)
    {
        if (filePath.extension() != detail::csv_extension<char>)
        {
            throw fastcsv_exception(fmt::format(
                "File does not have the required '.{}' extension: {}  {} {}",
                detail::csv_extension<char>,
                __FILE__,
                __LINE__));
        }

        auto file = std::ofstream(filePath, std::ios::out | std::ios::binary);
        write_csv_v<Ts...>(file, {}, vectors...);
    }

    template <typename... Ts>
    void save_csv_v(
        const std::filesystem::path & filePath,
        const std::vector<std::string_view> & headers,
        const std::vector<Ts> &... vectors)
    {
        if (filePath.extension() != detail::csv_extension<char>)
        {
            throw fastcsv_exception(fmt::format(
                "File does not have the required '.{}' extension: {}  {} {}",
                detail::csv_extension<char>,
                __FILE__,
                __LINE__));
        }

        auto file = std::ofstream(filePath, std::ios::out | std::ios::binary);
        write_csv_v<Ts...>(file, headers, vectors...);
    }

}  // namespace fastcsv