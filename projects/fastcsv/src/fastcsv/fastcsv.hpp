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
    #define FASTCSV_COMPILER_MSVC_VERSION = _MSC_VER
#elif defined(__GNUC__)
    #define FASTCSV_COMPILER_GCC
    #define FASTCSV_COMPILER_GCC_MAJOR = __GNUC__
    #define FASTCSV_COMPILER_GCC_MINOR = __GNUC_MINOR__
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

    #if FASTCSV_STD_LANG > 201703L
        #define FASTCSV_HAS_CXX20
    #endif
    #if FASTCSV_STD_LANG > 201402L
        #define FASTCSV_HAS_CXX17
    #endif

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

#if defined(FASTCSV_HAS_FROM_CHAR)
    #include <charconv>
#endif
#if defined(FASTCSV_HAS_CXX20)
    #include <chrono>
#endif
#include <exception>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>


namespace fastcsv
{

    namespace detail
    {

        inline static constexpr std::string_view csv_extension = ".csv";
        inline static constexpr char default_column_delimiter = ',';
        inline static constexpr char default_line_delimiter = '\n';
        inline static constexpr char quote_char = '"';
        inline static constexpr char escape_char = '\\';

        /// <summary>
        /// Iterator for consuming string data with split by a delimiter
        /// </summary>
        /// <typeparam name="TElem"></typeparam>
        /// <typeparam name="TTraits"></typeparam>
        template <typename TElem, typename TTraits>
        class basic_csv_iterator final
        {
        private:
            std::basic_string_view<TElem, TTraits> content_;
            TElem delimiter_;
            size_t current_;
            size_t next_;

        public:
            FASTCSV_CONSTEXPR basic_csv_iterator(
                std::basic_string_view<TElem, TTraits> content, TElem delimiter) noexcept
              : content_(content), delimiter_(delimiter), current_(0ul), next_(0ul)
            {
                next_ = next(delimiter_, current_);
                auto nextQuote = next(quote_char, current_);

                if (nextQuote < next_)
                {
                    do
                    {
                        nextQuote = next(quote_char, nextQuote + 1ul);
                    }
                    while (content_[nextQuote - 1ul] == escape_char);
                    next_ = next(delimiter_, nextQuote);
                }
            }

            /// <summary>
            /// Checks if the content is completed
            /// </summary>
            /// <returns>true if there are on more elements; false otherwise</returns>
            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR bool done() const noexcept { return current_ == next_; }

            /// <summary>
            /// The size of the current element
            /// </summary>
            /// <returns>Size of the current element</returns>
            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR size_t current_element_size() const noexcept
            {
                return next_ - current_;
            }

            /// <summary>
            /// Checks if the current element is empty
            /// </summary>
            /// <returns>true if the current element is empty; false otherwise</returns>
            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR bool current_element_empty() const noexcept
            {
                return current_element_size() == 0ul;
            }

            /// <summary>
            /// The contents current element
            /// </summary>
            /// <returns></returns>
            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR std::basic_string_view<TElem, TTraits> current_element() const noexcept
            {
                return std::basic_string_view<TElem, TTraits>(content_.data() + current_, next_ - current_);
            }

            /// <summary>
            /// Advances the iterator to the next element
            /// </summary>
            void FASTCSV_CONSTEXPR advance()
            {
                current_ = next_ + 1ul;

                if (current_ >= content_.size())
                {
                    current_ = next_ = content_.size();
                    return;
                }

                next_ = next(delimiter_, current_);
                auto nextQuote = next(quote_char, current_);

                if (nextQuote < next_)
                {
                    do
                    {
                        nextQuote = next(quote_char, nextQuote + 1ul);
                    }
                    while (content_[nextQuote - 1ul] == escape_char);
                    next_ = next(delimiter_, nextQuote);
                }
            }

            /// <summary>
            /// Consumes the current element by advancing
            /// </summary>
            /// <returns>Returns the contents of the element before advancing</returns>
            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR std::basic_string_view<TElem, TTraits> consume()
            {
                auto result = current_element();
                advance();
                return result;
            }

