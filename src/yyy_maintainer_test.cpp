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

#include "utils/yyy_maintainer.hpp"
#include "../test/yyy_test.h"

#include <stdlib.h>
#include <string>

/*---------------------------------------------------------------------------*/

struct ConstructionSignaller{
    int *m_dest;
    
    ConstructionSignaller()
      : m_dest(NULL){
        
    }
    
    ~ConstructionSignaller(){
        m_dest--;
    }
};

typedef YYY::Maintainer<ConstructionSignaller> TestMaintaner;

/*---------------------------------------------------------------------------*/

static int ConstructionTest(){
    // Just test that we don't crash.
    TestMaintaner maintainer = TestMaintaner();   
    return 1;
}

/*---------------------------------------------------------------------------*/

static int PushBackTest(){
    TestMaintaner maintainer = TestMaintaner();
    ConstructionSignaller &that = maintainer.create();
    YYY_ASSERT_INT_NOT_EQ(&that, NULL);
    YYY_ASSERT_INT_EQ(that.m_dest, NULL);
    int test = 0;
    // Write to the allocated memory, see that we don't crash.
    that.m_dest = &test; 
    return 1;
}

/*---------------------------------------------------------------------------*/

static int PushBackContainsTest(){
    TestMaintaner maintainer = TestMaintaner();
    ConstructionSignaller &that = maintainer.create();
    YYY_ASSERT_INT_NOT_EQ(&that, NULL);
    YYY_ASSERT_INT_EQ(that.m_dest, NULL);
    // Check that we can find our one element by iterating.
    TestMaintaner::iterator i = maintainer.begin();
    YYY_ASSERT_FALSE(i == maintainer.end());
    YYY_ASSERT_TRUE(i+1 == maintainer.end());
    YYY_ASSERT_INT_EQ(&(*i), &that);
    return 1;
}

/*---------------------------------------------------------------------------*/

static int PushBackMultipleTest(){
    TestMaintaner maintainer = TestMaintaner();
    ConstructionSignaller &that0 = maintainer.create();
    ConstructionSignaller &that1 = maintainer.create();
    YYY_ASSERT_INT_NOT_EQ(&that0, NULL);
    YYY_ASSERT_INT_NOT_EQ(&that1, NULL);
    YYY_ASSERT_INT_NOT_EQ(&that0, &that1);
    return 1;
}

/*---------------------------------------------------------------------------*/

static int PushBackMultipleContainsTest(){
    int SUCCESS_INDICATOR = 1;
    
    TestMaintaner maintainer = TestMaintaner();
    ConstructionSignaller &that0 = maintainer.create();
    ConstructionSignaller &that1 = maintainer.create();
    unsigned found0 = 0, found1 = 0;
    for(TestMaintaner::iterator i = maintainer.begin(); i != maintainer.end(); i++){
        ConstructionSignaller *const i_ptr = &(*i);
        YYY_EXPECT_TRUE(i_ptr == &that0 || i_ptr == &that1);
        if(i_ptr == &that0)
            found0++;
        if(i_ptr == &that1)
            found1++;
    }
    
    YYY_EXPECT_INT_EQ(found0, 1);
    YYY_EXPECT_INT_EQ(found1, 1);
    
    return SUCCESS_INDICATOR;
}

/*---------------------------------------------------------------------------*/

const struct YYY_Test YYY_MaintainerTests[] = {
    YYY_TEST(ConstructionTest),
    YYY_TEST(PushBackTest),
    YYY_TEST(PushBackContainsTest),
    YYY_TEST(PushBackMultipleTest),
    YYY_TEST(PushBackMultipleContainsTest)
};

/*---------------------------------------------------------------------------*/

YYY_TEST_FUNCTION(YYY_RunMaintainerTests, YYY_MaintainerTests, "YYY::Maintainer")

/*---------------------------------------------------------------------------*/

int main(int argc, char **argv){
    
    int i = 0;
    
    (void)argc;
    (void)argv;
    
    YYY_RUN_TEST_SUITE(YYY_RunMaintainerTests, i, "YYY::Maintainer");
    return EXIT_SUCCESS;
}
