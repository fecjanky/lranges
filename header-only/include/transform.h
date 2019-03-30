#pragma once

#include <array>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace transformed {

template <typename T> struct TD;

template <typename RangeT> struct Range : private RangeT {

    Range()        = default;
    Range(Range&)  = default;
    Range(Range&&) = default;
    Range& operator=(const Range&) = default;
    Range& operator=(Range&&) = default;

    explicit Range(RangeT r)
        : RangeT(std::move(r))
    {
    }

    using iterator = typename RangeT::iterator;

    auto begin() { return range().begin(); }
    auto end() { return range().end(); }
    auto begin() const { return range().begin(); }
    auto end() const { return range().end(); }

private:
    RangeT&       range() { return static_cast<RangeT&>(*this); }
    const RangeT& range() const { return static_cast<const RangeT&>(*this); }
};

template <typename RangeT> struct Range<RangeT&> {
    Range()        = default;
    Range(Range&)  = default;
    Range(Range&&) = default;
    Range& operator=(const Range&) = default;
    Range& operator=(Range&&) = default;

    explicit Range(RangeT& _r)
        : r { &_r }
    {
    }
    using iterator = typename RangeT::iterator;
    auto begin() { return r->begin(); }
    auto end() { return r->end(); }
    auto begin() const { return r->begin(); }
    auto end() const { return r->end(); }

private:
    RangeT* r = nullptr;
};

template <typename RangeT, typename TransformationT> struct TransformationSequence {

    struct Iterator {
        using iterator          = typename RangeT::iterator;
        using traits            = std::iterator_traits<iterator>;
        using iterator_category = typename traits::iterator_category;
        using difference_type   = typename traits::difference_type;
        using value_type        = std::remove_reference_t<decltype(
            std::declval<TransformationT>()(std::declval<typename traits::value_type>()))>;
        using reference         = std::add_lvalue_reference_t<value_type>;
        using pointer           = std::add_pointer_t<value_type>;

        Iterator(TransformationSequence& _seq, iterator _it)
            : seq { &_seq }
            , it { std::move(_it) }
        {
        }
        decltype(auto) operator*() { return seq->tf(*it); }
        void           operator++() { ++it; }
        Iterator       operator++(int)
        {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        bool operator==(const Iterator& rhs) const { return it == rhs.it; }
        bool operator!=(const Iterator& rhs) const { return !((*this) == rhs); }

        TransformationSequence* seq;
        iterator                it;
    };

    using iterator = Iterator;

    TransformationSequence(RangeT l, TransformationT r)
        : range { std::move(l) }
        , tf { std::move(r) }
    {
    }

    auto begin() { return Iterator(*this, range.begin()); }
    auto end() { return Iterator(*this, range.end()); }

private:
    RangeT          range;
    TransformationT tf;
};

template <typename RangeT, typename FilterPredicate> struct FilteredSequence {

    struct Iterator {
        using iterator = typename RangeT::iterator;

        using traits            = std::iterator_traits<iterator>;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = typename traits::difference_type;
        using value_type        = typename traits::value_type;
        using difference_type   = typename traits::difference_type;
        using reference         = typename traits::reference;
        using pointer           = typename traits::pointer;

        Iterator(FilteredSequence& _seq, iterator _it)
            : seq { &_seq }
            , it { std::move(_it) }
        {
            next();
        }
        decltype(auto) operator*() { return *it; }
        void           operator++()
        {
            ++it;
            next();
        }
        Iterator operator++(int)
        {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        bool operator==(const Iterator& rhs) const { return it == rhs.it; }
        bool operator!=(const Iterator& rhs) const { return !((*this) == rhs); }

        FilteredSequence* seq;
        iterator          it;

    private:
        void next()
        {
            for (; it != seq->range.end() && !seq->tf(*it); ++it)
                ;
        }
    };

    using iterator = Iterator;

    FilteredSequence(RangeT l, FilterPredicate r)
        : range { std::move(l) }
        , tf { std::move(r) }
    {
    }

    auto begin() { return Iterator(*this, range.begin()); }
    auto end() { return Iterator(*this, range.end()); }

private:
    RangeT          range;
    FilterPredicate tf;
};

template <typename RangeT, typename TransformationT> auto operator*(RangeT&& r, TransformationT tf)
{
    using range = Range<RangeT>;
    return TransformationSequence<range, TransformationT>(
        range(std::forward<RangeT>(r)), std::move(tf));
}

template <typename RangeT, typename FilterT> auto operator%(RangeT&& r, FilterT tf)
{
    using range = Range<RangeT>;
    return FilteredSequence<range, FilterT>(range(std::forward<RangeT>(r)), std::move(tf));
}

}
