#pragma once

#ifdef __cpusplus
#if (defined _MSC_VER) && (_MSC_VER >= 1700)
#define YYY_OVERRIDE(X) X override
#elif (defined __GNUC__) || (defined __clang__)
#define YYY_OVERRIDE(X) X override
#else
#define YYY_OVERRIDE(X) virtual X
#endif
#endif /* c++ */

#if defined(__GNUC__)
/* Only GCC */
#define YYY_INLINE_ALL_BODY_CALLS(X) X __attribute__((flatten))
#endif

#if defined(__GNUC__) || defined (__clang__)
/* GCC, Clang, or possibly the Intel compiler on Unix */

#define YYY_INLINE(X) X
#define YYY_FORCE_INLINE(X) X __attribute__((__always_inline__))
#define YYY_NOT_NULL(X) X __attribute__((returns_nonnull))
#define YYY_PURE(X) X __attribute__((pure))
#define YYY_CONST(X) X __attribute__((const))
#define YYY_PURE_INLINE(X) X __attribute__((__always_inline__,const))
#define YYY_WARN_UNUSED_RESULT(X) X __attribute__((warn_unused_result))
#define YYY_UNLIKELY_FUNCTION(X) X __attribute__((cold))
#define YYY_LIKELY_FUNCTION(X) X __attribute__((hot))
#define YYY_NEW_RETURN(X) X __attribute__((malloc))

#define YYY_LIKELY(X)   (__builtin_expect(!!(X), 1))
#define YYY_UNLIKELY(X) (__builtin_expect(!!(X), 0))

#define YYY_RESTRICT __restrict__

#define YYY_NOINLINE(X) X __attribute__((noinline))

/* End __GNUC__ || __clang__ */

#elif defined _MSC_VER
/* Microsoft Visual C */
#define YYY_INLINE(X) __inline X
#define YYY_FORCE_INLINE(X) __forceinline X

#define YYY_NEW_RETURN(X) __declspec(restrict) X

#define YYY_RESTRICT __restrict

#define YYY_NOINLINE(X) __declspec(noinline) X
/* End MSC_VER_ */

#elif defined(__WATCOMC__)
/* OpenWatcom C/C++ Compiler */

#define YYY_INLINE(X) __inline X

#define YYY_RESTRICT __restrict

/* End __WATCOMC__*/
#endif

#ifndef YYY_PURE_INLINE
#if defined YYY_PURE
#define YYY_PURE_INLINE YYY_PURE
#elif defined YYY_FORCE_INLINE
#define YYY_PURE_INLINE YYY_FORCE_INLINE
#elif defined YYY_INLINE
#define YYY_PURE_INLINE YYY_INLINE
#else
#define YYY_PURE_INLINE(X) X
#endif
#endif

#ifndef YYY_INLINE_ALL_BODY_CALLS
#define YYY_INLINE_ALL_BODY_CALLS(X) X
#endif

#ifndef YYY_INLINE
#define YYY_INLINE(X) X
#endif

#ifndef YYY_FORCE_INLINE
#define YYY_FORCE_INLINE YYY_INLINE
#endif

#ifndef YYY_NOT_NULL
#define YYY_NOT_NULL(X) X
#endif

#ifndef YYY_PURE
#define YYY_PURE(X) X
#endif

#ifndef YYY_CONST
#define YYY_CONST(X) X
#endif

#ifndef YYY_UNLIKELY_FUNCTION
#define YYY_UNLIKELY_FUNCTION(X) X
#endif

#ifndef YYY_LIKELY_FUNCTION
#define YYY_LIKELY_FUNCTION(X) X
#endif

#ifndef YYY_NEW_RETURN
#define YYY_NEW_RETURN(X) X
#endif

#ifndef YYY_LIKELY
#define YYY_LIKELY(X)   (!!(X))
#endif

#ifndef YYY_UNLIKELY
#define YYY_UNLIKELY(X) (!!(X))
#endif

#ifndef YYY_RESTRICT
#define YYY_RESTRICT
#endif

#ifndef YYY_NOINLINE
#define YYY_NOINLINE(X) X
#endif
