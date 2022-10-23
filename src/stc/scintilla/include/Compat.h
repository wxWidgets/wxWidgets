// c++11 compatibility with some c++14 features and higher.
// This helps minimize changes from the default branch.

#ifndef COMPAT_H
#define COMPAT_H

#ifdef __cplusplus

#include <cmath>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace Sci {

// std::clamp
template <typename T>
inline constexpr T clamp(T val, T minVal, T maxVal) {
	return (val > maxVal) ? maxVal : ((val < minVal) ? minVal : val);
}

// std::round (not present on older MacOSX SDKs)
template<class T>
T round(T arg) {
  return ::round(arg);
}

// std::lround (not present on older MacOSX SDKs)
template<class T>
long lround(T arg) {
  return ::lround(arg);
}

// std::make_unique
template<class T> struct _Unique_if {
  typedef std::unique_ptr<T> _Single_object;
};
template<class T> struct _Unique_if<T[]> {
  typedef std::unique_ptr<T[]> _Unknown_bound;
};
template<class T, size_t N> struct _Unique_if<T[N]> {
  typedef void _Known_bound;
};
template<class T, class... Args>
  typename _Unique_if<T>::_Single_object
  make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }
template<class T>
  typename _Unique_if<T>::_Unknown_bound
  make_unique(size_t n) {
    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[n]());
  }
template<class T, class... Args>
  typename _Unique_if<T>::_Known_bound
  make_unique(Args&&...) = delete;

// std::size
template <typename T, size_t N>
constexpr size_t size(const T (&)[N]) noexcept {
  return N;
}

}

#endif

#endif
