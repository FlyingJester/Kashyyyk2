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

#include "yyy_test.h"
#include "yyy_irc_core.h"

#include <string.h>
#include <assert.h>

/*---------------------------------------------------------------------------*/

static int yyy_stringify_round_trip(const struct YYY_Message *msg){
    size_t len;
    struct YYY_Message result;
    const char *text = YYY_IRCMessageToString(msg, &len);
    
    int SUCCESS_INDICATOR = 1;

    YYY_IRCParseMessage(text, len, &result);
    
    YYY_EXPECT_INT_EQ(result.type, msg->type);

    if(SUCCESS_INDICATOR == 0)
        goto yyy_test_ending;
    
    YYY_EXPECT_INT_EQ(result.m.any_from.from_len, msg->m.any_from.from_len);

    if(SUCCESS_INDICATOR == 0)
        goto yyy_test_ending;

    YYY_EXPECT_YYY_STR_EQ_N(result.m.any_from.from,
        msg->m.any_from.from,
        result.m.any_from.from_len);
    
    switch(result.type){
        case eYYYChatNotification:
             YYY_EXPECT_INT_EQ(result.m.notification.message_len, msg->m.notification.message_len);
             if(SUCCESS_INDICATOR == 0)
                 break;
             YYY_EXPECT_YYY_STR_EQ_N(result.m.message.message,
                 msg->m.message.message,
                 result.m.message.message_len);
             break;
        case eYYYChatMessage:
             YYY_EXPECT_INT_EQ(result.m.message.message_len, msg->m.message.message_len);
             if(SUCCESS_INDICATOR == 0)
                 break;
             YYY_EXPECT_YYY_STR_EQ_N(result.m.message.message,
                msg->m.message.message,
                 result.m.message.message_len);
             break;
        case eYYYChatNick:
             YYY_EXPECT_INT_EQ(result.m.nick.nick_len, msg->m.nick.nick_len);
             if(SUCCESS_INDICATOR == 0)
                 break;
             YYY_EXPECT_YYY_STR_EQ_N(result.m.nick.nick,
                msg->m.nick.nick,
                 result.m.nick.nick_len);
             break;
        case eYYYChatIdentify:
             YYY_EXPECT_INT_EQ(result.m.identify.user_len, msg->m.identify.user_len);
             YYY_EXPECT_INT_EQ(result.m.identify.real_len, msg->m.identify.real_len);
             if(SUCCESS_INDICATOR == 0)
                 break;
             YYY_EXPECT_YYY_STR_EQ_N(result.m.identify.user,
                msg->m.identify.user,
                 result.m.identify.user_len);
             YYY_EXPECT_YYY_STR_EQ_N(result.m.identify.real,
                msg->m.identify.real,
                 result.m.identify.real_len);
             break;
        case eYYYChatPing:
             YYY_EXPECT_INT_EQ(result.m.ping.message_len, msg->m.ping.message_len);
             if(SUCCESS_INDICATOR == 0)
                 break;
             YYY_EXPECT_YYY_STR_EQ_N(result.m.ping.message,
                msg->m.ping.message,
                 result.m.ping.message_len);
             break;
        case eYYYChatPong:
             YYY_EXPECT_INT_EQ(result.m.pong.message_len, msg->m.pong.message_len);
             if(SUCCESS_INDICATOR == 0)
                 break;
             YYY_EXPECT_YYY_STR_EQ_N(result.m.pong.message,
                msg->m.pong.message,
                 result.m.pong.message_len);
             break;
        case eYYYChatPart:
        case eYYYChatJoin:
        case eYYYChatQuit:
             YYY_EXPECT_INT_EQ(result.m.quit.message_len, msg->m.quit.message_len);
             /* Quit may have a zero-length message */
             if(result.m.quit.message_len == 0 || SUCCESS_INDICATOR == 0)
                 break;
             YYY_EXPECT_YYY_STR_EQ_N(result.m.quit.message,
                 msg->m.quit.message,
                 result.m.quit.message_len);
             break;
        case eYYYChatBan:
            YYY_EXPECT_INT_EQ(result.m.ban.message_len, msg->m.ban.message_len);
            YYY_EXPECT_INT_EQ(result.m.ban.where_len, msg->m.ban.where_len);
            YYY_EXPECT_INT_EQ(result.m.ban.who_len, msg->m.ban.who_len);
            if(SUCCESS_INDICATOR == 0)
                break;
            YYY_EXPECT_YYY_STR_EQ_N(result.m.ban.message,
                msg->m.ban.message,
                result.m.ban.message_len);
            YYY_EXPECT_YYY_STR_EQ_N(result.m.ban.where,
                msg->m.ban.where,
                result.m.ban.where_len);
            YYY_EXPECT_YYY_STR_EQ_N(result.m.ban.who,
                msg->m.ban.who,
                result.m.ban.who_len);
            break;
        case eYYYChatKick:
            YYY_EXPECT_INT_EQ(result.m.kick.message_len, msg->m.kick.message_len);
            YYY_EXPECT_INT_EQ(result.m.kick.where_len, msg->m.kick.where_len);
            YYY_EXPECT_INT_EQ(result.m.kick.who_len, msg->m.kick.who_len);
            if(SUCCESS_INDICATOR == 0)
                break;
            YYY_EXPECT_YYY_STR_EQ_N(result.m.kick.message,
                msg->m.kick.message,
                result.m.kick.message_len);
            YYY_EXPECT_YYY_STR_EQ_N(result.m.ban.where,
                msg->m.kick.where,
                result.m.kick.where_len);
            YYY_EXPECT_YYY_STR_EQ_N(result.m.kick.who,
                msg->m.kick.who,
                result.m.kick.who_len);
            break;
    }
    
yyy_test_ending:
    free((void*)text);
    return SUCCESS_INDICATOR;
}

