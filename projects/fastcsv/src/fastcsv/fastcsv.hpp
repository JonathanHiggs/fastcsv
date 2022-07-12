#pragma once


// https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html

// Detect the host platform
#if defined(__linux__) && !defined(FASTCSV_PLATFORM_LINUX)
    #define FASTCSV_PLATFORM_LINUX
#elif (defined(_WIN32) || defined(_WIN64)) && !defined(FASTCSV_PLATFORM_WIN)
    #define FASTCSV_PLATFORM_WIN
#else
    #error "Platform not supported"
#endif


// Detect the compiler
#if (defined(_MSC_VER) && !defined(__clang)) && !defined(FASTCSV_COMPILER_MSVC)
    #define FASTCSV_COMPILER_MSVC
    #define FASTCSV_COMPILER_MSVC_VERSION = _MSC_VER
#elif defined(__GNUC__) && !defined(FASTCSV_COMPILER_GCC)
    #define FASTCSV_COMPILER_GCC
    #define FASTCSV_COMPILER_GCC_MAJOR = __GNUC__
    #define FASTCSV_COMPILER_GCC_MINOR = __GNUC_MINOR__
#elif defined(__clang__) && !defined(FASTCSV_COMPILER_CLANG)
    #define FASTCSV_COMPILER_CLANG
#else
    #error "Compiler not supported"
#endif


// Detect the standard version
#if !defined(FASTCSV_HAS_CXX17) && !defined(FASTCSV_HAS_CXX20)
    #if defined(_MSVC_LANG)
        #define FASTCSV_STL_LANG _MSVC_LANG
    #elif defined(__cplusplus)
        #define FASTCSV_STL_LANG __cplusplus
    #else
        #define FASTCSV_STL_LANG 0L
    #endif  // ^^^ no C++ support ^^^

    #if FASTCSV_STL_LANG > 201703L
        #define FASTCSV_HAS_CXX17 1
        #define FASTCSV_HAS_CXX20 1
    #elif FASTCSV_STL_LANG > 201402L
        #define FASTCSV_HAS_CXX17 1
        #define FASTCSV_HAS_CXX20 0
    #else
        #define FASTCSV_HAS_CXX17 0
        #define FASTCSV_HAS_CXX20 0
    #endif

    #undef FASTCSV_STL_LANG
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


#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>

