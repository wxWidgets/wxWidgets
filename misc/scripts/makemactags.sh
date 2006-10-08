#!/bin/sh
ctags --totals --language-force=c++ -I WXDLLEXPORT \
	include/wx/*.h \
	include/wx/generic/*.h \
	include/wx/html/*.h \
	include/wx/mac/*.h \
	include/wx/mac/carbon/*.h \
	include/wx/mac/corefoundation/*.h \
	include/wx/mac/private/*.h \
	include/wx/protocol/*.h \
	include/wx/xml/*.h \
	include/wx/xrc/*.h \
	include/wx/unix/*.h \
	src/common/*.cpp \
	src/generic/*.cpp \
	src/html/*.cpp \
	src/mac/carbon/*.cpp \
	src/mac/corefoundation/*.cpp \
	src/unix/*.cpp \
	src/xml/*.cpp \
	src/xrc/*.cpp

