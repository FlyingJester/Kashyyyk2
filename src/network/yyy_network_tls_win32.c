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

#ifndef YYY_NETWORK_WIN32
#error The Win32 TLS backend requires the use of WinSock.
#endif

#include "yyy_network.h"
#include "yyy_network_win32.h"

#define YYY_TLS_INTERNAL 1
#include "yyy_network_tls.h"
#undef YYY_TLS_INTERNAL

#include <Mstcpip.h>
#include <Ws2tcpip.h>
#include <TCHAR.h>

enum YYY_NetworkError YYY_EnableSocketTLS(struct YYY_NetworkSocket *socket){
    SOCKET_SECURITY_SETTINGS settings;
    settings.SecurityProtocol = SOCKET_SECURITY_PROTOCOL_DEFAULT;
    settings.SecurityFlags = SOCKET_SETTINGS_GUARANTEE_ENCRYPTION;

    {
        const int err = WSASetSocketSecurity(socket->socket,
            &settings,
            sizeof(SOCKET_SECURITY_SETTINGS),
            NULL,
            NULL);
        
        switch(err){
            case 0:
                socket->has_tls = 1;
                return eYYYNetworkSuccess;
            case WSAEAFNOSUPPORT: /* FALLTHROUGH */
            case WSAEMSGSIZE: /* FALLTHROUGH */
            case WSAEINVAL:
            case WSAENOTSOCK:
                return eYYYNetworkFatal;
            case WSAEISCONN:
                return eYYYNetworkAlreadyConnected;
            default:
#ifndef NDEBUG
                {
                    TCHAR buffer[80];
                    _sntprintf_s(buffer, 80, _TRUNCATE, TEXT("WSASetSocketSecurity error: %i\n"), err);
                    OutputDebugString(buffer);
                }
#endif
                return eYYYNetworkFailure;
        }
    }
}

void YYY_TLSBeginConnection(struct YYY_NetworkSocket *s){ (void)s; }

void YYY_TLSEndConnection(struct YYY_NetworkSocket *s){ (void)s; }

void *YYY_TLSDecode(struct YYY_NetworkSocket *s, void *data, size_t size, size_t *out_size){
    (void)s;
    out_size[0] = size;
    return data;
}

void YYY_TLSFreeDecode(struct YYY_NetworkSocket *s, void *v){
    (void)s;
    (void)v;
}

const void *YYY_TLSEncode(struct YYY_NetworkSocket *s,
    const void *data,
    size_t size,
    size_t *out_size){
    
    (void)s;
    out_size[0] = size;
    return data;
}

void YYY_TLSFreeEncode(struct YYY_NetworkSocket *s, void *v){
    (void)s;
    (void)v;
}
