@echo off

rem Any copyright of this file is dedicated to the Public Domain.
rem http://creativecommons.org/publicdomain/zero/1.0/

set YYY_MONITOR_BITS=32
set YYY_MONITOR_MODE=RELEASE
set YYY_MONITOR_VERBOSE=n
set YYY_MONITOR_CRT=T
set YYY_MONITOR_TARGET=

:loop
if not "%1"=="" (
    if "%1"=="/32" (
        set YYY_MONITOR_BITS=32
        shift
        goto :loop
    )
    if "%1"=="/64" (
        set YYY_MONITOR_BITS=64
        shift
        goto :loop
    )
    if "%1"=="/release" (
        set YYY_MONITOR_MODE=RELEASE
        shift
        goto :loop
    )
    if "%1"=="/rdebug" (
        set YYY_MONITOR_MODE=RDEBUG
        shift
        goto :loop
    )
    if "%1"=="/relwithasserts" (
        set YYY_MONITOR_MODE=RDEBUG
        shift
        goto :loop
    )
    if "%1"=="/debug" (
        set YYY_MONITOR_MODE=DEBUG
        shift
        goto :loop
    )
    if "%1"=="/v" (
        set YYY_MONITOR_VERBOSE=y
        shift
        goto :loop
    )
    if "%1"=="/q" (
        set YYY_MONITOR_VERBOSE=n
        shift
        goto :loop
    )
    if "%1"=="/MT" (
        set YYY_MONITOR_CRT=T
        shift
        goto :loop
    )
    if "%1"=="/MTd" (
        set YYY_MONITOR_CRT=Td
        shift
        goto :loop
    )
    if "%1"=="/MD" (
        set YYY_MONITOR_CRT=D
        shift
        goto :loop
    )
    if "%1"=="/MDd" (
        set YYY_MONITOR_CRT=Dd
        shift
        goto :loop
    )
    if "%1"=="/clean" (
        set YYY_MONITOR_TARGET=clean
        shift
        goto :loop
    )
    
    shift goto :loop
)

if "%YYY_MONITOR_VERBOSE%"=="y" (
    echo Building YYY Monitor for %YYY_MONITOR_BITS% bits in %YYY_MONITOR_MODE% mode
)

nmake /nologo /f makefile.msvc %YYY_MONITOR_TARGET% CRT=%YYY_MONITOR_CRT% MODE=%YYY_MONITOR_MODE% BITS=%YYY_MONITOR_BITS%
