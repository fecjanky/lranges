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

template <typename RangeT, typename TransformationT> struct TransformationIterator;

template <typename RangeT, typename TransformationT> struct TransformationSequence {

    using iterator = TransformationIterator<RangeT, TransformationT>;

    TransformationSequence(RangeT l, TransformationT r)
        : range { std::move(l) }
        , tf { std::move(r) }
    {
    }

    auto begin() { return iterator(*this, range.begin()); }
    auto end() { return iterator(*this, range.end()); }

    const TransformationT& transformation() const { return tf; }
    TransformationT&       transformation() { return tf; }

private:
    RangeT          range;
    TransformationT tf;
};

template <typename RangeT, typename TransformationT> struct TransformationIterator {
    using iterator          = typename RangeT::iterator;
    using traits            = std::iterator_traits<iterator>;
    using iterator_category = typename traits::iterator_category;
    using difference_type   = typename traits::difference_type;
    using value_type        = std::remove_reference_t<decltype(
        std::declval<TransformationT>()(std::declval<typename traits::value_type>()))>;
    using reference         = std::add_lvalue_reference_t<value_type>;
    using pointer           = std::add_pointer_t<value_type>;
    using sequence_t        = TransformationSequence<RangeT, TransformationT>;

    TransformationIterator(sequence_t& _seq, iterator _it)
        : seq { &_seq }
        , it { std::move(_it) }
    {
    }
    decltype(auto)         operator*() { return seq->transformation()(*it); }
    void                   operator++() { ++it; }
    TransformationIterator operator++(int)
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }
    bool operator==(const TransformationIterator& rhs) const { return it == rhs.it; }
    bool operator!=(const TransformationIterator& rhs) const { return !((*this) == rhs); }

    sequence_t* seq;
    iterator    it;
};

template <typename R, typename T>
auto operator-(TransformationIterator<R, T> const& lhs, TransformationIterator<R, T> const& rhs)
{
    return lhs.it - rhs.it;
}

template <typename R, typename T>
auto operator<(TransformationIterator<R, T> const& lhs, TransformationIterator<R, T> const& rhs)
{
    return lhs.it < rhs.it;
}

template <typename RangeT, typename FilterPredicate> struct FilterIterator;

template <typename RangeT, typename FilterPredicate> struct FilteredSequence {

    using iterator = FilterIterator<RangeT, FilterPredicate>;

    FilteredSequence(RangeT l, FilterPredicate r)
        : _range { std::move(l) }
        , _filter { std::move(r) }
    {
    }

    auto                   begin() { return iterator(*this, _range.begin()); }
    auto                   end() { return iterator(*this, _range.end()); }
    FilterPredicate&       filter() { return _filter; }
    const FilterPredicate& filter() const { return _filter; }
    RangeT&                range() { return _range; }
    const RangeT&          range() const { return _range; }

private:
    RangeT          _range;
    FilterPredicate _filter;
};

template <typename RangeT, typename FilterPredicate> struct FilterIterator {
    using iterator = typename RangeT::iterator;

    using traits              = std::iterator_traits<iterator>;
    using iterator_category   = std::forward_iterator_tag;
    using difference_type     = typename traits::difference_type;
    using value_type          = typename traits::value_type;
    using difference_type     = typename traits::difference_type;
    using reference           = typename traits::reference;
    using pointer             = typename traits::pointer;
    using filtered_sequence_t = FilteredSequence<RangeT, FilterPredicate>;

    FilterIterator(filtered_sequence_t& _seq, iterator _it)
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
    FilterIterator operator++(int)
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }
    bool operator==(const FilterIterator& rhs) const { return it == rhs.it; }
    bool operator!=(const FilterIterator& rhs) const { return !((*this) == rhs); }

    filtered_sequence_t* seq;
    iterator             it;

private:
    void next()
    {
        for (; it != seq->range().end() && !seq->filter()(*it); ++it)
            ;
    }
};

template <typename F> struct Transformation : public F {
    Transformation() = default;
    Transformation(F&& f)
        : F(std::move(f))
    {
    }
    Transformation(const F& f)
        : F(f)
    {
    }
};

template <typename F> struct Filter : public F {
    Filter() = default;
    Filter(F&& f)
        : F(std::move(f))
    {
    }
    Filter(const F& f)
        : F(f)
    {
    }
};

template <typename TransformationT> auto transform(TransformationT tf)
{
    return Transformation<TransformationT>(std::move(tf));
}

template <typename FilterT> auto filter(FilterT tf) { return Filter<FilterT>(std::move(tf)); }

template <typename RangeT, typename TransformationT>
auto operator|(RangeT&& r, Transformation<TransformationT> tf)
{
    using range = Range<RangeT>;
    return TransformationSequence<range, TransformationT>(
        range(std::forward<RangeT>(r)), std::move(tf));
}

template <typename RangeT, typename FilterT> auto operator|(RangeT&& r, Filter<FilterT> tf)
{
    using range = Range<RangeT>;
    return FilteredSequence<range, FilterT>(range { std::forward<RangeT>(r) }, std::move(tf));
}

}
