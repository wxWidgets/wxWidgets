# this is the project file for the console wxWindows sample

# we generate the VC++ IDE project file, comment this line
# to generate a makefile for (n)make
TEMPLATE = vc6app

# wxbase is for wxWin console programs
CONFIG  = wxbase

WXCONFIGS = *

# project files
SOURCES = console.cpp
TARGET  = console
