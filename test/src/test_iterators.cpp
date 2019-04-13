#include <catch2/catch.hpp>

#include <lranges.h>

#include <forward_list>
#include <list>
#include <sstream>
#include <string>
#include <vector>

TEST_CASE("Transform iterator on input iterator", "[transform][iterator][input]")
{
    std::istringstream          iss("a b c");
    std::istream_iterator<char> begin { iss };
    std::istream_iterator<char> end {};
    using namespace lranges;

    auto upper = make_iterator_range(begin, end) | transform(toupper);

    static_assert(std::is_same<std::iterator_traits<decltype(upper.begin())>::iterator_category,
                      std::iterator_traits<decltype(begin)>::iterator_category>::value,
        "keeps iterator category");

    auto t_begin = upper.begin();
    auto t_end   = upper.end();
    REQUIRE(t_begin != t_end);
    REQUIRE(t_begin == t_begin);
    REQUIRE(*t_begin == 'A');
    ++t_begin;
    REQUIRE(*t_begin++ == 'B');
    REQUIRE(*t_begin == 'C');
    REQUIRE(*t_begin++ == 'C');
    REQUIRE(t_begin == t_end);
}

TEST_CASE("Transform iterator on forward iterator", "[transform][iterator][forward]")
{
    std::forward_list<char> list { 'a', 'b', 'c' };
    using namespace lranges;

    auto upper = list | transform(toupper);

    static_assert(std::is_same<std::iterator_traits<decltype(upper.begin())>::iterator_category,
                      std::iterator_traits<decltype(list)::iterator>::iterator_category>::value,
        "keeps iterator category");

    auto t_begin = upper.begin();
    auto t_end   = upper.end();
    REQUIRE(t_begin != t_end);
    REQUIRE(t_begin == t_begin);
    REQUIRE(*t_begin == 'A');
    auto t_temp_A = t_begin;
    ++t_begin;
    auto t_temp_B = t_begin;
    REQUIRE(*t_begin++ == 'B');
    REQUIRE(*t_begin == 'C');
    auto t_temp_C = t_begin;
    REQUIRE(*t_begin++ == 'C');
    REQUIRE(t_begin == t_end);
    REQUIRE(*t_temp_A == 'A');
    REQUIRE(*t_temp_B == 'B');
    REQUIRE(*t_temp_C == 'C');

    // Ensure multi - pass guarantee
    std::vector<char> multi_pass(upper.begin(), upper.end());
    REQUIRE(multi_pass.size() == 3);
    REQUIRE(multi_pass[0] == 'A');
    REQUIRE(multi_pass[1] == 'B');
    REQUIRE(multi_pass[2] == 'C');
}

TEST_CASE("Transform iterator on bidirectional iterator", "[transform][iterator][bi-dir]")
{
    std::list<char> list { 'a', 'b', 'c' };
    using namespace lranges;

    auto upper = list | transform(toupper);

    static_assert(std::is_same<std::iterator_traits<decltype(upper.begin())>::iterator_category,
                      std::iterator_traits<decltype(list)::iterator>::iterator_category>::value,
        "keeps iterator category");

    auto t_begin = upper.begin();
    auto t_end   = upper.end();
    REQUIRE(t_begin != t_end);
    REQUIRE(t_begin == t_begin);
    REQUIRE(*t_begin == 'A');
    auto t_temp_A = t_begin;
    ++t_begin;
    auto t_temp_B = t_begin;
    REQUIRE(*t_begin++ == 'B');
    REQUIRE(*t_begin == 'C');
    --t_begin;
    REQUIRE(*t_begin == 'B');
    ++t_begin;
    auto t_temp_C = t_begin;
    REQUIRE(*t_begin++ == 'C');
    REQUIRE(t_begin-- == t_end);
    REQUIRE(*t_begin++ == 'C');
    REQUIRE(t_begin == t_end);
    REQUIRE(*t_temp_A == 'A');
    REQUIRE(*t_temp_B == 'B');
    REQUIRE(*t_temp_C == 'C');
    REQUIRE(*t_temp_C-- == 'C');
    REQUIRE(*t_temp_C == 'B');

    // Ensure multi - pass guarantee
    std::vector<char> multi_pass(upper.begin(), upper.end());
    REQUIRE(multi_pass.size() == 3);
    REQUIRE(multi_pass[0] == 'A');
    REQUIRE(multi_pass[1] == 'B');
    REQUIRE(multi_pass[2] == 'C');
}

