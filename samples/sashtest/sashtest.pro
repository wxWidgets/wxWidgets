# this is the project file for the sashtest wxWindows sample

# we generate the VC++ IDE project file, comment this line
# to generate a makefile for (n)make
TEMPLATE = vc6app

# wx is mandatory for wxWindows projects
CONFIG  = wx

# the configurations of wxWindows we want to use: the value below is the
# default one; possible other values are {Debug|Release}Unicode[Dll]
#WXCONFIGS = Debug Release DebugDll ReleaseDll

# project files
SOURCES = sashtest.cpp
RC_FILE = sashtest.rc
TARGET  = sashtest
