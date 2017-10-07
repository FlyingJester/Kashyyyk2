/* 
 *  Copyright (c) 2014-2017 Martin McDonough.  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * - Products derived from this software may not be called "Kashyyyk", nor may
 *     "YYY" appear in their name, without prior written permission of
 *     the copyright holders.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/*---------------------------------------------------------------------------*/

#ifndef KASHYYYK_YYY_DATE_H
#define KASHYYYK_YYY_DATE_H
#pragma once
    
/*---------------------------------------------------------------------------*/

#include "yyy_attributes.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    
/*---------------------------------------------------------------------------*/

/* We use a unique date type. We want to use this instead of a timeval because
 * we do not need the microsecond precision, and we also want this structure to
 * be as small as possible so a full string or UTC time would not be ideal.
 * It is possible to compare dates simply by comparing the date member, there
 * is only one possible representation of each possible date.
 */
struct YYY_Date {
    uint32_t date;
};

/**
 * @defgroup YYY_Date Getters
 * @{
 */
/*---------------------------------------------------------------------------*/

unsigned YYY_PURE_INLINE(YYY_DateGetYear(const struct YYY_Date date));

/*---------------------------------------------------------------------------*/

unsigned YYY_PURE_INLINE(YYY_DateGetMonth(const struct YYY_Date date));

/*---------------------------------------------------------------------------*/

unsigned YYY_PURE_INLINE(YYY_DateGetDay(const struct YYY_Date date));

/*---------------------------------------------------------------------------*/

unsigned YYY_PURE_INLINE(YYY_DateGetHour(const struct YYY_Date date));

/*---------------------------------------------------------------------------*/

unsigned YYY_PURE_INLINE(YYY_DateGetMinute(const struct YYY_Date date));

/**
 * @}
 */

/**
 * @defgroup YYY_Date Setters
 * @{
 */
/*---------------------------------------------------------------------------*/

void YYY_PURE_INLINE(YYY_DateSetYear(struct YYY_Date *date, unsigned y));

/*---------------------------------------------------------------------------*/

void YYY_PURE_INLINE(YYY_DateSetMonth(struct YYY_Date *date, unsigned m));

/*---------------------------------------------------------------------------*/

void YYY_PURE_INLINE(YYY_DateSetDay(struct YYY_Date *date, unsigned d));

/*---------------------------------------------------------------------------*/

void YYY_PURE_INLINE(YYY_DateSetHour(struct YYY_Date *date, unsigned h));

/*---------------------------------------------------------------------------*/

void YYY_PURE_INLINE(YYY_DateSetMinute(struct YYY_Date *date, unsigned m));

/*---------------------------------------------------------------------------*/

void YYY_DateSetNow(struct YYY_Date *date);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* KASHYYYK_YYY_DATE_H */