TEST_CASE("Transform iterator on random access iterator", "[transform][iterator][random-access]")
{
    std::vector<char> list { 'a', 'b', 'c' };
    using namespace lranges;

    auto upper = list | transform(toupper);

    static_assert(std::is_same<std::iterator_traits<decltype(upper.begin())>::iterator_category,
                      std::iterator_traits<decltype(list)::iterator>::iterator_category>::value,
        "keeps iterator category");

    auto t_begin = upper.begin();
    auto t_end   = upper.end();
    REQUIRE(t_begin != t_end);
    REQUIRE(t_begin == t_begin);
    REQUIRE(*t_begin == 'A');
    auto t_temp_A = t_begin;
    ++t_begin;
    auto t_temp_B = t_begin;
    REQUIRE(*t_begin++ == 'B');
    REQUIRE(*t_begin == 'C');
    --t_begin;
    REQUIRE(*t_begin == 'B');
    ++t_begin;
    auto t_temp_C = t_begin;
    REQUIRE(*t_begin++ == 'C');
    REQUIRE(t_begin-- == t_end);
    REQUIRE(*t_begin++ == 'C');
    REQUIRE(t_begin == t_end);
    REQUIRE(*t_temp_A == 'A');
    t_temp_A += 2;
    REQUIRE(*t_temp_A == 'C');
    t_temp_A -= 2;
    REQUIRE(*t_temp_A == 'A');
    REQUIRE(t_temp_A[2] == 'C');
    REQUIRE(*t_temp_B == 'B');
    REQUIRE(*(t_temp_B + 1) == 'C');
    REQUIRE(*(1 + t_temp_B) == 'C');
    REQUIRE(*(t_temp_B - 1) == 'A');
    REQUIRE((t_temp_C - t_temp_A) == 2);
    REQUIRE(*t_temp_C == 'C');
    REQUIRE(*t_temp_C-- == 'C');
    REQUIRE(*t_temp_C == 'B');

    REQUIRE(t_temp_A < t_temp_B);
    REQUIRE(t_temp_A <= t_temp_A);
    REQUIRE(t_temp_A <= t_temp_B);
    REQUIRE(t_temp_B > t_temp_A);
    REQUIRE(t_temp_B >= t_temp_A);
    REQUIRE(t_temp_A >= t_temp_A);

    // Ensure multi - pass guarantee
    std::vector<char> multi_pass(upper.begin(), upper.end());
    REQUIRE(multi_pass.size() == 3);
    REQUIRE(multi_pass[0] == 'A');
    REQUIRE(multi_pass[1] == 'B');
    REQUIRE(multi_pass[2] == 'C');
}

TEST_CASE("Transform iterator on random access iterator multi-stage",
    "[transform][iterator][random-access]")
{
    std::vector<char> list { 'a', 'b', 'c' };
    using namespace lranges;

    auto upper = list | transform(toupper) | transform([](char c) { return ++c; });

    static_assert(std::is_same<std::iterator_traits<decltype(upper.begin())>::iterator_category,
                      std::iterator_traits<decltype(list)::iterator>::iterator_category>::value,
        "keeps iterator category");

    auto t_begin = upper.begin();
    auto t_end   = upper.end();
    REQUIRE(t_begin != t_end);
    REQUIRE(t_begin == t_begin);
    REQUIRE(*t_begin == 'B');
    auto t_temp_A = t_begin;
    ++t_begin;
    auto t_temp_B = t_begin;
    REQUIRE(*t_begin++ == 'C');
    REQUIRE(*t_begin == 'D');
    --t_begin;
    REQUIRE(*t_begin == 'C');
    ++t_begin;
    auto t_temp_C = t_begin;
    REQUIRE(*t_begin++ == 'D');
    REQUIRE(t_begin-- == t_end);
    REQUIRE(*t_begin++ == 'D');
    REQUIRE(t_begin == t_end);
    REQUIRE(*t_temp_A == 'B');
    t_temp_A += 2;
    REQUIRE(*t_temp_A == 'D');
    t_temp_A -= 2;
    REQUIRE(*t_temp_A == 'B');
    REQUIRE(t_temp_A[2] == 'D');
    REQUIRE(*t_temp_B == 'C');
    REQUIRE(*(t_temp_B + 1) == 'D');
    REQUIRE(*(1 + t_temp_B) == 'D');
    REQUIRE(*(t_temp_B - 1) == 'B');
    REQUIRE((t_temp_C - t_temp_A) == 2);
    REQUIRE(*t_temp_C == 'D');
    REQUIRE(*t_temp_C-- == 'D');
    REQUIRE(*t_temp_C == 'C');

    REQUIRE(t_temp_A < t_temp_B);
    REQUIRE(t_temp_A <= t_temp_A);
    REQUIRE(t_temp_A <= t_temp_B);
    REQUIRE(t_temp_B > t_temp_A);
    REQUIRE(t_temp_B >= t_temp_A);
    REQUIRE(t_temp_A >= t_temp_A);

    // Ensure multi - pass guarantee
    std::vector<char> multi_pass(upper.begin(), upper.end());
    REQUIRE(multi_pass.size() == 3);
    REQUIRE(multi_pass[0] == 'B');
    REQUIRE(multi_pass[1] == 'C');
    REQUIRE(multi_pass[2] == 'D');
}

