# - copy&paste this to any new Makefile, !!! modify include !!!
# autodetect <srcdir> as the path to this Makefile
ifndef srcdir
 srcdir:=$(dir $(lastword $(MAKEFILE_LIST)))
endif
# include the platform header
# - it includes config.make and may overwrite anything set before
include $(srcdir)make/build.make
# - now we can use "include $(makedir)..."
# - platform initialized, end of copy&paste 

# -- Library --

# Parameter pack
LIB_FLG:=$(call em_flags,lib)
$(LIB_FLG):INCLUDE_DIRS:=$(srcdir)include
$(LIB_FLG):FLAGS:=-std=c11 -Wall -Wextra

B64:=$(call em_link_lib,b64,$(call em_compile,$(wildcard $(srcdir)b64/*.c),$(LIB_FLG)))
STR:=$(call em_link_lib,str,$(call em_compile,$(wildcard $(srcdir)src/*.c),$(LIB_FLG)))

all:$(STR) $(B64)

# TODO : install
#HDRS:=$(wildcard $(srcdir)include/str/*.h)

# -- Unit tests --

STR_CHECK_FLG:=$(call em_flags,str_check)
$(STR_CHECK_FLG):INCLUDE_DIRS:=$(srcdir)include
$(STR_CHECK_FLG):FLAGS:=-std=c++11 -Wall -Wextra

check:$(call em_link_bin,check,$(call em_compile,$(wildcard $(srcdir)check/*.cpp),$(STR_CHECK_FLG)) $(STR) $(B64))
	$<

# -- Unit tests --

STR_INFO_FLG:=$(call em_flags,str_info)
$(STR_INFO_FLG):INCLUDE_DIRS:=$(srcdir)include
$(STR_INFO_FLG):FLAGS:=-std=c++11 -Wall -Wextra

info:$(call em_link_bin,info,$(call em_compile,$(wildcard $(srcdir)info/*.cpp),$(STR_INFO_FLG)) $(STR) $(B64))
	$<

#end
