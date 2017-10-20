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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/*---------------------------------------------------------------------------*/

#include "yyy_buffer.h"
#include "../test/yyy_test.h"

#include <stdlib.h>
#include <string>

/*---------------------------------------------------------------------------*/

class YYY_BufferHolder{
public:
    YYY_MSGBuffer *const m_buffer;
    
    YYY_BufferHolder()
      : m_buffer((YYY_MSGBuffer*)malloc(YYY_MSGBufferSize())){
        YYY_InitMSGBuffer(m_buffer);
    }
    
    ~YYY_BufferHolder(){
        YYY_DestroyMSGBuffer(m_buffer);
        free(m_buffer);
    }
    
    void putMessage(const char *str, unsigned len){
        std::string msg;
        msg.assign(str, len);
        msg += "\r\n";
        YYY_PutMSGBuffer(m_buffer, msg.c_str(), msg.length());
    }
    
    void putMessage(const std::string &str){
        putMessage(str.c_str(), str.length());
    }
    
    std::string getMessage(){
        std::string str;
        char output[YYY_MAX_MSG_LEN];
        const unsigned written = YYY_GetMSGBuffer(m_buffer, output);
        str.assign(output, written);
        return str;
    }
};

/*---------------------------------------------------------------------------*/

static int ConstructionTest(){
    // Just test that we don't crash.
    YYY_BufferHolder buffer = YYY_BufferHolder();   
    return 1;
}

/*---------------------------------------------------------------------------*/

static int PutMessageTest(){
    int SUCCESS_INDICATOR = 1;
    
    const std::string in_msg = "Test message, please ignore.";
    
    YYY_BufferHolder buffer = YYY_BufferHolder();
    buffer.putMessage(in_msg);
    const std::string out_msg = buffer.getMessage();
    YYY_EXPECT_INT_EQ(out_msg.size(), in_msg.size());
    YYY_EXPECT_YYY_STR_EQ_N(out_msg.c_str(), in_msg.c_str(), in_msg.size());
    
    return SUCCESS_INDICATOR;
}

/*---------------------------------------------------------------------------*/

static int PutEmptyMessageTest(){
    int SUCCESS_INDICATOR = 1;
    
    YYY_BufferHolder buffer = YYY_BufferHolder();
    buffer.putMessage("", 0);
    const std::string msg = buffer.getMessage();
    YYY_EXPECT_INT_EQ(msg.size(), 0);
    
    return SUCCESS_INDICATOR;
}


/*---------------------------------------------------------------------------*/

static int WraparoundMessagesTest(){
    int SUCCESS_INDICATOR = 1;
    
    const std::string in_msg = "This is a LOOOOOOOOOOOOOOOOOOOOOOOOOONG message!";
    YYY_BufferHolder buffer = YYY_BufferHolder();
    
    unsigned n_chars = 0, n_messages = 0;
    do{
        buffer.putMessage(in_msg);
        n_chars += in_msg.length();
        n_messages++;
    }while(n_chars < YYY_MAX_MSG_LEN - in_msg.length());
    
    YYY_ASSERT_INT_NOT_EQ(n_messages, 0);
    YYY_ASSERT_INT_NOT_EQ(n_messages, 1);
    
    while(n_messages > 1){
        const std::string out_msg = buffer.getMessage();
        YYY_ASSERT_INT_EQ(out_msg.size(), in_msg.size());
        YYY_EXPECT_YYY_STR_EQ_N(out_msg.c_str(), in_msg.c_str(), in_msg.size());
        n_messages--;
    }
    
    // There should be one remaining message, which stops the read and write
    // indices from being reset. If we write one more, we will overflow, and
    // two more and we will have one message on the other side.
    
    const std::string test_msg = "Please do not panic.";
    buffer.putMessage(in_msg);
    buffer.putMessage(test_msg);

    for(unsigned i = 0; i < 2; i++){
        const std::string out_msg = buffer.getMessage();
        YYY_ASSERT_INT_EQ(out_msg.size(), in_msg.size());
        YYY_EXPECT_YYY_STR_EQ_N(out_msg.c_str(), in_msg.c_str(), in_msg.size());
    }
    
    {
        const std::string out_msg = buffer.getMessage();
        YYY_ASSERT_INT_EQ(out_msg.size(), test_msg.size());
        YYY_EXPECT_YYY_STR_EQ_N(out_msg.c_str(), test_msg.c_str(), test_msg.size());
    }
    
    {
        const std::string empty_msg = buffer.getMessage();
        YYY_ASSERT_INT_EQ(empty_msg.size(), 0);
    }
    
    return SUCCESS_INDICATOR;
}

/*---------------------------------------------------------------------------*/

const struct YYY_Test YYY_BufferTests[] = {
    YYY_TEST(ConstructionTest),
    YYY_TEST(PutMessageTest),
    YYY_TEST(PutEmptyMessageTest),
    YYY_TEST(WraparoundMessagesTest)
};

/*---------------------------------------------------------------------------*/

YYY_TEST_FUNCTION(YYY_RunBufferTests, YYY_BufferTests, "yyy_buffer")

/*---------------------------------------------------------------------------*/

int main(int argc, char **argv){
    
    int i = 0;
    
    (void)argc;
    (void)argv;
    
    YYY_RUN_TEST_SUITE(YYY_RunBufferTests, i, "yyy_buffer");
    return EXIT_SUCCESS;
}
