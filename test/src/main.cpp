#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <lranges.h>

#include <iostream>
#include <sstream>
#include <vector>

TEST_CASE("transform example", "[transform]")
{
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };

    using lranges::filter;
    using lranges::transform;

    auto t = vec | transform([](auto val) { return val * val; })
        | transform([](auto val) { return val + 1; })
        | filter([](auto val) { return val % 5 == 0; })
        | transform([](auto val) { return val + 0.1; });
    std::vector<double> res;

    std::copy(t.begin(), t.end(), std::back_inserter(res));

    REQUIRE(res.size() == 2);
    REQUIRE(res[0] == 5.1);
    REQUIRE(res[1] == 10.1);
}

TEST_CASE("filter example", "[transform]")
{
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };

    using lranges::filter;
    using lranges::transform;

    auto t = vec | transform([](auto val) { return val + 1; })
        | filter([](auto val) { return val >= 3; });
    std::vector<int> res;
    std::copy(t.begin(), t.end(), std::back_inserter(res));

    REQUIRE(res.size() == 5);
    REQUIRE(res[0] == 3);
    REQUIRE(res[1] == 4);
    REQUIRE(res[2] == 5);
    REQUIRE(res[3] == 6);
    REQUIRE(res[4] == 7);
}

int  plus_1(int val) { return val + 1; }
bool greater_than_3(int val) { return val >= 3; }

TEST_CASE("transform and filter by freestanding func", "[transform]")
{
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };

    using lranges::filter;
    using lranges::transform;

    auto             t = vec | transform(plus_1) | filter(greater_than_3);
    std::vector<int> res;
    std::copy(t.begin(), t.end(), std::back_inserter(res));

    REQUIRE(res.size() == 5);
    REQUIRE(res[0] == 3);
    REQUIRE(res[1] == 4);
    REQUIRE(res[2] == 5);
    REQUIRE(res[3] == 6);
    REQUIRE(res[4] == 7);
}

TEST_CASE("transform and filter by freestanding ptr to mem", "[transform]")
{
    struct Baz {
        int  val;
        auto greater_than_3() const { return val >= 3; }
    };
    struct Foo {
        auto toBaz() const { return Baz { val + 1 }; }
        auto greater_than_3() const { return val >= 3; }
        int  val = 0;
    };

    std::vector<Foo> vec { Foo { 1 }, Foo { 2 }, Foo { 3 }, Foo { 4 }, Foo { 5 }, Foo { 6 } };

    using lranges::filter;
    using lranges::transform;

    auto t         = vec | transform(&Foo::toBaz) | filter(&Baz::greater_than_3);
    using iterator = decltype(t.begin());
    using deref    = decltype(*t.begin());
    static_assert(std::is_same<iterator::value_type, Baz>::value, "Oh no...");
    static_assert(std::is_same<deref, Baz>::value, "Oh no...");
    std::vector<Baz> res;
    std::copy(t.begin(), t.end(), std::back_inserter(res));

    REQUIRE(res.size() == 5);
    REQUIRE(res[0].val == 3);
    REQUIRE(res[1].val == 4);
    REQUIRE(res[2].val == 5);
    REQUIRE(res[3].val == 6);
    REQUIRE(res[4].val == 7);
}

#include <array>

struct isEven {
    bool operator()(int val) const { return val % 2 == 0; }
};

struct Mul {
    int operator()(int val) const { return val * 2 + 1; }
};

std::array<int, 4> func_lranges(std::array<int, 4>& a)
{
    using lranges::filter;
    using lranges::transform;
    std::array<int, 4> arr_out;
    arr_out.fill(0);
    auto out = arr_out.begin();
    auto t   = a | filter(isEven {}) | transform(Mul {});
    for (auto it = t.begin(), end = t.end(); it != end; ++it, ++out) {
        *out = *it;
    }
    return arr_out;
}

TEST_CASE("Sample")
{
    std::array<int, 4> arr { 1, 2, 3, 4 };
    auto               res = func_lranges(arr);
    REQUIRE(res[0] == 5);
    REQUIRE(res[1] == 9);
}
