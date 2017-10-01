@echo off

rem Any copyright of this file is dedicated to the Public Domain.
rem http://creativecommons.org/publicdomain/zero/1.0/

set YYY_THREAD_BITS=32
set YYY_THREAD_MODE=RELEASE
set YYY_THREAD_VERBOSE=n
set YYY_THREAD_CRT=T
set YYY_THREAD_TARGET=

:loop
if not "%1"=="" (
    if "%1"=="/32" (
        set YYY_THREAD_BITS=32
        shift
        goto :loop
    )
    if "%1"=="/64" (
        set YYY_THREAD_BITS=64
        shift
        goto :loop
    )
    if "%1"=="/release" (
        set YYY_THREAD_MODE=RELEASE
        shift
        goto :loop
    )
    if "%1"=="/rdebug" (
        set YYY_THREAD_MODE=RDEBUG
        shift
        goto :loop
    )
    if "%1"=="/relwithasserts" (
        set YYY_THREAD_MODE=RDEBUG
        shift
        goto :loop
    )
    if "%1"=="/debug" (
        set YYY_THREAD_MODE=DEBUG
        shift
        goto :loop
    )
    if "%1"=="/v" (
        set YYY_THREAD_VERBOSE=y
        shift
        goto :loop
    )
    if "%1"=="/q" (
        set YYY_THREAD_VERBOSE=n
        shift
        goto :loop
    )
    if "%1"=="/MT" (
        set YYY_THREAD_CRT=T
        shift
        goto :loop
    )
    if "%1"=="/MTd" (
        set YYY_THREAD_CRT=Td
        shift
        goto :loop
    )
    if "%1"=="/MD" (
        set YYY_THREAD_CRT=D
        shift
        goto :loop
    )
    if "%1"=="/MDd" (
        set YYY_THREAD_CRT=Dd
        shift
        goto :loop
    )
    if "%1"=="/clean" (
        set YYY_THREAD_TARGET=clean
        shift
        goto :loop
    )
    
    shift goto :loop
)

if "%YYY_THREAD_VERBOSE%"=="y" (
    echo Building YYY Thread for %YYY_THREAD_BITS% bits in %YYY_THREAD_MODE% mode
)

nmake /nologo /f makefile.msvc %YYY_THREAD_TARGET% CRT=%YYY_THREAD_CRT% MODE=%YYY_THREAD_MODE% BITS=%YYY_THREAD_BITS%
