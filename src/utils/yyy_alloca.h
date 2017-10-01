#pragma once

#ifndef YYY_ALLOCA

#ifdef __GNUC__

#include <alloca.h>
#define YYY_HAS_ALLOCA 1

#elif (defined __WATCOMC__) || (defined _MSC_VER)

#include <malloc.h>
#define YYY_HAS_ALLOCA 1

#else
// Do someting here if alloca isn't defined!
#endif

#ifdef YYY_HAS_ALLOCA
#define YYY_ALLOCA alloca
#define YYY_ALLOCA_FREE(X) ((void)(X))
#else
#define YYY_ALLOCA malloc
#define YYY_ALLOCA_FREE free
#endif

#endif
