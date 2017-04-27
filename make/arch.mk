#ARCH	:= mips
#mt7688, ar9331,x86

#####################################################
ifeq ($(ARCH),mt7688)
LIBS_DIR	:= /home/au/all/lib/cross
CROSS		:= mipsel-openwrt-linux-
endif

ifeq ($(ARCH),x86)
LIBS_DIR	:= /home/au/all/lib/pc
CROSS		:= 
endif

ifeq ($(ARCH),ar9331)
LIBS_DIR	:= /home/au/all/lib/cross
CROSS		:= mips-openwrt-linux-
endif

#RPC_DIR		:= $(LIBS_DIR)/xmlrpc
#CURL_DIR	:= $(LIBS_DIR)/curl
#MQ_DIR		:= $(LIBS_DIR)/rabbitmq
#JSONC_DIR	:= $(LIBS_DIR)/json-c
#DBUS_DIR	:= $(LIBS_DIR)/dbus
#DBUSCXX_DIR	:= $(LIBS_DIR)/dbus-c++

#3PARTS rpc rabbitmq json dbus dbus-cxx cxx
#RPC_CFLAGS		:= -I$(RPC_DIR)/include
#RPC_LDFLAGS		:= -L$(RPC_DIR)/lib -lxmlrpc \
				   -lxmlrpc_util \
					-lxmlrpc_xmlparse \
					-lxmlrpc_xmltok \
					-lxmlrpc_client \
					-lxmlrpc_server \
					-lxmlrpc_abyss \
					-lxmlrpc_server_abyss 
#CURL_CFLAGS		:=   -I$(CURL_DIR)/include
#CURL_LDFLAGS	:=  -L$(CURL_DIR)/lib -lcurl
#RABBITMQ_CFLAGS	:=  -I$(MQ_DIR)/include
#RABBITMQ_LDFLAGS:= -L$(MQ_DIR)/lib -lrabbitmq
#JSONC_CFLAGS	:=  -I$(JSONC_DIR)/include/json-c
#JSONC_LDFLAGS	:= -L$(JSONC_DIR)/lib -ljson-c
#DBUS_CFLAGS		:=  -I$(DBUS_DIR)/include/dbus-1.0 
#DBUS_LDFLAGS	:= -L$(DBUS_DIR)/lib -ldbus-1
#DBUSCXX_CFLAGS	:= -I$(DBUSCXX_DIR)/include
#CBUSCXX_LDFLAGS := -L($(DBUSCXX_DIR)/lib -ldbus-c++-1
#SEDONA_CFLAGS	:=	-D_DEBUG\
			   		-D__UNIX__=1\
			   		-DSOCKET_FAMILY_INET=1\
			   		-DPLAT_BUILD_VERSION=\"1.2.28\"
#SEDONA_LDFLAGS	:=

CXX_CFLAGS		:= -std=c++0x
CXX_LDFLAGS		:= -lstdc++

SELF_CFLAGS     := -I./inc

ifeq ($(ARCH),ar9331)
SELF_CFLAGS		+= -I../qqsdk/include/
SELF_LDFLAGS	+= -L../qqsdk/lib/ar9331 -lgwsdk -ltxdevicesdk


STAGING_DIR	:= $(OPENWRT_DIR)/staging_dir
CROSS_DIR	:= $(STAGING_DIR)/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin
endif
ifeq ($(ARCH),mt7688)

SELF_CFLAGS		+= -I./inc
SELF_LDFLAGS	+= -I./lib
OPENWRT_DIR	:= /opt/au/openwrt_7620
STAGING_DIR	:= $(OPENWRT_DIR)/staging_dir
CROSS_DIR	:= $(STAGING_DIR)/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin
endif
ifeq ($(ARCH),x86) 
SELF_CFLAGS		+= -I../qqsdk/include/
SELF_LDFLAGS	+= -L../qqsdk/lib/x86 -lgwsdk -ltxdevicesdk
OPENWRT_DIR	:= 
STAGING_DIR	:= 
CROSS_DIR	:= 
endif

SEDONA_HOME		:= /mnt/hgfs/sedona
SEDONABIN_DIR	:= $(SEDONA_HOME)/bin
JAVA_HOME		:= /home/au/all/program/jdk1.8.0_11
export PATH		:= $(PATH):$(CROSS_DIR):$(SEDONABIN_DIR)
export STAGING_DIR
export JAVA_HOME
export CLASSPATH:=.:$(JAVA_HOME)/lib/tools.jar:$(JAVA_HOME)/lib/dt.jar

# tools
CC			:= $(CROSS)gcc
CFLAGS 		:= -Wall -g -O2
CFLAGS		+= $(SELF_CFLAGS)
CFLAGS		+= $(SEDONA_CFLAGS)
CFLAGS		+= $(RPC_CFLAGS)
CFLAGS		+= $(CURL_CFLAGS)
CFLAGS		+= $(RABBITMQ_CFLAGS)
CFLAGS		+= $(JSONC_CFLAGS)
CFLAGS		+= $(DBUS_CFLAGS)
CFLAGS		+= $(DEPEND_CFLAGS)

CPP			:= $(CROSS)g++
CPPFLAGS	:= $(CFLAGS)
CPPFLAGS	+= $(CXX_CFLAGS)
CPPFLAGS	+= $(DBUSCXX_CFLAGS)

LD			:= $(CROSS)g++
LDFLAGS		:= -lm -lrt -ldl -lpthread
LDFLAGS		+= $(CXX_LDFLAGS)
LDFLAGS		+= $(DBUSCXX_LDFLAGS)
LDFLAGS		+= $(SEDONA_LDFLAGS)
LDFLAGS		+= $(RPC_LDFLAGS)
LDFLAGS		+= $(CURL_LDFLAGS)
LDFLAGS		+= $(RABBITMQ_LDFLAGS)
LDFLAGS		+= $(JSONC_LDFLAGS)
LDFLAGS		+= $(DBUS_LDFLAGS)
LDFLAGS		+= $(SELF_LDFLAGS)

CP			:= cp 
CPFLAGS		:= -rf
RM			:= rm
RMFLAGS		:= -rf

CPPEXT		:= .cpp
CEXT		:= .c
HEXT		:= .h
OEXT		:= .o
COMPILE		:= -c
OUT			:= -o


SEDONACC	:= sedonac.sh
SEDONAOUT	:= -outDir
SCODEEXT	:= .scode
SABEXT		:= .sab
XMLEXT		:= .xml
SAXEXT		:= .sax

STRIP		:= $(CROSS)strip
