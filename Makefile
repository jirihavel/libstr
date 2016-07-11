B64_INCLUDE_DIRS?=

srcdir?=

src_to_obj=$(patsubst $(srcdir)%,obj/%.o,$1)
obj_to_dep=$(patsubst %.o,%.d,$1)

# -- Library --

.PHONY:all
all:libstr.a

SRCS:=$(wildcard $(srcdir)src/*.c)

CPPFLAGS:=-Wall -Wextra -Iinclude
CFLAGS:=-std=c11 -g0 -O3
ARFLAGS:=rcs

OBJS:=$(call src_to_obj,$(SRCS))

libstr.a:$(OBJS)
	$(AR) $(ARFLAGS) $@ $^

obj/%.c.o:$(srcdir)%.c
	$(CC) -c -MMD -o $@ $(CPPFLAGS) $(CFLAGS) $<

-include $(call obj_to_dep,$(OBJS))

.PHONY:install

INSTALL?=install
INSTALL_DATA?=$(INSTALL) -m 644

DESTDIR?=
prefix?=/usr/local
exec_prefix?=$(prefix)
includedir?=$(prefix)/include
libdir?=$(exec_prefix)/lib

install:libstr.a
	$(INSTALL_DATA) $(wildcard *.h) $(DESTDIR)$(includedir)/
	$(INSTALL_DATA) libstr.a $(DESTDIR)$(libdir)/

# -- Unit tests --

CHECK_SRCS:=$(wildcard $(srcdir)check/*.cpp)

CXXFLAGS:=-std=c++14

.PHONY:check

check:check.exe
	$<

CHECK_OBJS:=$(call src_to_obj,$(CHECK_SRCS))

check.exe:$(CHECK_OBJS) libstr.a
	$(CXX) -o $@ $^

obj/%.cpp.o:$(srcdir)%.cpp
	$(CXX) -c -MMD -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

-include $(call obj_to_dep,$(CHECK_OBJS))