/*---------------------------------------------------------------------------*/

#define YYY_PARSE_TEST(STR)\
    struct YYY_Message YYY_MSG;\
    const char YYY_MSG_SRC[] = STR "\r\n";\
    YYY_IRCParseMessage(YYY_MSG_SRC, sizeof(YYY_MSG_SRC)-1, &YYY_MSG)

/*---------------------------------------------------------------------------*/

static int yyy_parse_nick_test0(void){
    int SUCCESS_INDICATOR = 1;
    #define YYY_NICK_0 "somenickname"
    YYY_PARSE_TEST("NICK " YYY_NICK_0);
    YYY_ASSERT_INT_EQ(YYY_MSG.type, eYYYChatNick);
    YYY_ASSERT_YYY_STR_EQ_LITERAL_N(YYY_MSG.m.nick.nick, YYY_MSG.m.nick.nick_len, YYY_NICK_0);

    return SUCCESS_INDICATOR;
}

/*---------------------------------------------------------------------------*/

static int yyy_parse_nick_test1(void){
    int SUCCESS_INDICATOR = 1;
    #define YYY_NICK_1 "Kilroy"
    #define YYY_NICK_FROM_1 "WiZ!jto@tolsun.oulu.fi"
    YYY_PARSE_TEST(":" YYY_NICK_FROM_1 " NICK " YYY_NICK_1);
    YYY_ASSERT_INT_EQ(YYY_MSG.type, eYYYChatNick);
    YYY_ASSERT_YYY_STR_EQ_LITERAL_N(YYY_MSG.m.nick.nick,
        YYY_MSG.m.nick.nick_len, YYY_NICK_1);
    YYY_ASSERT_YYY_STR_EQ_LITERAL_N(YYY_MSG.m.nick.from,
        YYY_MSG.m.nick.from_len, YYY_NICK_FROM_1);

    return SUCCESS_INDICATOR;
}

/*---------------------------------------------------------------------------*/

int YYY_DummyParseTest(void) { return 1; }
int YYY_DummyStringifyTest(void) { return 1; }

/*---------------------------------------------------------------------------*/

const struct YYY_Test YYY_IRCParseTests[] = {
    YYY_TEST(yyy_parse_nick_test0),
    YYY_TEST(yyy_parse_nick_test1)
};

