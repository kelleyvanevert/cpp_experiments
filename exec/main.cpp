#include <format>
#include <iostream>
#include <variant>
#include <string>

#include <fmt/core.h>

#include <boost/container/small_vector.hpp>

#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy/input/file.hpp>
#include <lexy/action/parse.hpp>
#include <lexy_ext/report_error.hpp>

#include <tsl/robin_map.h>
#include <tsl/robin_set.h>

using namespace std;

namespace
{
    struct Color
    {
        std::uint8_t r, g, b;
    };

    namespace grammar
    {
        namespace dsl = lexy::dsl;

        struct channel
        {
            static constexpr auto rule = dsl::integer<std::uint8_t>(dsl::n_digits<2, dsl::hex>);
            static constexpr auto value = lexy::forward<std::uint8_t>;
        };

        struct color
        {
            static constexpr auto rule = dsl::hash_sign + dsl::times<3>(dsl::p<channel>);
            static constexpr auto value = lexy::construct<Color>;
        };
    }
}

struct List
{
    std::vector<int> *data;
};

using stack_val = std::variant<bool, long, double, string, List>;

template <class T>
using smallvec = boost::container::small_vector<T, 10>;

using stack_args = boost::container::small_vector<stack_val, 10>;

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

typedef stack_val (*ScriptFunction)(stack_args);

stack_val add(stack_args args)
{
    auto a = std::get<long>(args[0]);
    auto b = std::get<long>(args[1]);
    return a + b;
}

stack_val concat(stack_args args)
{
    auto a = std::get<string>(args[0]);
    auto b = std::get<string>(args[1]);
    return a + b;
}

stack_val some_function(stack_args args)
{
    for (const auto &arg : args)
    {
        std::visit(overloaded{
                       [](const long &q)
                       { std::cout << "- long: " << q << "\n"; },
                       [](const bool &q)
                       { std::cout << "- bool: " << q << "\n"; },
                       [](const double &q)
                       { std::cout << "- double: " << q << "\n"; },
                       [](const string &q)
                       { std::cout << "- string: " << q << "\n"; },
                       [](const List &q)
                       { std::cout << "- list"
                                   << "\n"; },
                   },
                   arg);
    }

    fmt::print("CALLED with {} args.\n", args.size());

    return true;
}

void print_val(stack_val val)
{
    std::visit(overloaded{
                   [](const long &q)
                   { std::cout << "long: " << q << "\n"; },
                   [](const bool &q)
                   { std::cout << "bool: " << q << "\n"; },
                   [](const double &q)
                   { std::cout << "double: " << q << "\n"; },
                   [](const string &q)
                   { std::cout << "string: " << q << "\n"; },
                   [](const List &q)
                   { std::cout << "list"
                               << "\n"; },
               },
               val);
}

optional<stack_val> call(tsl::robin_map<string, ScriptFunction> &map, string name, stack_args args)
{
    if (auto kv = map.find(name); kv != map.end())
    {
        fmt::print("found fn {}, result:\n", name);
        auto f = kv->second;

        auto result = f(args);
        print_val(result);
        return result;
    }

    return {};
}

int main()
{
    fmt::print("The answer is {}.\n", 42);

    auto good = lexy::zstring_input("#FF00FF");
    auto result = lexy::parse<grammar::color>(good, lexy_ext::report_error);
    if (result.has_value())
    {
        auto color = result.value();
        std::printf("#%02x%02x%02x\n", color.r, color.g, color.b);
    }
    else
    {
        printf("something went wrong\n");
    }

    printf("hello world\n");
    std::cout << "Hello World?!\n";

    tsl::robin_map<string, ScriptFunction> map = {};

    map.insert({"e", (ScriptFunction)&some_function});
    map.insert({"add", (ScriptFunction)&add});
    map.insert({"concat", (ScriptFunction)&concat});

    stack_args my_small_vec = {1, 2, true, 3, 8.0, 5};

    call(map, "e", my_small_vec);
    call(map, "add", {1, 5});
    call(map, "concat", {"hello", "world"});
}
