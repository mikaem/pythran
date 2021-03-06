#ifndef PYTHONIC_INCLUDE_NUMPY_MAX_HPP
#define PYTHONIC_INCLUDE_NUMPY_MAX_HPP

#include "pythonic/include/numpy/reduce.hpp"
#include "pythonic/include/operator_/imax.hpp"
#include "pythonic/include/utils/functor.hpp"

namespace pythonic
{

  namespace numpy
  {
    template <class... Args>
    auto max(Args &&... args) -> decltype(
        reduce<operator_::functor::imax>(std::forward<Args>(args)...));

    DECLARE_FUNCTOR(pythonic::numpy, max);
  }
}

#endif
