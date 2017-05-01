#ifndef _NONAME_UTILS_H
#define _NONAME_UTILS_H

#include <cctype>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <typeinfo>
#include <type_traits>

namespace noname {

// template <typename Base, typename Derived>
// struct is_of_type_impl {
//   static inline bool doit(const Derived& derived) {
//     /**
//       * Inspiration sources
//       * http://en.cppreference.com/w/cpp/types/is_base_of
//       * https://repl.it/H4pc/1
//       *
//       * this will not work std::is_base_of<Base, Derived>::value
//       * because the Base and Derived types must be complete at this point
//       *
//       * If both Base and Derived are non-union class types, and they are not the same type
//       * (ignoring cv-qualification), Derived shall be a complete type; otherwise the behavior is undefined.
//       * EXPLANATION: http://en.cppreference.com/w/cpp/language/type#Incomplete_type
//       */
//     return derived.getType() == Base::getClassType() || std::is_base_of<Base, Derived>::value;
//   }
// };
// template <class Base, class Derived>
// inline bool is_of_type(const Derived& derived) {
//   return is_of_type_impl<Base, Derived>::doit(derived);
// }

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