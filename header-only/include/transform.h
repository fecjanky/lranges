#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

namespace lranges {
namespace detail {

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

    template <typename RangeT, typename TransformationT>
    struct TransformedRange : private RangeT, private TransformationT {

        using iterator = TransformationIterator<RangeT, TransformationT>;

        TransformedRange(RangeT l, TransformationT r)
            : RangeT { std::move(l) }
            , TransformationT { std::move(r) }
        {
        }

        auto begin() { return iterator(*this, range().begin()); }
        auto end() { return iterator(*this, range().end()); }

        decltype(auto) transformation() { return static_cast<TransformationT&>(*this); }
        decltype(auto) transformation() const { return static_cast<const TransformationT&>(*this); }
        decltype(auto) range() { return static_cast<RangeT&>(*this); }
        decltype(auto) range() const { return static_cast<const RangeT&>(*this); }
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
        using sequence_t        = TransformedRange<RangeT, TransformationT>;

        TransformationIterator(sequence_t& _seq, iterator _it)
            : seq { &_seq }
            , it { std::move(_it) }
        {
        }
        decltype(auto) operator*() { return seq->transformation()(*it); }
        void           operator++() { ++it; }
        auto           operator++(int)
        {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        auto operator==(const TransformationIterator& rhs) const { return it == rhs.it; }
        auto operator!=(const TransformationIterator& rhs) const { return !((*this) == rhs); }

        sequence_t* seq;
        iterator    it;
    };

    // TODO(fecjanky):add remaining iterator operators
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

    template <typename RangeT, typename FilterPredicate>
    struct FilteredRange : private RangeT, private FilterPredicate {

        using iterator = FilterIterator<RangeT, FilterPredicate>;

        FilteredRange(RangeT l, FilterPredicate r)
            : RangeT { std::move(l) }
            , FilterPredicate { std::move(r) }
        {
        }

        auto           begin() { return iterator(*this, range().begin()); }
        auto           end() { return iterator(*this, range().end()); }
        decltype(auto) filter() { return static_cast<FilterPredicate&>(*this); }
        decltype(auto) filter() const { return static_cast<const FilterPredicate&>(*this); }
        decltype(auto) range() { return static_cast<RangeT&>(*this); }
        decltype(auto) range() const { return static_cast<const RangeT&>(*this); }

    private:
    };

    template <typename RangeT, typename FilterPredicate> struct FilterIterator {
        using iterator = typename RangeT::iterator;

        using traits              = std::iterator_traits<iterator>;
        using iterator_category   = std::forward_iterator_tag;
        using difference_type     = typename traits::difference_type;
        using value_type          = typename traits::value_type;
        using reference           = typename traits::reference;
        using pointer             = typename traits::pointer;
        using filtered_sequence_t = FilteredRange<RangeT, FilterPredicate>;

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
        auto operator++(int)
        {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        auto operator==(const FilterIterator& rhs) const { return it == rhs.it; }
        auto operator!=(const FilterIterator& rhs) const { return !((*this) == rhs); }

        filtered_sequence_t* seq;
        iterator             it;

    private:
        void next()
        {
            for (auto end = seq->range().end(); it != end && !seq->filter()(*it); ++it)
                ;
        }
    };

    template <typename F, typename = void> struct FuncWrapper : public F {
        FuncWrapper() = default;
        FuncWrapper(F&& f)
            : F(std::move(f))
        {
        }
        FuncWrapper(const F& f)
            : F(f)
        {
        }
    };

    template <typename R, typename Arg> struct FuncWrapper<R(Arg)> {
        using signature = R(Arg);
        FuncWrapper()   = default;
        explicit FuncWrapper(signature func)
            : fptr { func }
        {
        }

        template <typename UArg> R operator()(UArg&& arg) const
        {
            return fptr(std::forward<UArg>(arg));
        }

    private:
        signature* fptr = nullptr;
    };

    template <typename PMemFun>
    struct FuncWrapper<PMemFun, std::enable_if_t<std::is_member_function_pointer<PMemFun>::value>> {
        FuncWrapper() = default;
        explicit FuncWrapper(PMemFun func)
            : fptr { func }
        {
        }

        template <typename UArg> decltype(auto) operator()(UArg&& arg) const
        {
            return (std::forward<UArg>(arg).*fptr)();
        }

    private:
        PMemFun fptr = nullptr;
    };

    template <typename F> struct Transformation : public FuncWrapper<F> {
        using FuncWrapper<F>::FuncWrapper;
    };

    template <typename F> struct Filter : public FuncWrapper<F> {
        using FuncWrapper<F>::FuncWrapper;
    };

    template <typename RangeT, typename TransformationT>
    auto operator|(RangeT&& r, Transformation<TransformationT> tf)
    {
        using range          = Range<RangeT>;
        using transformation = decltype(tf);
        return TransformedRange<range, transformation>(
            range(std::forward<RangeT>(r)), std::move(tf));
    }

    template <typename RangeT, typename FilterT> auto operator|(RangeT&& r, Filter<FilterT> tf)
    {
        using range          = Range<RangeT>;
        using transformation = decltype(tf);
        return FilteredRange<range, transformation>(
            range { std::forward<RangeT>(r) }, std::move(tf));
    }
}

template <typename TransformationT> auto transform(TransformationT&& tf)
{
    return detail::Transformation<std::remove_reference_t<TransformationT>>(
        std::forward<TransformationT>(tf));
}

template <typename FilterT> auto filter(FilterT&& tf)
{
    return detail::Filter<std::remove_reference_t<FilterT>>(std::forward<FilterT>(tf));
}

}
