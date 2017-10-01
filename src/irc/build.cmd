@echo off

rem Any copyright of this file is dedicated to the Public Domain.
rem http://creativecommons.org/publicdomain/zero/1.0/

set YYY_IRC_BITS=32
set YYY_IRC_MODE=RELEASE
set YYY_IRC_VERBOSE=n
set YYY_IRC_CRT=T
set YYY_IRC_TARGET=

:loop
if not "%1"=="" (
    if "%1"=="/32" (
        set YYY_IRC_BITS=32
        shift
        goto :loop
    )
    if "%1"=="/64" (
        set YYY_IRC_BITS=64
        shift
        goto :loop
    )
    if "%1"=="/release" (
        set YYY_IRC_MODE=RELEASE
        shift
        goto :loop
    )
    if "%1"=="/rdebug" (
        set YYY_IRC_MODE=RDEBUG
        shift
        goto :loop
    )
    if "%1"=="/relwithasserts" (
        set YYY_IRC_MODE=RDEBUG
        shift
        goto :loop
    )
    if "%1"=="/debug" (
        set YYY_IRC_MODE=DEBUG
        shift
        goto :loop
    )
    if "%1"=="/v" (
        set YYY_IRC_VERBOSE=y
        shift
        goto :loop
    )
    if "%1"=="/q" (
        set YYY_IRC_VERBOSE=n
        shift
        goto :loop
    )
    if "%1"=="/MT" (
        set YYY_IRC_CRT=T
        shift
        goto :loop
    )
    if "%1"=="/MTd" (
        set YYY_IRC_CRT=Td
        shift
        goto :loop
    )
    if "%1"=="/MD" (
        set YYY_IRC_CRT=D
        shift
        goto :loop
    )
    if "%1"=="/MDd" (
        set YYY_IRC_CRT=Dd
        shift
        goto :loop
    )
    if "%1"=="/clean" (
        set YYY_IRC_TARGET=clean
        shift
        goto :loop
    )
    
    shift goto :loop
)

if "%YYY_IRC_VERBOSE%"=="y" (
    echo Building YYY IRC for %YYY_IRC_BITS% bits in %YYY_IRC_MODE% mode
)

nmake /nologo /f makefile.msvc %YYY_IRC_TARGET% CRT=%YYY_IRC_CRT% MODE=%YYY_IRC_MODE% BITS=%YYY_IRC_BITS%
