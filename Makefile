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

.PHONY:install installdirs

INSTALL?=install
INSTALL_DATA?=$(INSTALL) -m 644

default_prefix?=/usr/local
default_exec_prefix?=$(prefix)
default_includedir?=$(prefix)/include
default_libdir?=$(exec_prefix)/lib

DESTDIR?=
prefix?=$(default_prefix)
exec_prefix?=$(default_exec_prefix)
includedir?=$(default_includedir)
libdir?=$(default_libdir)

HDRS:=$(wildcard $(srcdir)include/str/*.h)

install:libstr.a installdirs
	$(INSTALL_DATA) $(HDRS) $(subst /,\,$(DESTDIR)$(includedir)/str/)
	$(INSTALL_DATA) libstr.a $(subst /,\,$(DESTDIR)$(libdir)/)
	$(file >$(DESTDIR)$(libdir)/pkgconfig/libstr.pc,prefix=$(prefix))
	cat libstr.pc >>$(DESTDIR)$(libdir)/pkgconfig/libstr.pc

mkdir_1=if not exist $1 md $1 2>nul
mkdir=$(call mkdir_1,$(subst /,\,$1))

installdirs:
	$(call mkdir,$(DESTDIR)$(includedir)/str/)
	$(call mkdir,$(DESTDIR)$(libdir)/pkgconfig/)

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
