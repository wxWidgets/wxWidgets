# this is the project file for the minimal wxWindows sample

# we generate the VC++ IDE project file, comment this line
# to generate a makefile for (n)make
TEMPLATE = vc6app

# wx is mandatory for wxWindows projects
CONFIG  = wx

# project files
SOURCES = minimal.cpp
RC_FILE = minimal.rc
TARGET  = minimal
