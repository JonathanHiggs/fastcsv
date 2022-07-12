# fastcsv

A simple and fast c++20 csv library


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
void example()
{
    auto data = fastcsv::load_csv<Foo>("some/path.csv");
    fastcsv::save_csv("some/path.csv", data);

    auto csvString = fastcsv::write_csv(data);
    auto parsedData = fastcsv::read_csv<Foo>(csvString);
}
```