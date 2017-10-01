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

#define _CRT_SECURE_NO_WARNINGS

#include "yyy_prefs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

/*---------------------------------------------------------------------------*/

/* Literal Compare */
#define YYY_LITCMP(NONLIT, LIT, LEN)\
    (LEN + 1 == sizeof(LIT) && memcmp(NONLIT, LIT, sizeof(LIT) - 1) == 0)

#ifdef __GNUC__
#define inline __inline
#endif

enum EnumParserSection {
    ePrefs,
    eDefaults,
    eServer
};

enum EnumParserKey {
    eUnknown,
    eTheme,
    eUsername = eTheme,
    eServers,
    eChannels = eServers,
    ePort,
    eUrl,
};

static inline unsigned yyy_parse_to_newline(FILE *const file){
    while(!feof(file)){
        const int ic = fgetc(file);
        if(ic == EOF || ic == '\n')
            break;
    }
    return 1;
}

static enum EnumParserSection yyy_parse_section(FILE *const file,
    char **buffer, unsigned *len){
    unsigned at = 0;
    
    assert(buffer != NULL);
    assert(*buffer != NULL);
    
    while(!feof(file)){
        const int ic = fgetc(file);
        const char c = ic;
        if(ic == EOF)
            break;
        
        if(at + 1 >= *len){
            len[0] <<= 1;
            buffer[0] = realloc(*buffer, *len);
        }
        
        if(c == '\n' || c == '\r' || (c == ']' && yyy_parse_to_newline(file))){
            
            printf("%c -> ", c);
            buffer[0][at] = '\0';
            
            if(YYY_LITCMP(*buffer, "prefs", at))
                return ePrefs;
            else if(YYY_LITCMP(*buffer, "defaults", at))
                return eDefaults;
            else
                return eServer;
            
        }
        else if(c == '#' || c == ';'){
            buffer[0][at] = '\0';
            yyy_parse_to_newline(file);
            return eServer;
        }
        
        buffer[0][at++] = c;
    }

    buffer[0][at] = '\0';
    return eServer;
}

static inline enum EnumParserKey yyy_parse_key(FILE *const file,
    const char start, enum EnumParserSection section){

    char buffer[12];
    unsigned short at = 0;
    buffer[at++] = start;
    
    while(at + 1u < sizeof(buffer) && !feof(file)){
        const int ic = fgetc(file);
        const char c = ic;
        if(ic == EOF)
            return eUnknown;
        
        if(c == '\n' || c == '\r')
            return eUnknown;
        else if(c == '#' || c == ';'){
            yyy_parse_to_newline(file);
            return eUnknown;
        }
        else if(c == '='){
#define KEY_FOR_SECTION(KEY, SECTION)\
    return( (section == SECTION) ? KEY : eUnknown)

#define KEY_NOT_FOR_SECTION(KEY, SECTION)\
    return ( (section != SECTION) ? KEY : eUnknown)

            if(YYY_LITCMP(buffer, "theme", at))
                KEY_FOR_SECTION(eTheme, ePrefs);
            else if(YYY_LITCMP(buffer, "username", at))
                KEY_NOT_FOR_SECTION(eUsername, ePrefs);
            else if(YYY_LITCMP(buffer, "servers", at))
                KEY_FOR_SECTION(eServers, eDefaults);
            else if(YYY_LITCMP(buffer, "channels", at))
                KEY_FOR_SECTION(eChannels, eServer);
            else if(YYY_LITCMP(buffer, "port", at))
                KEY_NOT_FOR_SECTION(ePort, ePrefs);
            else if(YYY_LITCMP(buffer, "url", at))
                KEY_FOR_SECTION(eUrl, eServer);
            else{
                unsigned i = 0;
                fputs("Did not match ", stdout);
                while(i < at){
                    putchar(buffer[i++]);
                }
                putchar('\n');
                fflush(stdout);
            }
#undef KEY_FOR_SECTION
#undef KEY_NOT_FOR_SECTION

        }
        else
            buffer[at++] = c;
    }
    
    yyy_parse_to_newline(file);
    return eUnknown;
}

static void yyy_parse_value(FILE *const file,
    char **buffer, unsigned *len){
    unsigned at = 0;
    assert(buffer != NULL);
    assert(*buffer != NULL);

    while(!feof(file)){
        
        const int ic = fgetc(file);
        const char c = ic;
        if(ic == EOF)
            break;
        
        if(at + 1 >= *len){
            len[0] <<= 1;
            buffer[0] = realloc(*buffer, *len);
        }
        
        if(c == '\n' || c == '\r' ||
            ((c == '#' || c == ';') && yyy_parse_to_newline(file))){
            break;
        }
        else
            buffer[0][at++] = c;
    }
    buffer[0][at] = '\0';
}

static void yyy_parse_csv(FILE *const file,
    char ***out, unsigned short *num_out){
    
    unsigned at = 0, len = 32;
    char *arena = malloc(len);
    unsigned n = 0;
    
    while(!feof(file)){
        const int ic = fgetc(file);
        const char c = ic;
        if(ic == EOF)
            break;
        
        if(at + 1 >= len){
            len <<= 1;
            arena = realloc(arena, len);
        }
        
        if(c == ','){
            arena[at++] = '\0';
            n++;
        }
        else if(c == '\n' || c == '\r' ||
            ((c == '#' || c == ';') && yyy_parse_to_newline(file))){
            n++;
            break;
        }
        else{
            arena[at++] = c;
        }
    }
    
    if(n == 0 || at == 0){
        free(arena);
        out[0] = NULL;
        num_out[0] = 0;
        return;
    }
    
    arena[at] = '\0';
    
    out[0] = malloc((n * sizeof(char*)));
    
    len = 0;
    at = 0;
    do{
        out[0][len++] = arena + at;
        
        while(arena[at++]) {}
    }while(len != n);
    
    num_out[0] = n;
}

