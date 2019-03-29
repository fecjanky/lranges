#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <headeronly.h>
#include <transform.h>

#include <iostream>
#include <sstream>
#include <vector>

TEST_CASE("test hello", "[hello]")
{
    std::stringstream ss;
    Sample::hello(ss, 4);
    REQUIRE(ss.str() == "Hello with value:4\n");
}

TEST_CASE("transform example", "[transform]")
{
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };

    using transformed::operator*;
    using transformed::operator%;

    transformed::Range<decltype(vec)&> r(vec);
    r.begin();

    transformed::Range<decltype(vec)> r2(vec);

    auto t = vec * [](auto val) { return val * val; } * [](auto val) { return val + 1; } %
        [](auto val) { return val % 5 == 0; } * [](auto val) { return val + 0.1; };
    std::vector<double> res;
    for (auto v : t) {
        res.push_back(v);
    }
    REQUIRE(res.size() == 2);
    REQUIRE(res[0] == 5.1);
    REQUIRE(res[1] == 10.1);
}

TEST_CASE("filter example", "[transform]")
{
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };

    using transformed::operator*;
    using transformed::operator%;

    auto             t = vec % [](auto val) { return val >= 3; };
    std::vector<int> res;

    for (auto v : t) {
        res.push_back(v);
    }
    REQUIRE(res.size() == 4);
    REQUIRE(res[0] == 3);
    REQUIRE(res[1] == 4);
    REQUIRE(res[2] == 5);
    REQUIRE(res[3] == 6);
}