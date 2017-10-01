@echo off

rem Any copyright of this file is dedicated to the Public Domain.
rem http://creativecommons.org/publicdomain/zero/1.0/

set YYY_CHAT_BITS=32
set YYY_CHAT_MODE=RELEASE
set YYY_CHAT_VERBOSE=n
set YYY_CHAT_CRT=T
set YYY_CHAT_TARGET=

:loop
if not "%1"=="" (
    if "%1"=="/32" (
        set YYY_CHAT_BITS=32
        shift
        goto :loop
    )
    if "%1"=="/64" (
        set YYY_CHAT_BITS=64
        shift
        goto :loop
    )
    if "%1"=="/release" (
        set YYY_CHAT_MODE=RELEASE
        shift
        goto :loop
    )
    if "%1"=="/rdebug" (
        set YYY_CHAT_MODE=RDEBUG
        shift
        goto :loop
    )
    if "%1"=="/relwithasserts" (
        set YYY_CHAT_MODE=RDEBUG
        shift
        goto :loop
    )
    if "%1"=="/debug" (
        set YYY_CHAT_MODE=DEBUG
        shift
        goto :loop
    )
    if "%1"=="/v" (
        set YYY_CHAT_VERBOSE=y
        shift
        goto :loop
    )
    if "%1"=="/q" (
        set YYY_CHAT_VERBOSE=n
        shift
        goto :loop
    )
    if "%1"=="/MT" (
        set YYY_CHAT_CRT=T
        shift
        goto :loop
    )
    if "%1"=="/MTd" (
        set YYY_CHAT_CRT=Td
        shift
        goto :loop
    )
    if "%1"=="/MD" (
        set YYY_CHAT_CRT=D
        shift
        goto :loop
    )
    if "%1"=="/MDd" (
        set YYY_CHAT_CRT=Dd
        shift
        goto :loop
    )
    if "%1"=="/clean" (
        set YYY_CHAT_TARGET=clean
        shift
        goto :loop
    )
    
    shift goto :loop
)

if "%YYY_CHAT_VERBOSE%"=="y" (
    echo Building YYY Chat for %YYY_CHAT_BITS% bits in %YYY_CHAT_MODE% mode
)

nmake /nologo /f makefile.msvc %YYY_CHAT_TARGET% CRT=%YYY_CHAT_CRT% MODE=%YYY_CHAT_MODE% BITS=%YYY_CHAT_BITS%
