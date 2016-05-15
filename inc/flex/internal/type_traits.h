#ifndef FLEX_INTERNAL_TYPE_TRAITS_H
#define FLEX_INTERNAL_TYPE_TRAITS_H

#ifdef FLEX_HAS_CXX11

#include <type_traits>

#else

namespace std
{

  ///////////////////////////////////////////////////////////////////////
  // integral_constant
  //
  // This is the base class for various type traits, as defined by C++11.
  // This is essentially a utility base class for defining properties
  // as both class constants (value) and as types (type).
  //
  template<typename T, T v>
  struct integral_constant
  {
    static const T value = v;
    typedef T value_type;
    typedef integral_constant<T, v> type;
  };

  ///////////////////////////////////////////////////////////////////////
  // true_type / false_type
  //
  // These are commonly used types in the implementation of type_traits.
  // Other integral constant types can be defined, such as those based on int.
  //
  typedef integral_constant<bool, true> true_type;
  typedef integral_constant<bool, false> false_type;

  ///////////////////////////////////////////////////////////////////////
  // conditional
  //
  // This is used to declare a type from one of two type options.
  // The result is based on the condition type. This has certain uses
  // in template metaprogramming.
  //
  // Example usage:
  //    typedef ChosenType = conditional<is_integral<SomeType>::value, ChoiceAType, ChoiceBType>::type;
  //
  template<bool bCondition, class ConditionIsTrueType, class ConditionIsFalseType>
  struct conditional
  {
    typedef ConditionIsTrueType type;
  };

  template<typename ConditionIsTrueType, class ConditionIsFalseType>
  struct conditional<false, ConditionIsTrueType, ConditionIsFalseType>
  {
    typedef ConditionIsFalseType type;
  };

}  //namespace std

#endif //FLEX_HAS_CXX11

#endif //FLEX_INTERNAL_TYPE_TRAITS_H