TEST_CASE("Filter drops to bidir iterator category", "[filter][iterator][random-access]")
{
    std::vector<char> list { 'a', 'b', 'c' };
    using namespace lranges;

    auto upper = list | transform(toupper) | filter([](char c) { return c > 'A'; })
        | transform([](char c) { return ++c; });

    static_assert(std::is_same<std::iterator_traits<decltype(upper.begin())>::iterator_category,
                      std::bidirectional_iterator_tag>::value,
        "drops to forward iterator category");

    auto t_begin = upper.begin();
    auto t_end   = upper.end();
    REQUIRE(t_begin != t_end);
    REQUIRE(t_begin == t_begin);
    REQUIRE(*t_begin == 'C');
    auto t_temp_C = t_begin;
    ++t_begin;
    auto t_temp_D = t_begin;
    REQUIRE(*t_begin++ == 'D');
    REQUIRE(t_begin == t_end);
    REQUIRE(*t_temp_D-- == 'D');
    REQUIRE(*t_temp_D == 'C');
    ++t_temp_D;
    REQUIRE(*t_temp_D-- == 'D');
    --t_begin;
    REQUIRE(*t_begin == 'D');
    ++t_begin;
    REQUIRE(t_begin == t_end);

    // Ensure multi - pass guarantee
    std::vector<char> multi_pass(upper.begin(), upper.end());
    REQUIRE(multi_pass.size() == 2);
    REQUIRE(multi_pass[0] == 'C');
    REQUIRE(multi_pass[1] == 'D');
}

TEST_CASE("Filter iterator post-fix decrement",
    "[filter][iterator][API]")
{
    std::list<int>          iss{'a','b', 'c'};
    using namespace lranges;

    auto upper = make_iterator_range(iss.begin(), iss.end()) | filter([](char c) { return c > 'a'; });

    using t = std::iterator_traits<decltype(upper.begin())>::iterator_category;
    static_assert(std::is_same<std::iterator_traits<decltype(upper.begin())>::iterator_category,
                      std::bidirectional_iterator_tag>::value,
        "keeps iterator category");

    auto t_begin = upper.begin();
    auto t_end   = upper.end();
    REQUIRE(t_begin != t_end);
    REQUIRE(t_begin == t_begin);
    REQUIRE(*t_begin == 'b');
    ++t_begin;
    REQUIRE(*t_begin-- == 'c');
    REQUIRE(*t_begin == 'b');
    ++t_begin;
    REQUIRE(*t_begin++ == 'c');
    REQUIRE(t_begin == t_end);
}

TEST_CASE("Filter iterator keeps iterator category of the iterator up until bi-dir",
    "[filter][iterator][input]")
{
    std::istringstream          iss("a b c");
    std::istream_iterator<char> begin { iss };
    std::istream_iterator<char> end {};
    using namespace lranges;

    auto upper = make_iterator_range(begin, end) | transform(toupper)
        | filter([](char c) { return c > 'A'; });

    using t = std::iterator_traits<decltype(upper.begin())>::iterator_category;
    static_assert(std::is_same<std::iterator_traits<decltype(upper.begin())>::iterator_category,
                      std::iterator_traits<decltype(begin)>::iterator_category>::value,
        "keeps iterator category");

    auto t_begin = upper.begin();
    auto t_end   = upper.end();
    REQUIRE(t_begin != t_end);
    REQUIRE(t_begin == t_begin);
    REQUIRE(*t_begin == 'B');
    ++t_begin;
    REQUIRE(*t_begin++ == 'C');
    REQUIRE(t_begin == t_end);
}

TEST_CASE("min on ordered types", "[meta]")
{
    using namespace lranges;
    using order = detail::Ordered<char, int, double>;
    static_assert(std::is_same<order::min<char, char>::type, char>::value, "min: char,char -> char");
    static_assert(std::is_same<order::min<char, int>::type, char>::value, "min: char,int -> char");
    static_assert(std::is_same<order::min<int, char>::type, char>::value, "min: int,char -> char");
    static_assert(std::is_same<order::min<double, int>::type, int>::value, "min: double,int -> int");
}
