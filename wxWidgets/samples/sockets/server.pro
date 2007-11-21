# this is the project file for the server part of wxWindows sockets sample

# we generate the VC++ IDE project file, comment this line
# to generate a makefile for (n)make
TEMPLATE = vc6app

# wx is mandatory for wxWindows projects
CONFIG  = wx

# project files
SOURCES = server.cpp
RC_FILE = server.rc
TARGET  = socketserver
