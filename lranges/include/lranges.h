#pragma once

#include <algorithm>
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

        /*Iterator API (Input, Forward)*/
        decltype(auto) operator*() { return seq->transformation()(*it); }
        decltype(auto) operator*() const { return seq->transformation()(*it); }
        auto           operator==(const TransformationIterator& rhs) const { return it == rhs.it; }
        auto operator!=(const TransformationIterator& rhs) const { return !((*this) == rhs); }

        decltype(auto) operator++()
        {
            ++it;
            return *this;
        }
        auto operator++(int)
        {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        /*Bi-directional iterator API*/
        decltype(auto) operator--()
        {
            --it;
            return *this;
        }
        auto operator--(int)
        {
            auto temp = *this;
            --(*this);
            return temp;
        }

        /*Random-access API*/
        decltype(auto) operator+=(difference_type n)
        {
            it += n;
            return *this;
        }
        decltype(auto) operator-=(difference_type n)
        {
            it -= n;
            return *this;
        }
        decltype(auto) operator[](difference_type n) { return seq->transformation()(it[n]); }
        decltype(auto) operator[](difference_type n) const { return seq->transformation()(it[n]); }
        auto           operator-(const TransformationIterator& rhs) const { return it - rhs.it; }
        auto           operator<(const TransformationIterator& rhs) const { return it < rhs.it; }
        auto           operator<=(const TransformationIterator& rhs) const { return it <= rhs.it; }
        auto           operator>(const TransformationIterator& rhs) const { return it > rhs.it; }
        auto           operator>=(const TransformationIterator& rhs) const { return it >= rhs.it; }

        sequence_t* seq;
        iterator    it;
    };

    /*Random-access API*/
    template <typename R, typename T>
    auto operator-(TransformationIterator<R, T> const&         it,
        typename TransformationIterator<R, T>::difference_type n)
    {
        auto temp = it;
        temp -= n;
        return temp;
    }

    template <typename R, typename T>
    auto operator+(TransformationIterator<R, T> const&         it,
        typename TransformationIterator<R, T>::difference_type n)
    {
        auto temp = it;
        temp += n;
        return temp;
    }

    template <typename R, typename T>
    auto operator+(typename TransformationIterator<R, T>::difference_type n,
        TransformationIterator<R, T> const&                               it)
    {
        auto temp = it;
        temp += n;
        return temp;
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

    template <size_t I, typename T, typename... Ts> struct index_of {
        template <typename U> struct TYPE_MISSING_FROM_SEQUENCE_ERROR;
        static constexpr auto value = TYPE_MISSING_FROM_SEQUENCE_ERROR<T>::value;
    };

    template <size_t I, typename T, typename... Rest> struct index_of<I, T, T, Rest...> {
        static constexpr auto value = I;
    };

    template <size_t I, typename T, typename First, typename... Rest>
    struct index_of<I, T, First, Rest...> {
        static constexpr auto value = index_of<I + 1, T, Rest...>::value;
    };

    template <typename... Categories> struct Ordered {
        template <typename T1, typename T2> struct min {
        private:
            static constexpr auto t1_idx = index_of<0, T1, Categories...>::value;
            static constexpr auto t2_idx = index_of<0, T2, Categories...>::value;

        public:
            using type = std::conditional_t < t1_idx<t2_idx && !(t2_idx < t1_idx), T1, T2>;
        };
    };

    using iterator_ordering = Ordered<std::input_iterator_tag, std::forward_iterator_tag,
        std::bidirectional_iterator_tag, std::random_access_iterator_tag>;

    template <typename T1, typename T2>
    using iterator_min_t = typename iterator_ordering::min<T1, T2>::type;

    template <typename RangeT, typename FilterPredicate> struct FilterIterator {
        using iterator = typename RangeT::iterator;

        using traits = std::iterator_traits<iterator>;

        using iterator_category
            = iterator_min_t<typename traits::iterator_category, std::bidirectional_iterator_tag>;
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

        /*Iterator API (Input, Forward)*/

        decltype(auto) operator*() { return *it; }
        decltype(auto) operator*() const { return *it; }
        auto           operator==(const FilterIterator& rhs) const { return it == rhs.it; }
        auto           operator!=(const FilterIterator& rhs) const { return !((*this) == rhs); }

        void operator++()
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

        /*Bi-directional iterator API*/
        decltype(auto) operator--()
        {
            --it;
            return *this;
        }
        auto operator--(int)
        {
            auto temp = *this;
            --(*this);
            return temp;
        }

        filtered_sequence_t* seq;
        iterator             it;

    private:
        void next() { it = std::find_if(it, seq->range().end(), seq->filter()); }
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

template <typename Iterator> struct iterator_range {

    using iterator = Iterator;

    iterator_range(Iterator begin, Iterator end)
        : _begin { begin }
        , _end { end }
    {
    }

    auto begin() const { return _begin; }
    auto end() const { return _end; }

    Iterator _begin;
    Iterator _end;
};

template <typename Iterator> auto make_iterator_range(Iterator b, Iterator e)
{
    return iterator_range<Iterator>(std::move(b), std::move(e));
}

}
