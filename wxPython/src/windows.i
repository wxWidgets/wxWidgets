/////////////////////////////////////////////////////////////////////////////
// Name:        windows.i
// Purpose:     SWIG definitions of various window classes
//
// Author:      Robin Dunn
//
// Created:     24-June-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module(package="wx") _windows

//---------------------------------------------------------------------------

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
    
%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = _core }

%include __windows_rename.i


MAKE_CONST_WXSTRING_NOSWIG(EmptyString);
MAKE_CONST_WXSTRING_NOSWIG(PanelNameStr);


// Include all the files that make up this module
%include _panel.i
%include _toplvl.i
%include _statusbar.i
%include _splitter.i
%include _sashwin.i
%include _popupwin.i
%include _tipwin.i
%include _vscroll.i
%include _taskbar.i
%include _cmndlgs.i
%include _mdi.i
%include _pywindows.i
%include _printfw.i

//---------------------------------------------------------------------------
