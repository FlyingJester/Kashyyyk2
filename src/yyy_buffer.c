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

#include "yyy_buffer.h"

#include <assert.h>

#define YYY_BUF_LEN 640

/*---------------------------------------------------------------------------*/

struct YYY_MSGBuffer {
    unsigned char crlf_count;
    unsigned short insert, remove;
    char buffer[YYY_BUF_LEN];
};

/*---------------------------------------------------------------------------*/

unsigned YYY_MSGBufferSize(){
    return sizeof(struct YYY_MSGBuffer);
}

/*---------------------------------------------------------------------------*/

static char yyy_get_last_char(const struct YYY_MSGBuffer *buffer,
    const char default_char){
    const unsigned short insert = buffer->insert;

    if(insert == buffer->remove)
        return default_char;
    
    if(insert == 0)
        return buffer->buffer[YYY_BUF_LEN-1];

    return buffer->buffer[insert-1];
}

/*---------------------------------------------------------------------------*/

void YYY_InitMSGBuffer(struct YYY_MSGBuffer *buffer){
    buffer->crlf_count = 0;
    buffer->insert = 0;
    buffer->remove = 0;
}

/*---------------------------------------------------------------------------*/

void YYY_DestroyMSGBuffer(struct YYY_MSGBuffer *buffer){
    (void)buffer;
}

/*---------------------------------------------------------------------------*/

void YYY_PutMSGBuffer(struct YYY_MSGBuffer *YYY_RESTRICT buffer,
    const void *YYY_RESTRICT message,
    unsigned message_len){
    if(message_len != 0){
        char finalchr = yyy_get_last_char(buffer, '\0');
        
        unsigned i;
        for(i = 0; i < message_len; i++){
            const unsigned short insert = buffer->insert;
            const char c = ((char*)message)[i];
            
            buffer->buffer[insert] = c;
            if(finalchr == '\r' && c == '\n')
                buffer->crlf_count++;
            
            finalchr = c;
            
            if(insert == YYY_BUF_LEN - 1)
                buffer->insert = 0;
            else
                buffer->insert++;
        }
    }
}

/*---------------------------------------------------------------------------*/

unsigned YYY_GetMSGBuffer(struct YYY_MSGBuffer *buffer, char *output){
    unsigned short i = 0, remove = buffer->remove;
    const unsigned short insert = buffer->insert;
    char last_char = '\0';
    
    if(buffer->crlf_count == 0)
        return 0;
    
    while(remove != insert){
        const char c = buffer->buffer[remove++];
        if(last_char == '\r' && c == '\n'){
            buffer->crlf_count--;
            break;
        }
        
        last_char = c;
        
        if(c != 0x7F && (c == '\t' || c >= ' '))
            output[i++] = c;

        assert(remove <= YYY_BUF_LEN + 1);
        if(remove >= YYY_BUF_LEN + 1)
            remove = 0;
    }
    
    output[i] = '\0';
    
    buffer->remove = remove;
    return i;
}
