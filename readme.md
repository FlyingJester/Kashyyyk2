Kashyyyk 2
==========

Simple, Lightweight, Cross-Platform IRC Client with Minimal Dependencies

******

Why you want to use Kashyyyk 2
------------------------------

 - Kashyyyk 2 is lightweight, fast, and efficient.
 - Intuitive interface
 - Native support for Windows, Linux, OpenBSD, and FreeBSD

License
-------

Kashyyyk 2 is licensed under a permissive BSD-like license with a derivative
naming clause.

License text:

```
  Copyright (c) 2014-2017 Martin McDonough.  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
 
 - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
 
 - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
 
 - Products derived from this software may not be called "Kashyyyk", nor may
     "YYY" appear in their name, without prior written permission of
     the copyright holders.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 IN THE SOFTWARE.
```

About Kashyyyk 2
----------------

Kashyyyk 2 is based on the original Kashyyyk IRC client, which was designed to
be similar to HexChat but have better OS X support. Kashyyyk 2 focuses on being
lightweight and efficient on resources, without sacrificing user experience.
This includes having very minimal dependencies, with most platform/osdep code
being written specifically for the needs of Kashyyyk 2.

Kashyyyk 2 supports many compilers and platforms.

#### Supported Compilers
 - MSVC 2015
 - MSYS2/MinGW
 - GCC 4, 6
 - OpenWatcom C/C++ 1.9

#### Supported Platforms
 - Windows 7, 10
 - Linux 2.6+
 - FreeBSD 10, 11
 - OpenBSD 5.9, 6.1

Building for different compilers may have different build system requirements.

Building Kashyyyk 2
------------------

The requirements for building Kashyyyk are different depending on what compiler
and platform you are using. For all platforms, FLTK 1.3+ is needed.

Building Kashyyyk 2 with MSVC
-----------------------------

To build with MSVC 2015, open the Kashyyyk2.sln file.

You will need to build FLTK. If you are building a release build, be sure to
override the flags for FLTK to use /MT instead of /MD.

Place the fltk.lib or fltkd.lib file into the fltk directory of the Kashyyyk 2
root. Copy the FL header directory and the fluid binaries into the fltk
directory as well, and be sure to get the abi-version.h file from the build
directory of FLTK.

Building Kashyyyk 2 with OpenWatcom
-----------------------------------

Building with OpenWatcom can be done by running `wmake -f makefile.wat` in the
Kashyyyk 2 root. You will need to put the FLTK source directory in the same
directory as the Kashyyyk 2 root, and build from 'fltk-watcom/build' in FLTK
for the libraries and tools (fluid) to be found.

Building FLTK with OpenWatcom is possible, but not simple. This deserves its
own section (TODO!)

Building Kashyyyk 2 with MSYS/MinGW
-----------------------------------

Building with OpenWatcom can be done by running `make -f makefile.gcc` in the
Kashyyyk 2 root. You will need to put the FLTK source directory in the same
directory as the Kashyyyk 2 root, and build from 'fltk/build' in FLTK
for the libraries and tools (fluid) to be found.

Building Kashyyyk 2 with GCC on Linux/BSD
-----------------------------------------

Building Kashyyyk 2 on Linux/BSD requires the SCons build tool, which requires
Python. The SCons files should work with either Python 2.7 or Python 3 versions
of SCons.

After FLTK and Fluid have been installed, run SCons from the Kashyyyk 2 root
directory.
