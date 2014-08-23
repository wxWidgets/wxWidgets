#########################################################################
#
#           Open Watcom 1.5 EXPAT XML Parser Main Makefile
#
# Open Watcom 1.5 and newer multi-platform makefile for EXPAT XML Parser.
# Currently will build targets for OS/2-ECS, NT/Win2000/XP, and Linux.
# This makefile will call, based on comand line target, watmake.mif
# to build target release or debug.
#
# Author        : Michael Greene, July 2006
#
# User selectable project options are set in buildopts.inc
#
#########################################################################


all: .SYMBOLIC
  @echo ***********************************************
  @echo Valid options:
  @echo   "wmake <platform build>"
  @echo      os2       - OS/2-ECS Release
  @echo      nt        - NT/Win2000/XP
  @echo      linux     - Linux Release
  @echo      os2d      - OS/2-ECS Debug
  @echo      ntd       - NT/Win2000/XP Debug
  @echo      linuxd    - Linux Debug
  @echo      full      - all release
  @echo      fulld     - all debug
  @echo      fullall   - all release and debug
  @echo      cleanall  - remove all
  @echo      clean     - remove build keep release
  @echo      xmlts.zip - download test suite and unzip

full: os2 nt linux .SYMBOLIC

fulld: os2d ntd linuxd .SYMBOLIC

fullall: os2 nt linux os2d ntd linuxd .SYMBOLIC

os2: .SYMBOLIC
  @%write  watopts.tmp SYSPLATFORM = os2
  @%make buildproc

os2d: .SYMBOLIC
  @%write  watopts.tmp SYSPLATFORM = os2
  @%append watopts.tmp DEBUG = 1
  @%make buildproc

nt:  .SYMBOLIC
  @%write  watopts.tmp SYSPLATFORM = nt
  @%make buildproc

ntd:  .SYMBOLIC
  @%write  watopts.tmp SYSPLATFORM = nt
  @%append watopts.tmp DEBUG = 1
  @%make buildproc

linux: .SYMBOLIC
  @%write  watopts.tmp SYSPLATFORM = linux
  @%make buildproc

linuxd: .SYMBOLIC
  @%write  watopts.tmp SYSPLATFORM = linux
  @%append watopts.tmp DEBUG = 1
  @%make buildproc

clean: .SYMBOLIC
  @%write  watopts.tmp CLEAN = 1
  @wmake -h -f watmake.mif clean

cleanall: .SYMBOLIC
  @%write  watopts.tmp CLEAN = 1
  @wmake -h -f watmake.mif cleanall

xmlts.zip: .SYMBOLIC
  @%write  watopts.tmp DUMMY = 1
  @wmake -h -f watmake.mif xmlts.zip

buildproc: .procedure
!ifdef DEBUG
  @%append  watopts.tmp DEBUG   = 1
!endif
  @wmake -a -h -f watmake.mif