#if defined(__cpp_lib_to_chars)
    #include <charconv>
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

        inline static constexpr std::string_view csv_extension = "csv";
        inline static constexpr char default_column_delimiter = ',';
        inline static constexpr char default_line_delimiter = '\n';

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
              : content_(content), delimiter_(delimiter), current_(0ul), next_(next(delimiter_, current_))
            { }

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
                auto nextQuote = next('"', current_);

                if (nextQuote < next_)
                {
                    do
                    {
                        nextQuote = next('"', nextQuote + 1ul);
                    }
                    while (content_[nextQuote - 1ul] == '\\');
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

        // clang-format off
        template <typename T> inline constexpr bool is_csv_primative_v = false;
        template <> inline constexpr bool is_csv_primative_v<int> = true;
        template <> inline constexpr bool is_csv_primative_v<long> = true;
        template <> inline constexpr bool is_csv_primative_v<long long> = true;
        template <> inline constexpr bool is_csv_primative_v<unsigned> = true;
        template <> inline constexpr bool is_csv_primative_v<unsigned long> = true;
        template <> inline constexpr bool is_csv_primative_v<unsigned long long> = true;
        template <> inline constexpr bool is_csv_primative_v<float> = true;
        template <> inline constexpr bool is_csv_primative_v<double> = true;
        // clang-format on

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

        template <typename TRead>
        FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR inline TRead read() const
        {
            if constexpr (std::is_same_v<from_csv<TRead>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            return from_csv<TRead>{ iterator }();
        }

        template <typename TRead>
        FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR inline std::optional<TRead> read_opt() const
        {
            if constexpr (std::is_same_v<from_csv<std::optional<TRead>>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            return from_csv<std::optional<TRead>>{ iterator }();
        }

        template <typename TRead>
        FASTCSV_NO_DISCARD FASTCSV_CONSTEXPR inline std::optional<TRead> try_read() const
        {
            if constexpr (std::is_same_v<from_csv<TRead>, decltype(*this)>)
            {
                throw fastcsv_exception(fmt::format("Recursive call  {} {}", __FILE__, __LINE__));
            }

            try
            {
                return std::make_optional<TRead>(from_csv<TRead>{ iterator }());
            }
            catch (...)
            {
                return std::nullopt;
            }
        }
    };

    template <typename T>
    struct from_csv<T, std::enable_if_t<detail::is_csv_primative_v<T>>> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline T operator()() const
        {
#if defined(__cpp_lib_to_chars)
            T value;
            auto element = iterator.consume();
            auto [ptr, errorCode] = std::from_chars(element.data(), element.data() + element.size(), value);
            if (errorCode != std::errc())
            {
                throw fastcsv_exception(fmt::format(
                                            "Failed to parse {} from '{}', errorCode: {}  {} {}",
                                            typeid(T).name(),
                                            element,
                                            errorCode,
                                            __FILE__,
                                            __LINE__)
                                            .c_str());
            }
            return value;
#else
            if constexpr (std::is_same_v<T, int>) { return std::stoi(std::string(iterator.consume())); }
            if constexpr (std::is_same_v<T, long>) { return std::stol(std::string(iterator.consume())); }
            if constexpr (std::is_same_v<T, long long>) { return std::stoll(std::string(iterator.consume())); }
            if constexpr (std::is_same_v<T, unsigned> || std::is_same_v<T, unsigned long>)
            {
                return std::stoul(std::string(iterator.consume()));
            }
            if constexpr (std::is_same_v<T, unsigned long long>)
            {
                return std::stoull(std::string(iterator.consume()));
            }
            if constexpr (std::is_same_v<T, float>) { return std::stof(std::string(iterator.consume())); }
            if constexpr (std::is_same_v<T, double>) { return std::stod(std::string(iterator.consume())); }
#endif
        }
    };

    template <>
    struct from_csv<std::string> final : csv_reader
    {
        FASTCSV_NO_DISCARD inline std::string operator()() const
        {
            auto value = iterator.consume();
            if (value.empty()) { return std::string(); }

            if (value.size() == 1ul && value[0] == '"') { return std::string("\""); }

            if (value[0] == '"' && value[value.size() - 1ul] == '"')
            {
                // ToDo: unescape chars
                return std::string(value.substr(1ul, value.size() - 2ul));
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

            if (value == "true" || value == "TRUE" || value == "True") { return true; }
            if (value == "false" || value == "FALSE" || value == "False") { return false; }

            throw fastcsv_exception(
                fmt::format("Unable to parse bool from '{}'  {} {}", value, __FILE__, __LINE__));
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
    struct to_csv<T, std::enable_if_t<detail::is_csv_primative_v<T>>> final : csv_writer
    {
        inline void operator()(T value) { fmt::print(stream, "{}", value); }
        inline void operator()(T value, std::string_view fmt) { fmt::print(stream, fmt, value); }
    };

    template <>
    struct to_csv<std::string> final : csv_writer
    {
        inline void operator()(const std::string & value)
        {
            auto hasComma = value.find(detail::default_column_delimiter) != std::string::npos;
            auto hasQuote = value.find('"') != std::string::npos;

            if (hasQuote)
            {
                // ToDo: escape quotes and write
                throw fastcsv_exception(fmt::format("Not implemented  {} {}", __FILE__, __LINE__));
            }
            else if (hasComma)
            {
                stream << '"' << value << '"';
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


    namespace detail
    {

        template <typename T>
        void write_line(std::ostream & stream, const std::vector<T> & vector, size_t index, bool & first)
        {
            auto writer = to_csv<T>{ stream, first }.write(vector[index]);
            first = false;
        }

    }  // namespace detail

    template <typename T>
    FASTCSV_NO_DISCARD std::vector<T> read_csv(const std::string & content)
    {
        if (content.empty()) { return std::vector<T>(); }

        auto data = std::vector<T>();
        auto linesIterator = detail::csv_iterator(content, detail::default_line_delimiter);

        if (linesIterator.current_element_size() != 0ul)
        {
            // Simple heuristic to estimate the total number of lines to avoid lots of vector resizing
            auto estimatedNumberOfLines = content.size() / linesIterator.current_element_size();
            data.reserve(estimatedNumberOfLines);
        }

        // ToDo: headers

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
            throw fastcsv_exception(
                fmt::format("File does not exist: '{}'  {} {}", filePath, __FILE__, __LINE__));
        }

        if (filePath.extension() != detail::csv_extension)
        {
            throw fastcsv_exception(fmt::format(
                                        "File does not have the required '.{}' extension: {}  {} {}",
                                        detail::csv_extension,
                                        __FILE__,
                                        __LINE__)
                                        .c_str());
        }

        auto file = std::ifstream(filePath, std::ios::in | std::ios::binary);

        if (!file)
        {
            throw fastcsv_exception(fmt::format("Cannot open file: {}  {} {}", filePath, __FILE__, __LINE__));
        }

        std::string content;
        content.reserve(static_cast<size_t>(std::filesystem::file_size(filePath)));
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
                                        __LINE__)
                                        .c_str());
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
                                        __LINE__)
                                        .c_str());
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