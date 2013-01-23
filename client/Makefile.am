# Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
#
# This work is confidential and proprietary to Nexon Korea Corporation and
# must not be used, disclosed, copied, or distributed without the prior
# consent of Nexon Korea Corporation.

AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4

# Build 과정 중에 생성되서 이용되는 source file 들.
# E.g., protobuf 결과 파일들.
# 참고: BUILT_SOURCES 는 build 전에 먼저 생성된다.
BUILT_SOURCES = \
  $(nodist_pacman_SOURCES)


# 'make clean' 을 했을 때 기본적인 binrary/library 외에 추가로
# 지워져야 될 file 들.
CLEANFILES = $(BUILT_SOURCES)


# 'make install' 시 설치될 내용들
#   /usr/bin/<pkgName> 에 설치될 compiled program 들.
pkgbin_PROGRAMS = pacman
#   /usr/bin/<pkgName> 에 설치될 script 들
pkgbin_SCRIPTS =
#   /usr/share/<pkgName> 에 설치될 data file 들.
pkgdata_DATA =
#   /usr/include/<pkgName> 에 설치될 header file 들.
pkginclude_HEADERS =
#   /usr/lib/<pkgName> 에 설치될 library file 들.
pkglib_LTLIBRARIES =


################################################################################
# pacman
################################################################################

pacman_SOURCES = \
  src/pacman_app.cc \
  src/pacman_render.cc \
  src/pacman_network.cc \
  src/pacman.cc \
  src/pacman_util.cc \
  src/util/boost_util.cc \
  src/util/json_archive.cc \
  src/util/json_util.cc

nodist_pacman_SOURCES = \
  src/account_messages.pb.cc \
  src/account_messages.pb.h \
  src/app_messages_for_client.pb.cc \
  src/app_messages_for_client.pb.h \
  src/pacman_client_messages.pb.cc \
  src/pacman_client_messages.pb.h \
  src/pacman_server_messages.pb.cc \
  src/pacman_server_messages.pb.h

pacman_CPPFLAGS = \
  $(BOOST_CPPFLAGS) \
  $(FUNAPI_CPPFLAGS) \
  -I$(top_srcdir)/include \
  -I$(top_srcdir)/src \
  -I$(top_srcdir)/src/util \
  -I$(top_builddir)/include \
  -I$(top_builddir)/src

pacman_CXXFLAGS = \
  -Wall -g

pacman_LDFLAGS = \
  $(FUNAPI_LDFLAGS)

pacman_LDADD = \
  $(FUNAPI_LIBS) \
  -lboost_system \
  -lboost_thread \
  -ljson_spirit \
  -lncurses \
  -lprotobuf

################################################################################
# Built sources 를 위한 rules
################################################################################

src/account_messages.pb.cc \
src/account_messages.pb.h: \
src/account_messages.proto
	@$(MKDIR_P) $(top_builddir)/src
	$(PROTOC) \
	--cpp_out=$(top_builddir)/src \
	--proto_path=$(top_srcdir)/src \
	$<

src/app_messages_for_client.pb.cc \
src/app_messages_for_client.pb.h: \
src/app_messages_for_client.proto
	@$(MKDIR_P) $(top_builddir)/src
	$(PROTOC) \
	--cpp_out=$(top_builddir)/src \
	--proto_path=$(top_srcdir)/src \
	$<

src/pacman_client_messages.pb.cc \
src/pacman_client_messages.pb.h: \
src/pacman_client_messages.proto
	@$(MKDIR_P) $(top_builddir)/src
	$(PROTOC) \
	--cpp_out=$(top_builddir)/src \
	--proto_path=$(top_srcdir)/src \
	$<

src/pacman_server_messages.pb.cc \
src/pacman_server_messages.pb.h: \
src/pacman_server_messages.proto
	@$(MKDIR_P) $(top_builddir)/src
	$(PROTOC) \
	--cpp_out=$(top_builddir)/src \
	--proto_path=$(top_srcdir)/src \
	$<
