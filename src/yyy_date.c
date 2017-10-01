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

#include "yyy_date.h"

/* What|Size|Strt| End
 Year  | 7  | 20 | 27
 Month | 4  | 16 | 20
 Day   | 5  | 11 | 16
 Hour  | 5  | 6  | 11
 Minute| 6  | 0  | 6
 */

unsigned YYY_GetYear(const struct YYY_Date date){
    return (date.date >> 20) & 0x7F;
}

/*---------------------------------------------------------------------------*/

unsigned YYY_GetMonth(const struct YYY_Date date){
    return (date.date >> 16) & 0x0F;
}

/*---------------------------------------------------------------------------*/

unsigned YYY_GetDay(const struct YYY_Date date){
    return (date.date >> 11) & 0x1F;
}

/*---------------------------------------------------------------------------*/

unsigned YYY_GetHour(const struct YYY_Date date){
    return (date.date >> 6) & 0x1F;
}

/*---------------------------------------------------------------------------*/

unsigned YYY_GetMinute(const struct YYY_Date date){
    return date.date & 0x3F;
}

/*---------------------------------------------------------------------------*/

void YYY_SetYear(struct YYY_Date *date, unsigned y){
    date->date &= 0x000FFFFF;
    date->date |= (y & 0xFF) << 20;
}

/*---------------------------------------------------------------------------*/

void YYY_SetMonth(struct YYY_Date *date, unsigned m){
    date->date &= 0xFFF0FFFF;
    date->date |= (m & 0x0F) << 16;
}

/*---------------------------------------------------------------------------*/

void YYY_SetDay(struct YYY_Date *date, unsigned d){
    date->date &= 0xFFFF07FF;
    date->date |= (d & 0x1F) << 11;
}

/*---------------------------------------------------------------------------*/

void YYY_SetHour(struct YYY_Date *date, unsigned h){
    date->date &= 0xFFFFF83F;
    date->date |= (h & 0x1F) << 6;
}

/*---------------------------------------------------------------------------*/

void YYY_SetMinute(struct YYY_Date *date, unsigned m){
    date->date &= 0xFFFFFFC0;
    date->date |= m & 0x3F;
}
