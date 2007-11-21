# this is the project file for the widgets wxWindows sample

# we generate the VC++ IDE project file, comment this line
# to generate a makefile for (n)make
TEMPLATE = vc6app

# wx is mandatory for wxWindows projects
CONFIG  = wx

# the configurations of wxWindows we want to use: the value below is the
# default one; possible other values are {Debug|Release}Unicode[Dll]
#WXCONFIGS = Debug Release DebugDll ReleaseDll

# project files
SOURCES = widgets.cpp button.cpp combobox.cpp gauge.cpp listbox.cpp \
          notebook.cpp radiobox.cpp slider.cpp spinbtn.cpp static.cpp \
          textctrl.cpp
RC_FILE = widgets.rc
TARGET  = widgets
