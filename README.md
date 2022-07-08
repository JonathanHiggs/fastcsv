# fastcsv

A fast c++20 csv library


## Example

Define a basic structure

```cpp
struct Foo
{
    std::string name;
    int number;
}
```

Define `to_csv` and `from_csv` specialisations for the structure in the `fastcsv` namespace

```cpp
namepace fastcsv
{
    template <>
    struct from_csv<Foo> : csv_reader
    {
        Foo operator()() const
        { 
            return Foo{ read<std::string>(), read<int>() };
        }
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
}
```

Use the library to convert data to strings and files

```cpp
void myMethod()
{
    std::vector<Foo> data = fastcsv::load_csv("some/path.csv");
    fastcsv::save_csv("some/path.csv", data);

    auto csvString = fastcsv::to_csv_string(data);
    auto parsedData = fastcsv::parse_csv(csvString);
}
```