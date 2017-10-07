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

#include "yyy_irc_numerics.h"

/*---------------------------------------------------------------------------*/

const char *YYY_IRCGetNumericCodeName(enum YYY_NumericCode code){

#define IRC_NUM_STR(CODE, STR) case YYY_IRC ## CODE ## Num: return "RPL_" #STR

#define IRC_NUM_LIST_NO_START_STR(CODE, STR)\
    case YYY_IRC ## CODE ## EntryNum: return "RPL_" #STR;\
    case YYY_IRCEndOf ## CODE ## Num: return "RPL_" #STR "END"

#define IRC_NUM_LIST_STR(CODE, STR)\
    case YYY_IRCStart ## CODE ## Num: return "RPL_" #STR "START";\
    IRC_NUM_LIST_NO_START_STR(CODE, STR)

#define IRC_NUM_REPLY(CODE, STR)\
    case YYY_IRC ## CODE ## EntryNum: return "RPL_" #STR "REPLY";\
    case YYY_IRCEndOf ## CODE ## Num: return "RPL_ENDOF" #STR;\

    switch(code){
        IRC_NUM_STR(Welcome, WELCOME);
        IRC_NUM_STR(YourHost, YOURHOST);
        IRC_NUM_STR(Created, CREATED);
        IRC_NUM_STR(MyInfo, MYINFO);
        IRC_NUM_STR(Bounce, BOUNCE);
        IRC_NUM_STR(UModeIs, UMODEIS);
        IRC_NUM_LIST_NO_START_STR(Services, SERVLIST);
        IRC_NUM_STR(ClientNumber, LUSERCLIENT);
        IRC_NUM_STR(OpsNumber, LUSEROP);
        IRC_NUM_STR(UnknownNumber, LUSERUNKNOWN);
        IRC_NUM_STR(ChannelNumber, LUSERCHANNEL);
        IRC_NUM_STR(EndpointNumber, LUSERME);
        IRC_NUM_STR(AdminInfo, ADMINME);
        IRC_NUM_STR(AdminLoc0Info, ADMINLOC0);
        IRC_NUM_STR(AdminLoc1Info, ADMINLOC1);
        IRC_NUM_STR(AdminEmailInfo, ADMINEMAIL);
        IRC_NUM_STR(TryAgain, TRYAGAIN);
        IRC_NUM_STR(Away, AWAY);
        IRC_NUM_STR(UnAway, UNAWAY);
        IRC_NUM_STR(NoAway, NOAWAY);
        IRC_NUM_STR(UserHost, USERHOST);
        IRC_NUM_STR(IsOn, ISON);
        IRC_NUM_STR(WhoIsUser, WHOISUSER);
        IRC_NUM_STR(WhoWasUser, WHOWASUSER);
        IRC_NUM_STR(WhoIsIdle, WHOISIDLE);
        IRC_NUM_STR(WhoIsServer, WHOISSERVER);
        IRC_NUM_STR(WhoIsChannel, WHOISCHANNELS);
        IRC_NUM_STR(WhoIsOper, WHOISOPER);
        IRC_NUM_STR(EndOfWhoIs, ENDOFWHOIS);
        IRC_NUM_STR(EndOfWhoWas, ENDOFWHOWAS);
        IRC_NUM_LIST_STR(List, LIST);
        IRC_NUM_STR(ChannelModeIs, CHANNELMODEIS);
        IRC_NUM_STR(UniqueOpIs, UNIQOPIS);
        IRC_NUM_STR(Topic, TOPIC);
        IRC_NUM_STR(NoTopic, NOTOPIC);
        IRC_NUM_STR(InviteSent, INVITING);
        IRC_NUM_STR(SummonSent, SUMMONING);
        IRC_NUM_STR(Version, VERSION);
        IRC_NUM_REPLY(Who, WHO);
        IRC_NUM_REPLY(Names, NAMES);
        IRC_NUM_REPLY(Links, LINKS);
        IRC_NUM_LIST_STR(MOTD, MOTD);
        IRC_NUM_REPLY(Ban, BANLIST);
        IRC_NUM_REPLY(Info, INFO);
        IRC_NUM_STR(OpGranted, YOUREOPER);
        IRC_NUM_STR(ServiceGranted, YOURESERVICE);
        IRC_NUM_STR(RehashSent, REHASHING);
        IRC_NUM_LIST_STR(Users, USERS);
        IRC_NUM_STR(NoUsers, NOUSERS);
        IRC_NUM_STR(Time, TIME);
        IRC_NUM_STR(NoSuchName, NOSUCHNICK);
    }
    
    return "<UNKNOWN>";
}