        private:
            /// <summary>
            /// Gets the position of the next instance of the supplied delimiter after the given position
            /// </summary>
            /// <param name="delimiter">Delimiter to search for</param>
            /// <param name="position">Position to search from</param>
            /// <returns>Position of the next instance of the delimiter or end if not found</returns>
            FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR size_t next(TElem delimiter, size_t position)
            {
                return std::min(content_.size(), content_.find(delimiter, position));
            }
        };

        using csv_iterator = basic_csv_iterator<char, std::char_traits<char>>;

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

    }  // namespace detail

    class fastcsv_exception final : public std::exception
    {
    private:
        std::string what_;

    public:
        explicit fastcsv_exception(std::string message) noexcept : what_(std::move(message)) { }

        const char * what() const noexcept override { return what_.c_str(); }
    };


    template <typename T, typename = void>
    struct from_csv;

    struct csv_reader
    {
        detail::csv_iterator & iterator;

        template <typename TRead, typename... TArgs>
        FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR inline TRead read(TArgs &&... args) const
        {
            if constexpr (std::is_same_v<from_csv<TRead>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            return from_csv<TRead>{ iterator }(std::forward<TArgs>()...);
        }

        template <typename TRead, typename... TArgs>
        FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR inline std::optional<TRead> read_opt(TArgs &&... args) const
        {
            if constexpr (std::is_same_v<from_csv<std::optional<TRead>>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            return from_csv<std::optional<TRead>>{ iterator }(std::forward<TArgs>(args)...);
        }
    };

    template <typename T>
    struct from_csv<T, std::enable_if_t<detail::has_from_chars_integral_v<T>>> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline T operator()(const int base = 10) const
        {
            auto element = iterator.consume();
            return parse(element);
        }

        FASTCSV_NO_DISCARD inline static T parse(std::string_view element, const int base = 10)
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

    template <typename T>
    struct from_csv<T, std::enable_if_t<detail::has_from_chars_floating_point_v<T>>> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline T operator()(const std::chars_format fmt = std::chars_format::general) const
        {
            auto element = iterator.consume();
            return parse(element, fmt);
        }

        FASTCSV_NO_DISCARD inline static T parse(
            std::string_view element, const std::chars_format fmt = std::chars_format::general)
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

    template <>
    struct from_csv<char> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline char operator()() const
        {
            auto value = iterator.consume();
            if (value.size() != 1ul)
            {
                // ToDo:
                throw fastcsv_exception("");
            }

            return *value.data();
        }
    };

    template <>
    struct from_csv<std::string> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline std::string operator()() const
        {
            auto value = iterator.consume();
            if (value.empty()) { return std::string(); }

            if (value.size() != 1ul && value[0] == detail::quote_char
                && value[value.size() - 1ul] == detail::quote_char)
            {
                std::stringstream ss;

                for (auto pos = 1ul; pos < value.size() - 1ul; ++pos)
                {
                    if (value[pos] == detail::escape_char && value[pos + 1ul] == detail::quote_char)
                    {
                        ss << detail::quote_char;
                        ++pos;
                    }
                    else
                    {
                        ss << value[pos];
                    }
                }

                return ss.str();
            }

            return std::string(value);
        }
    };

    template <>
    struct from_csv<bool> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline bool operator()() const
        {
            auto value = iterator.consume();

            if (value == "true" || value == "TRUE" || value == "True" || value == "t") { return true; }
            if (value == "false" || value == "FALSE" || value == "False" || value == "f") { return false; }

            throw fastcsv_exception(fmt::format("Unable to parse bool from '{}'  {} {}", value, __FILE__, __LINE__));
        }
    };

    template <typename T>
    struct from_csv<std::optional<T>> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline std::optional<T> operator()() const
        {
            if (iterator.current_element_empty())
            {
                iterator.advance();
                return std::nullopt;
            }

            return from_csv<T>{ iterator }();
        }
    };

