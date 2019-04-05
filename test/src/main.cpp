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