const struct YYY_Test YYY_IRCStringifyTests[] = {
    YYY_TEST(YYY_DummyStringifyTest)
};

static const size_t YYY_NUM_IRC_PARSE_TESTS =
    sizeof(YYY_IRCParseTests) / sizeof(*YYY_IRCParseTests);
static const size_t YYY_NUM_IRC_STRINGIFY_TESTS =
    sizeof(YYY_IRCStringifyTests) / sizeof(*YYY_IRCStringifyTests);

static char *strdup_to_lower(const char *str, char *buffer){
    const unsigned stringlen = strlen(str);
    unsigned i;
    buffer = (char*)realloc(buffer, stringlen + 1);
    for(i = 0; i < stringlen; i++){
        const char c = str[i];
        if(c >= 'A' && c <= 'Z')
            buffer[i] = (c - 'A') + 'a';
        else
            buffer[i] = c;
    }
    buffer[stringlen] = '\0';
    return buffer;
}

static unsigned run_test_by_name(const char *name,
    const struct YYY_Test *tests,
    unsigned num_tests,
    char **buffer){
    unsigned e;
    for(e = 0; e < num_tests; e++){
        buffer[0] = strdup_to_lower(tests[e].name, *buffer);
        if(strcmp(*buffer, name) == 0){
            unsigned short unused;
            YYY_RUN_TEST(tests[e], unused, "Test");
            return 1;
        }
    }
    return 0;
}

YYY_TEST_FUNCTION(YYY_RunParseTests, YYY_IRCParseTests, "yyy_irc_parse")
YYY_TEST_FUNCTION(YYY_RunStringifyTests, YYY_IRCStringifyTests, "yyy_irc_stringify")

enum YYY_IRCTestSuite{
    eYYY_IRCSuiteNone,
    eYYY_IRCSuiteParse,
    eYYY_IRCSuiteStringify,
    eYYY_IRCSuiteBoth
};

int main(int argc, char **argv){
    enum YYY_IRCTestSuite suite = eYYY_IRCSuiteBoth;
    char *buffer = NULL;
    if(argc > 1){
        const unsigned num_strinfigy =
            sizeof(YYY_IRCStringifyTests) / sizeof(YYY_IRCStringifyTests[0]);
        const unsigned num_parse =
            sizeof(YYY_IRCParseTests) / sizeof(YYY_IRCParseTests[0]);
        unsigned i = 1, num_ran = 0;
        char *arg = NULL;
        while(i < (unsigned)argc){
            const char *name = arg = strdup_to_lower(argv[i++], arg);
            const unsigned ran_stringify =
                run_test_by_name(name, YYY_IRCStringifyTests, num_strinfigy, &buffer);
            const unsigned ran_parse =
                run_test_by_name(name, YYY_IRCParseTests, num_parse, &buffer);
            if(ran_stringify || ran_parse)
                num_ran++;
            else{
                fputs("[Test]ERROR:\tCould not find test ", stdout); puts(name);
            }
        }
        free(arg);
        free(buffer);
        return ((int)num_ran == argc - 1) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if(argc == 1){
        buffer = strdup_to_lower(*argv, buffer);
        if(strstr(buffer, "stringify") != NULL){
            suite = eYYY_IRCSuiteStringify;
        }
        else if(strstr(buffer, "parse") != NULL){
            suite = eYYY_IRCSuiteParse;
        }
    }
    {
        int i = 0;
        assert(suite != eYYY_IRCSuiteNone);
        assert((unsigned)suite &
            ((unsigned)eYYY_IRCSuiteStringify|(unsigned)eYYY_IRCSuiteParse));
        
        if((unsigned)suite & (unsigned)eYYY_IRCSuiteStringify){
            YYY_RUN_TEST_SUITE(YYY_RunStringifyTests, i, "Stringify");
        }
        if((unsigned)suite & (unsigned)eYYY_IRCSuiteParse){
            YYY_RUN_TEST_SUITE(YYY_RunParseTests, i, "Parse");
        }
    }
    free(buffer);
    return EXIT_SUCCESS;
}
