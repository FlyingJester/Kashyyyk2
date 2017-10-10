 /* 
 *  Copyright (c) 2017 Martin McDonough.  All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGYYYENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/*---------------------------------------------------------------------------*/

#ifndef YYY_IRC_NUMERICS_H
#define YYY_IRC_NUMERICS_H
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

/* Enum of the RPL_* codes in FRC 2812 */
enum YYY_NumericCode {
    YYY_IRCWelcomeNum = 1,
    YYY_IRCYourHostNum,
    YYY_IRCCreatedNum,
    YYY_IRCMyInfoNum,
    YYY_IRCBounceNum, /* 5 */
    
    /* Information */
    YYY_IRCUModeIsNum = 221,
    YYY_IRCServicesEntryNum = 234, /* Response to a SERVLIST message */
    YYY_IRCEndOfServicesNum,
    YYY_IRCClientNumberNum = 251, /* Info on the number of clients */
    YYY_IRCOpsNumberNum, /* Info on the number of ops */
    YYY_IRCUnknownNumberNum, /* Info on the number of unknown clients */
    YYY_IRCChannelNumberNum, /* Info on the number of channels */
    YYY_IRCEndpointNumberNum, /* 255 */ /*Info on the number of servers */
    YYY_IRCAdminInfoNum, /* Response to an ADMIN message (beginning) */
    YYY_IRCAdminLoc0InfoNum, /* Response to an ADMIN message (location 0) */
    YYY_IRCAdminLoc1InfoNum, /* Response to an ADMIN message (location 1) */
    YYY_IRCAdminEmailInfoNum, /* Response to an ADMIN message (email) */
    YYY_IRCTryAgainNum = 263, /* A command was dropped and must be resent. */
    
    /* Actions and queries */
    YYY_IRCAwayNum = 301,
    YYY_IRCUserHostNum,
    YYY_IRCIsOnNum,
    YYY_IRCUnAwayNum = 305,
    YYY_IRCNoAwayNum,
    YYY_IRCWhoIsUserNum = 311,
    YYY_IRCWhoIsServerNum,
    YYY_IRCWhoIsOperNum,
    YYY_IRCWhoWasUserNum,
    YYY_IRCEndOfWhoNum, /* 315 */
    YYY_IRCWhoIsIdleNum = 317,
    YYY_IRCEndOfWhoIsNum,
    YYY_IRCWhoIsChannelNum,
    YYY_IRCStartListNum = 321, /* Deprecated */
    YYY_IRCListEntryNum,
    YYY_IRCEndOfListNum,
    YYY_IRCChannelModeIsNum,
    YYY_IRCUniqueOpIsNum, /* 325 */
    YYY_IRCNoTopicNum = 331,
    YYY_IRCTopicNum,
    YYY_IRCInviteSentNum = 341,
    YYY_IRCSummonSentNum,
    YYY_IRCInviteListEntryNum = 364,
    YYY_IRCEndOfInviteListNum,
    YYY_IRCExceptListEntryNum,
    YYY_IRCEndOfExceptListNum,
    YYY_IRCVersionNum = 351, /* <major>.<minor> <server> :<comments> */
    YYY_IRCWhoEntryNum, /* Response entry to a WHO message. */
    YYY_IRCNamesEntryNum, /* Response entry to a NAMES message. */
    YYY_IRCLinksEntryNum = 364, /* Response entry to a LINKS message. */
    YYY_IRCEndOfLinksNum,
    YYY_IRCEndOfNamesNum,
    YYY_IRCBanEntryNum, /* Used when listing the bans of a channel */
    YYY_IRCEndOfBanNum,
    YYY_IRCEndOfWhoWasNum, /* 369 */
    YYY_IRCInfoEntryNum = 371, /* Response entry to an INFO message */
    YYY_IRCMOTDEntryNum, /* Begins with :- */
    YYY_IRCEndOfInfoNum = 374,
    YYY_IRCStartMOTDNum, /* Begins with :- */
    YYY_IRCEndOfMOTDNum,
    YYY_IRCOpGrantedNum = 381,
    YYY_IRCRehashSentNum,
    YYY_IRCServiceGrantedNum,
    YYY_IRCTimeNum = 391, /* Response to a TIME message. <server> :<time> */
    YYY_IRCStartUsersNum,
    YYY_IRCUsersEntryNum, /* Response to a USERS message */
    YYY_IRCEndOfUsersNum,
    YYY_IRCNoUsersNum, /* Response to a USERS message is empty */
    
    /* Errors */
    YYY_IRCNoSuchNameNum = 401, /* No such nick or channel */
    
};

/*---------------------------------------------------------------------------*/

#define YYY_STRING_TO_NUMERIC_CODE(STR) ((enum YYY_NumericCode)\
        (((STR)[0] - '0') * 100) +\
        (((STR)[1] - '0') * 10) +\
        ((STR)[2] - '0'))

/*---------------------------------------------------------------------------*/

#define YYY_STRING_IS_NUMERIC(STR)\
    ((STR)[0] >= '0' && (STR)[0] <= '9' &&\
    (STR)[1] >= '0' && (STR)[1] <= '9' &&\
    (STR)[2] >= '0' && (STR)[2] <= '9')

/*---------------------------------------------------------------------------*/

const char *
#ifdef __GNUC__
__attribute__((const))
#endif
YYY_IRCGetNumericCodeName(enum YYY_NumericCode code);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* YYY_IRC_NUMERICS_H */