#if defined(FASTCSV_HAS_CXX20)
    template <>
    struct from_csv<std::chrono::year_month_day> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline std::chrono::year_month_day operator()() const
        {
            auto element = iterator.consume();

            if (element.size() != 10ul) { throw fastcsv_exception(""); }

            return std::chrono::year_month_day{ std::chrono::year(from_csv<int>::parse(element.substr(0ul, 4ul))),
                                                std::chrono::month(from_csv<int>::parse(element.substr(5ul, 2ul))),
                                                std::chrono::day(from_csv<int>::parse(element.substr(8ul, 2ul))) };
        }
    };
#endif

    template <typename... Ts>
    struct from_csv<std::tuple<Ts...>, void> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline std::tuple<Ts...> operator()() const { return apply<Ts...>(); }

    private:
        template <typename T, typename... Ts>
        FASTCSV_NO_DISCARD inline std::tuple<T, Ts...> apply() const
        {
            if constexpr (sizeof...(Ts) == 0ul) { return std::make_tuple<T>(read<T>()); }
            else
            {
                auto leftArg = read<T>();
                return std::make_tuple<T, Ts...>(std::move(leftArg), std::get<Ts>(apply<Ts...>())...);
            }
        }
    };


    template <typename T, typename = void>
    struct to_csv;

    struct csv_writer
    {
        std::ostream & stream;
        bool first;

        template <typename TWrite, typename... TArgs>
        inline void write(const TWrite & value, TArgs &&... args)
        {
            if constexpr (std::is_same_v<to_csv<TWrite>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            if (!first) { stream << detail::default_column_delimiter; }

            to_csv<TWrite>{ stream, true }(value, std::forward<TArgs>(args)...);

            first = false;
        }
    };

    template <typename T>
    struct to_csv<
        T,
        std::enable_if_t<detail::has_from_chars_integral_v<T> || detail::has_from_chars_floating_point_v<T>>>
        final : csv_writer
    {
        inline void operator()(T value) { fmt::print(stream, "{}", value); }
        inline void operator()(T value, std::string_view fmt) { fmt::print(stream, fmt, value); }
    };

    template <>
    struct to_csv<char> final : csv_writer
    {
        inline void operator()(char value) { stream << value; }
    };

    template <>
    struct to_csv<std::string> final : csv_writer
    {
        inline void operator()(const std::string & value)
        {
            auto hasComma = value.find(detail::default_column_delimiter) != std::string::npos;
            auto quotePos = value.find(detail::quote_char);

            if (quotePos != std::string::npos)
            {
                stream << detail::quote_char;

                size_t startPos = 0ul;
                do
                {
                    stream << std::string_view(value.data() + startPos, quotePos - startPos) << detail::escape_char
                           << detail::quote_char;

                    startPos = quotePos + 1ul;
                    quotePos = value.find(detail::quote_char, startPos);
                }
                while (quotePos != std::string::npos);

                stream << std::string_view(value.data() + startPos, value.size() - startPos) << detail::quote_char;
            }
            else if (hasComma)
            {
                stream << detail::quote_char << value << detail::quote_char;
            }
            else
            {
                stream << value;
            }
        }
    };

    template <>
    struct to_csv<bool> final : csv_writer
    {
        inline void operator()(bool value) { fmt::print(stream, value ? "true" : "false"); }
    };

    template <typename T>
    struct to_csv<std::optional<T>> final : csv_writer
    {
        inline void operator()(const std::optional<T> & value)
        {
            if (!value) { return; }

            to_csv<T>{ stream, first }(*value);
        }
    };

#if defined(FASTCSV_HAS_CXX20)
    template <>
    struct to_csv<std::chrono::year_month_day> final : csv_writer
    {
        inline void operator()(std::chrono::year_month_day value)
        {
            fmt::print(
                stream,
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
        template <size_t I = 0u, typename... Ts>
        inline void apply(const std::tuple<Ts...> & value)
        {
            if constexpr (I == sizeof...(Ts)) { return; }
            else
            {
                write(std::get<I>(value));
                apply<I + 1u, Ts...>(value);
            }
        }
    };

    namespace detail
    {

        template <typename T>
        void write_line(std::ostream & stream, const std::vector<T> & vector, size_t index, bool & first)
        {
            auto writer = to_csv<T>{ stream, first }.write(vector[index]);
            first = false;
        }

        struct no_header_tag
        {
        };

    }  // namespace detail

    FASTCSV_CONSTEXPR inline detail::no_header_tag no_header{};

    template <typename T>
    FASTCSV_NO_DISCARD std::vector<T> read_csv(
        const std::string & content, std::optional<detail::no_header_tag> noHeader = std::nullopt)
    {
        if (content.empty()) { return std::vector<T>(); }

        auto data = std::vector<T>();
        auto linesIterator = detail::csv_iterator(content, detail::default_line_delimiter);

        if (!noHeader.has_value()) { linesIterator.advance(); }

        if (linesIterator.current_element_size() != 0ul)
        {
            // Simple heuristic to estimate the total number of lines to avoid lots of vector resizing
            auto estimatedNumberOfLines = content.size() / linesIterator.current_element_size();
            data.reserve(estimatedNumberOfLines);
        }

        while (!linesIterator.done())
        {
            auto columnIterator = detail::csv_iterator(linesIterator.consume(), detail::default_column_delimiter);
            data.emplace_back(from_csv<T>{ columnIterator }());
        }

        return data;
    }

    template <typename T>
    FASTCSV_NO_DISCARD std::vector<T> load_csv(const std::filesystem::path & filePath)
    {
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath))
        {
            throw fastcsv_exception(fmt::format("File does not exist: '{}'  {} {}", filePath, __FILE__, __LINE__));
        }

        if (filePath.extension() != detail::csv_extension)
        {
            throw fastcsv_exception(fmt::format(
                "File does not have the required '.{}' extension: {}  {} {}",
                detail::csv_extension,
                __FILE__,
                __LINE__));
        }

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

        return read_csv<T>(content);
    }

    template <typename T>
    FASTCSV_NO_DISCARD std::string write_csv(const std::vector<T> & data)
    {
        std::stringstream stream;
        auto writer = to_csv<T>{ stream, true };

        for (const auto & value : data)
        {
            writer.first = true;
            writer.write(value);
            stream << detail::default_line_delimiter;
        }

        return stream.str();
    }

    template <typename... Ts>
    FASTCSV_NO_DISCARD std::string write_csv(const std::vector<Ts> &... vectors)
    {
        std::stringstream stream;

        // fold to get the minimal vector size: https://www.foonathan.net/2020/05/fold-tricks/
        auto minSize = (vectors, ...).size();
        ((vectors.size() < minSize ? minSize, 0 : 0), ...);

        for (auto i = 0ul; i < minSize; ++i)
        {
            auto first = true;
            (detail::write_line(stream, vectors, i, first), ...);
            stream << detail::default_line_delimiter;
        }

        return stream.str();
    }

    template <typename T>
    void save_csv(const std::filesystem::path & filePath, const std::vector<T> & data)
    {
        if (filePath.extension() != detail::csv_extension)
        {
            throw fastcsv_exception(fmt::format(
                "File does not have the required '.{}' extension: {}  {} {}",
                detail::csv_extension,
                __FILE__,
                __LINE__));
        }

        auto file = std::ofstream(filePath, std::ios::out | std::ios::binary);
        auto writer = to_csv<T>{ file, true };

        for (const auto & value : data)
        {
            writer.first = true;
            writer.write(value);
            file << detail::default_line_delimiter;
        }
    }

    template <typename... Ts>
    void save_csv(const std::filesystem::path & filePath, const std::vector<Ts> &... vectors)
    {
        if (filePath.extension() != detail::csv_extension)
        {
            throw fastcsv_exception(fmt::format(
                "File does not have the required '.{}' extension: {}  {} {}",
                detail::csv_extension,
                __FILE__,
                __LINE__));
        }

        auto file = std::ofstream(filePath, std::ios::out | std::ios::binary);

        // fold to get the minimal vector size: https://www.foonathan.net/2020/05/fold-tricks/
        auto minSize = (vectors, ...).size();
        ((vectors.size() < minSize ? minSize, 0 : 0), ...);

        for (auto i = 0ul; i < minSize; ++i)
        {
            auto first = true;
            (detail::write_line(file, vectors, i, first), ...);
            file << detail::default_line_delimiter;
        }
    }

}  // namespace fastcsv