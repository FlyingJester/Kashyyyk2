#pragma once

#if defined(__GNUC__) || defined (__clang__)
/* GCC, Clang, or possibly the Intel compiler on Unix */

#define YYY_NETWORK_NOT_NULL(X) X __attribute__((returns_nonnull))
#define YYY_NETWORK_PURE(X) X __attribute__((pure))
#define YYY_NETWORK_CONST(X) X __attribute__((const))
#define YYY_NETWORK_WARN_UNUSED_RESULT(X) X __attribute__((warn_unused_result))
#define YYY_NETWORK_UNLIKELY_FUNCTION(X) X __attribute__((cold))
#define YYY_NETWORK_LIKELY_FUNCTION(X) X __attribute__((hot))
#define YYY_NETWORK_NEW_RETURN(X) X __attribute__((malloc))

#define YYY_NETWORK_LIKELY(X)   (__builtin_expect(!!(X), 1))
#define YYY_NETWORK_UNLIKELY(X) (__builtin_expect(!!(X), 0))

#define YYY_NETWORK_RESTRICT __restrict__

#define YYY_NETWORK_NOINLINE(X) X __attribute__((noinline))

/* End __GNUC__ || __clang__ */

#elif defined _MSC_VER
/* Microsoft Visual C */
#define YYY_NETWORK_NEW_RETURN(X) __declspec(restrict) X

#define YYY_NETWORK_RESTRICT __restrict

#define YYY_NETWORK_NOINLINE(X) __declspec(noinline) X
/* End MSC_VER_ */

#elif defined(__WATCOMC__)
/* OpenWatcom C/C++ Compiler */

#ifndef __cplusplus
#define YYY_NETWORK_RESTRICT __restrict
#endif

/* End __WATCOMC__*/
#endif

#ifndef YYY_NETWORK_NOT_NULL
#define YYY_NETWORK_NOT_NULL(X) X
#endif

#ifndef YYY_NETWORK_PURE
#define YYY_NETWORK_PURE(X) X
#endif

#ifndef YYY_NETWORK_CONST
#define YYY_NETWORK_CONST(X) X
#endif

#ifndef YYY_NETWORK_WARN_UNUSED_RESULT
#define YYY_NETWORK_WARN_UNUSED_RESULT(X) X
#endif

#ifndef YYY_NETWORK_UNLIKELY_FUNCTION
#define YYY_NETWORK_UNLIKELY_FUNCTION(X) X
#endif

#ifndef YYY_NETWORK_LIKELY_FUNCTION
#define YYY_NETWORK_LIKELY_FUNCTION(X) X
#endif

#ifndef YYY_NETWORK_NEW_RETURN
#define YYY_NETWORK_NEW_RETURN(X) X
#endif

#ifndef YYY_NETWORK_LIKELY
#define YYY_NETWORK_LIKELY(X)   (!!(X))
#endif

#ifndef YYY_NETWORK_UNLIKELY
#define YYY_NETWORK_UNLIKELY(X) (!!(X))
#endif

#ifndef YYY_NETWORK_RESTRICT
#define YYY_NETWORK_RESTRICT
#endif

#ifndef YYY_NETWORK_NOINLINE
#define YYY_NETWORK_NOINLINE(X) X
#endif
