#ifndef ATTRIBUTES_H__
#define ATTRIBUTES_H__

/**
 * C++ language standard detection
 */
#if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1) // fix for issue #464
  #define TZ_HAS_CPP_17
  #define TZ_HAS_CPP_14
#elif (defined(__cplusplus) && __cplusplus >= 201402L) || (defined(_HAS_CXX14) && _HAS_CXX14 == 1)
  #define TZ_HAS_CPP_14
#endif

/**
 * __has_attribute
 */
#ifdef __has_attribute
  #define TZ_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
  #define TZ_HAS_ATTRIBUTE(x) 0
#endif

/**
 * __has_cpp_attribute
 */
#if defined(__cplusplus) && defined(__has_cpp_attribute)
  #define TZ_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
  #define TZ_HAS_CPP_ATTRIBUTE(x) 0
#endif

#if defined(__has_include) && !defined(__INTELLISENSE__) && \
  !(defined(__INTEL_COMPILER) && __INTEL_COMPILER < 1600)
  #define TZ_HAS_INCLUDE(x) __has_include(x)
#else
  #define TZ_HAS_INCLUDE(x) 0
#endif

/**
 * Always Inline
 */
#if defined(__GNUC__)
  #define TZ_ALWAYS_INLINE inline __attribute__((__always_inline__))
#elif defined(_WIN32)
  #define TZ_ALWAYS_INLINE __forceinline
#else
  #define TZ_ALWAYS_INLINE inline
#endif

/**
 * Portable no discard warnings
 */
#if TZ_HAS_CPP_ATTRIBUTE(nodiscard)
  #if defined(__clang__) && !defined(TZ_HAS_CPP_17)
    #define TZ_NODISCARD
  #else
    #define TZ_NODISCARD [[nodiscard]]
  #endif
#elif TZ_HAS_CPP_ATTRIBUTE(gnu::warn_unused_result)
  #define TZ_NODISCARD [[gnu::warn_unused_result]]
#else
  #define TZ_NODISCARD
#endif

/**
 * Portable maybe_unused
 */
#if TZ_HAS_CPP_ATTRIBUTE(maybe_unused) && (defined(_HAS_CXX17) && _HAS_CXX17 == 1)
  #define TZ_MAYBE_UNUSED [[maybe_unused]]
#elif TZ_HAS_ATTRIBUTE(__unused__) || defined(__GNUC__)
  #define TZ_MAYBE_UNUSED __attribute__((__unused__))
#else
  #define TZ_MAYBE_UNUSED
#endif

/**
 * Gcc hot/cold attributes
 * Tells GCC that a function is hot or cold. GCC can use this information to
 * improve static analysis, i.e. a conditional branch to a cold function
 * is likely to be not-taken.
 */
#if TZ_HAS_ATTRIBUTE(hot) || (defined(__GNUC__) && !defined(__clang__))
  #define TZ_ATTRIBUTE_HOT __attribute__((hot))
#else
  #define TZ_ATTRIBUTE_HOT
#endif

#if TZ_HAS_ATTRIBUTE(cold) || (defined(__GNUC__) && !defined(__clang__))
  #define TZ_ATTRIBUTE_COLD __attribute__((cold))
#else
  #define TZ_ATTRIBUTE_COLD
#endif

/***/
#define TZ_NODISCARD_ALWAYS_INLINE_HOT TZ_NODISCARD TZ_ALWAYS_INLINE TZ_ATTRIBUTE_HOT

/***/
#define TZ_ALWAYS_INLINE_HOT TZ_ALWAYS_INLINE TZ_ATTRIBUTE_HOT

#endif /* ATTRIBUTES_H__ */