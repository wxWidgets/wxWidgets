#
#  Copyright (C) 1995 by Ke Jin <kejin@empress.com>
#

#============ Default for all system ==============
SHELL   =
SHELL   = /bin/sh
DLDAPI  = DLDAPI_SVR4_DLFCN
DLSUFFIX= so
OUTFILE = iodbc
OBJX    =

#============= Linux ELF =========================
#      Slackware 2.x,(kernel 1.3.x) on i386
#      Red Hat   2.x (kernel 1.2.x) on i486
#
ANSI    = -ansi
CC      = gcc
PIC     = -fPIC
LDFLAGS = -shared
LIBS    = -ldl
