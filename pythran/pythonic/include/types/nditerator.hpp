#ifndef PYTHONIC_INCLUDE_TYPES_NDITERATOR_HPP
#define PYTHONIC_INCLUDE_TYPES_NDITERATOR_HPP

#include <iterator>
#ifdef USE_BOOST_SIMD
#include <boost/simd/pack.hpp>
#endif

namespace pythonic
{

  namespace types
  {
    struct fast {
    };

    template <class T>
    auto fast_begin(T const &e) ->
        typename std::enable_if<has_fast_iterator<T>::value,
                                decltype(e.begin(fast{}))>::type
    {
      return e.begin(fast{});
    }
    template <class T>
    auto fast_begin(T const &e) ->
        typename std::enable_if<!has_fast_iterator<T>::value,
                                decltype(e.begin())>::type
    {
      return e.begin();
    }
    template <class T>
    auto fast_end(T const &e) ->
        typename std::enable_if<has_fast_iterator<T>::value,
                                decltype(e.end(fast{}))>::type
    {
      return e.end(fast{});
    }
    template <class T>
    auto fast_end(T const &e) ->
        typename std::enable_if<!has_fast_iterator<T>::value,
                                decltype(e.end())>::type
    {
      return e.end();
    }

    template <class E>
    struct fast_range {
      E const &container_;

      using dtype = typename E::dtype;
      static constexpr auto value = E::value;
      static constexpr auto is_vectorizable = E::is_vectorizable;

      operator E const &() const
      {
        return container_;
      }

      fast_range(E const &range) : container_(range)
      {
      }
      auto begin() const -> decltype(fast_begin(this->container_))
      {
        return fast_begin(container_);
      }
      auto end() const -> decltype(fast_end(this->container_))
      {
        return fast_end(container_);
      }
    };

    template <class E>
    auto make_fast_range(E const &e) ->
        typename std::enable_if<is_iterable<E>::value, fast_range<E>>::type
    {
      return {e};
    }
    template <class E>
    auto make_fast_range(E const &e) ->
        typename std::enable_if<!is_iterable<E>::value, E const &>::type
    {
      return e;
    }

    /* Iterator over whatever provides a fast(long) method to access its element
     */
    template <class E>
    struct nditerator
        : public std::iterator<std::random_access_iterator_tag,
                               typename std::remove_reference<decltype(
                                   std::declval<E &>().fast(0))>::type> {
      E &data;
      long index;
      nditerator(E &data, long index);

      auto operator*() -> decltype(data.fast(index));
      auto operator*() const -> decltype(data.fast(index));
      nditerator<E> &operator++();
      nditerator<E> &operator--();
      nditerator<E> &operator+=(long i);
      nditerator<E> &operator-=(long i);
      nditerator<E> operator+(long i) const;
      nditerator<E> operator-(long i) const;
      long operator-(nditerator<E> const &other) const;
      bool operator!=(nditerator<E> const &other) const;
      bool operator==(nditerator<E> const &other) const;
      bool operator<(nditerator<E> const &other) const;
      nditerator &operator=(nditerator const &other);
    };

    /* Const iterator over whatever provides a fast(long) method to access its
     * element
     */
    template <class E>
    struct const_nditerator
        : public std::iterator<std::random_access_iterator_tag,
                               typename std::remove_reference<decltype(
                                   std::declval<E &>().fast(0))>::type> {
      E const &data;
      long index;
      const_nditerator(E const &data, long index);

      auto operator*() const -> decltype(data.fast(index));
      const_nditerator<E> &operator++();
      const_nditerator<E> &operator--();
      const_nditerator<E> &operator+=(long i);
      const_nditerator<E> &operator-=(long i);
      const_nditerator<E> operator+(long i) const;
      const_nditerator<E> operator-(long i) const;
      long operator-(const_nditerator<E> const &other) const;
      bool operator!=(const_nditerator<E> const &other) const;
      bool operator==(const_nditerator<E> const &other) const;
      bool operator<(const_nditerator<E> const &other) const;
      const_nditerator &operator=(const_nditerator const &other);
    };
#ifdef USE_BOOST_SIMD
    template <class E>
    struct const_simd_nditerator
        : public std::iterator<std::random_access_iterator_tag,
                               typename std::remove_reference<decltype(
                                   std::declval<E &>().load(0))>::type> {

      E const &data;
      long index;

      using vector_type = typename boost::simd::pack<typename E::dtype>;
      static const std::size_t vector_size = vector_type::static_size;

      const_simd_nditerator(E const &data, long index);

      auto operator*() const -> decltype(data.load(index));
      const_simd_nditerator &operator++();
      const_simd_nditerator &operator--();
      const_simd_nditerator &operator+=(long i);
      const_simd_nditerator &operator-=(long i);
      const_simd_nditerator operator+(long i) const;
      const_simd_nditerator operator-(long i) const;
      long operator-(const_simd_nditerator const &other) const;
      bool operator!=(const_simd_nditerator const &other) const;
      bool operator==(const_simd_nditerator const &other) const;
      bool operator<(const_simd_nditerator const &other) const;
      const_simd_nditerator &operator=(const_simd_nditerator const &other);
    };
#endif

    // build an iterator over T, selecting a raw pointer if possible
    template <bool is_strided>
    struct make_nditerator {
      template <class T>
      auto operator()(T &self, long i)
          -> decltype(nditerator<T>(self, i)) const;
    };

    template <>
    struct make_nditerator<false> {
      template <class T>
      typename T::dtype *operator()(T &self, long i) const;
    };

    template <bool is_strided>
    struct make_const_nditerator {
      template <class T>
      auto operator()(T const &self, long i)
          -> decltype(const_nditerator<T>(self, i)) const;
    };

    template <>
    struct make_const_nditerator<false> {
      template <class T>
      typename T::dtype const *operator()(T const &self, long i) const;
    };
  }
}

#endif
