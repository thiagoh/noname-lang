#ifndef _NONAME_UTILS_H
#define _NONAME_UTILS_H

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <type_traits>

namespace noname {

template <typename To, typename From>
struct is_of_type_impl {
  static inline bool doit(const From& from) {
    /**
      * Inspiration sources
      * http://en.cppreference.com/w/cpp/types/is_base_of
      * https://repl.it/H4pc/1
      */
    return from.getType() == To::getClassType() || std::is_base_of<From, To>::value;
  }
};

template <class To, class From>
inline bool is_of_type(const From& from) {
  return is_of_type_impl<To, From>::doit(from);
}

template <typename To, typename From>
struct cast_impl {
  static inline To doit(const From& from) { return (To)from; }
};

template <class To, class From>
inline To cast(const From& from) {
  return cast_impl<To, From>::doit(from);
}
}

#endif