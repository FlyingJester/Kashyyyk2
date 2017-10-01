
CXX?=g++
CC?=gcc
LINKER?=$(CXX)

CCFLAGS=-I/usr/local/include -I${PWD}/../fltk-gcc/ -I${PWD} -I${PWD}/utils -Wall -Wextra -pedantic -Werror -g -DWIN32 -fstack-protector-all
CFLAGS=$(CCFLAGS)
CXXFLAGS=$(CFLAGS) -fno-rtti -fno-exceptions -std=c++98 -Wno-unused-parameter
LINKFLAGS=-L/usr/local/lib -g -L${PWD}/../fltk-gcc/lib

OBJDIR=objects
GENDIR=build
LIBDIR=lib
