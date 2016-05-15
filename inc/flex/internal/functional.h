#ifndef FLEX_INTERNAL_FUNCTIONAL_H
#define FLEX_INTERNAL_FUNCTIONAL_H

#include <functional>

#ifndef FLEX_HAS_CXX11

namespace std
{

  template<typename T> struct hash;

  template<typename T> struct hash<T*> // Note that we use the pointer as-is and don't divide by sizeof(T*). This is because the table is of a prime size and this division doesn't benefit distribution.
  {
    size_t operator()(T* p) const
    {
      return size_t(uintptr_t(p));
    }
  };

  template<> struct hash<bool>
  {
    size_t operator()(bool val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<char>
  {
    size_t operator()(char val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<signed char>
  {
    size_t operator()(signed char val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<unsigned char>
  {
    size_t operator()(unsigned char val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<signed short>
  {
    size_t operator()(signed short val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<unsigned short>
  {
    size_t operator()(unsigned short val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<signed int>
  {
    size_t operator()(signed int val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<unsigned int>
  {
    size_t operator()(unsigned int val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<signed long>
  {
    size_t operator()(signed long val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<unsigned long>
  {
    size_t operator()(unsigned long val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<signed long long>
  {
    size_t operator()(signed long long val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<unsigned long long>
  {
    size_t operator()(unsigned long long val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<float>
  {
    size_t operator()(float val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<double>
  {
    size_t operator()(double val) const
    {
      return static_cast<size_t>(val);
    }
  };

  template<> struct hash<long double>
  {
    size_t operator()(long double val) const
    {
      return static_cast<size_t>(val);
    }
  };

///////////////////////////////////////////////////////////////////////////
// string hashes
//
// Note that our string hashes here intentionally are slow for long strings.
// The reasoning for this is so:
//    - The large majority of hashed strings are only a few bytes long.
//    - The hash function is significantly more efficient if it can make this assumption.
//    - The user is welcome to make a custom hash for those uncommon cases where
//      long strings need to be hashed. Indeed, the user can probably make a
//      special hash customized for such strings that's better than what we provide.
///////////////////////////////////////////////////////////////////////////

  template<> struct hash<char8_t*>
  {
    size_t operator()(const char8_t* p) const
    {
      uint32_t c, result = 2166136261U; // FNV1 hash. Perhaps the best string hash. Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
      while ((c = (uint8_t) *p++) != 0)     // Using '!=' disables compiler warnings.
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

  template<> struct hash<const char8_t*>
  {
    size_t operator()(const char8_t* p) const
    {
      uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
      while ((c = (uint8_t) *p++) != 0)     // cast to unsigned 8 bit.
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

  template<> struct hash<char16_t*>
  {
    size_t operator()(const char16_t* p) const
    {
      uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
      while ((c = (uint16_t) *p++) != 0)    // cast to unsigned 16 bit.
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

  template<> struct hash<const char16_t*>
  {
    size_t operator()(const char16_t* p) const
    {
      uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
      while ((c = (uint16_t) *p++) != 0)    // cast to unsigned 16 bit.
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

  template<> struct hash<char32_t*>
  {
    size_t operator()(const char32_t* p) const
    {
      uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
      while ((c = (uint32_t) *p++) != 0)    // cast to unsigned 32 bit.
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

  template<> struct hash<const char32_t*>
  {
    size_t operator()(const char32_t* p) const
    {
      uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
      while ((c = (uint32_t) *p++) != 0)    // cast to unsigned 32 bit.
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

} // namespace std

#endif //FLEX_HAS_CXX11

#endif // FLEX_INTERNAL_FUNCTIONAL_H