static inline void yyy_free_arena(char *const *const arena){
    free(*arena);
    free((void*)arena);
}

static void yyy_ready_buffer(char **str, unsigned *len){
    if(*str == NULL){
        len[0] = 32;
        str[0] = malloc(32);
    }
    else{
        len[0] = strlen(*str);
    }
}

static inline int yyy_atoi(const char *const str){
    unsigned long l = 0;
    unsigned short i = 0;
    char c;
    while((c = str[i++])){
        if(c >= '0' && c <= '9'){
            l *= 10;
            l += c - '0';
        }
    }
    
    return l;
}

bool YYY_LoadPrefs(const char *path, struct YYY_Prefs *const prefs){
    FILE *const file = fopen(path, "rb");
    if(!file)
        return false;
    else{
        enum EnumParserSection section = ePrefs;
        unsigned section_len = 32, value_len = 32;
        char *section_buffer = malloc(section_len), *value_buffer = malloc(value_len);
        
        memset(prefs, 0, sizeof(struct YYY_Prefs));
        
        while(!feof(file)){
            const int ic = fgetc(file);
            const char c = ic;
            if(ic == EOF)
                break;
            if(c == '#' || c == ';'){
                yyy_parse_to_newline(file);
                continue;
            }
            else if(c == '['){
                section = yyy_parse_section(file, &section_buffer, &section_len);
                printf("Section: [%s]\n", section_buffer);
                if(section == eServer){
                    prefs->num_servers++;
                    prefs->servers = realloc(prefs->servers,
                        prefs->num_servers * sizeof(struct YYY_ServerPrefs));
                    {
                        struct YYY_ServerPrefs *const server =
                            prefs->servers + (prefs->num_servers - 1);
                        
                        memset(server, 0, sizeof(struct YYY_ServerPrefs));
                        
                        server->name = section_buffer;
                    }
                    section_len = 32;
                    section_buffer = malloc(section_len);
                }
                continue;
            }
            else if(c == '\n' || c == '\r' || c == ' ' || c == '\t')
                continue;
            else{
                const enum EnumParserKey key = yyy_parse_key(file, c, section);
                unsigned len = 0;
                
                printf("Key %i, Section %i\n", key, section); fflush(stdout);
                
                if(key == eChannels && section == eServer){
                    /* Default channels for a server */
                    struct YYY_ServerPrefs *const server =
                        prefs->servers + (prefs->num_servers - 1);
                    
                    assert(prefs->num_servers != 0);
                    
                    if(server->channels)
                        yyy_free_arena(server->channels);
                    
                    yyy_parse_csv(file, &(server->channels),
                        &(server->num_channels));
                }
                else if(key == eServers && section == eDefaults){
                    /* Default servers */
                    if(prefs->autojoin_servers)
                        yyy_free_arena(prefs->autojoin_servers);
                    
                    yyy_parse_csv(file, &(prefs->autojoin_servers),
                        &(prefs->num_autojoin_servers));
                }
                else if(key == eTheme && section == ePrefs){
                    puts("Theme"); fflush(stdout);
                    yyy_ready_buffer(&prefs->theme, &len);
                    yyy_parse_value(file, &prefs->theme, &len);
                }
                else if(key == ePort){
                    puts("Port"); fflush(stdout);
                    yyy_parse_value(file, &value_buffer, &value_len);
                    if(section == eServer){
                        struct YYY_ServerPrefs *const server =
                            prefs->servers + (prefs->num_servers - 1);
                        assert(prefs->num_servers != 0);
                        server->port = yyy_atoi(value_buffer);
                    }
                    else if(section == eDefaults){
                        prefs->default_port = yyy_atoi(value_buffer);
                    }
                }
                else if(key == eUsername && section == eDefaults){
                    puts("UserName (DEF)"); fflush(stdout);
                    yyy_ready_buffer(&(prefs->default_username), &len);
                    yyy_parse_value(file, &(prefs->default_username), &len);
                }
                else if(key == eUsername && section == eServer){
                    struct YYY_ServerPrefs *const server =
                        prefs->servers + (prefs->num_servers - 1);
                    puts("UserName"); fflush(stdout);
                    assert(prefs->num_servers != 0);
                    yyy_ready_buffer(&(server->username), &len);
                    yyy_parse_value(file, &(server->username), &len);
                }
                else if(key == eUrl && section == eServer){
                    struct YYY_ServerPrefs *const server =
                        prefs->servers + (prefs->num_servers - 1);
                    puts("URL"); fflush(stdout);
                    assert(prefs->num_servers != 0);
                    yyy_ready_buffer(&(server->url), &len);
                    yyy_parse_value(file, &(server->url), &len);
                }
                else{
                    printf("Unknown: %c\n", c); fflush(stdout);
                    yyy_parse_to_newline(file);
                }
            }
        }
        
        free(section_buffer);
        free(value_buffer);
        
        fclose(file);
        
        return true;
    }
}

void YYY_FreePrefs(struct YYY_Prefs *prefs){
    unsigned i;
    
    free(prefs->theme);
    free(prefs->default_username);
    yyy_free_arena(prefs->autojoin_servers);
    
    for(i = 0; i < prefs->num_servers; i++){
        yyy_free_arena(prefs->servers[i].channels);
        free(prefs->servers[i].url);
        free(prefs->servers[i].username);
    }
}
