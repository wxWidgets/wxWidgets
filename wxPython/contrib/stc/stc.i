/////////////////////////////////////////////////////////////////////////////
// Name:        stc.i
// Purpose:     Wrappers for the wxStyledTextCtrl.
//
// Author:      Robin Dunn
//
// Created:     12-Oct-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"The `StyledTextCtrl` provides a text editor that can used as a syntax
highlighting source code editor, or similar.  Lexers for several programming
languages are built-in."
%enddef

%module(package="wx", docstring=DOCSTRING) stc


%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include <wx/stc/stc.h>

%}

//---------------------------------------------------------------------------

%import core.i
%import misc.i  // for DnD
//%import gdi.i   // for wxFontEncoding

%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

MAKE_CONST_WXSTRING(STCNameStr);


%include _stc_docstrings.i

enum wxFontEncoding;  // forward declare

MustHaveApp(wxStyledTextCtrl);

//---------------------------------------------------------------------------
// Get all our defs from the REAL header file.

%include stc.h

%extend wxStyleTextCtrl {
    %pythoncode {
        GetCaretLineBack = GetCaretLineBackground
        SetCaretLineBack = SetCaretLineBackground
    }
}

//---------------------------------------------------------------------------
// Python functions to act like the event macros

%pythoncode {
EVT_STC_CHANGE = wx.PyEventBinder( wxEVT_STC_CHANGE, 1 )
EVT_STC_STYLENEEDED = wx.PyEventBinder( wxEVT_STC_STYLENEEDED, 1 )
EVT_STC_CHARADDED = wx.PyEventBinder( wxEVT_STC_CHARADDED, 1 )
EVT_STC_SAVEPOINTREACHED = wx.PyEventBinder( wxEVT_STC_SAVEPOINTREACHED, 1 )
EVT_STC_SAVEPOINTLEFT = wx.PyEventBinder( wxEVT_STC_SAVEPOINTLEFT, 1 )
EVT_STC_ROMODIFYATTEMPT = wx.PyEventBinder( wxEVT_STC_ROMODIFYATTEMPT, 1 )
EVT_STC_KEY = wx.PyEventBinder( wxEVT_STC_KEY, 1 )
EVT_STC_DOUBLECLICK = wx.PyEventBinder( wxEVT_STC_DOUBLECLICK, 1 )
EVT_STC_UPDATEUI = wx.PyEventBinder( wxEVT_STC_UPDATEUI, 1 )
EVT_STC_MODIFIED = wx.PyEventBinder( wxEVT_STC_MODIFIED, 1 )
EVT_STC_MACRORECORD = wx.PyEventBinder( wxEVT_STC_MACRORECORD, 1 )
EVT_STC_MARGINCLICK = wx.PyEventBinder( wxEVT_STC_MARGINCLICK, 1 )
EVT_STC_NEEDSHOWN = wx.PyEventBinder( wxEVT_STC_NEEDSHOWN, 1 )
EVT_STC_PAINTED = wx.PyEventBinder( wxEVT_STC_PAINTED, 1 )
EVT_STC_USERLISTSELECTION = wx.PyEventBinder( wxEVT_STC_USERLISTSELECTION, 1 )
EVT_STC_URIDROPPED = wx.PyEventBinder( wxEVT_STC_URIDROPPED, 1 )
EVT_STC_DWELLSTART = wx.PyEventBinder( wxEVT_STC_DWELLSTART, 1 )
EVT_STC_DWELLEND = wx.PyEventBinder( wxEVT_STC_DWELLEND, 1 )
EVT_STC_START_DRAG = wx.PyEventBinder( wxEVT_STC_START_DRAG, 1 )
EVT_STC_DRAG_OVER = wx.PyEventBinder( wxEVT_STC_DRAG_OVER, 1 )
EVT_STC_DO_DROP = wx.PyEventBinder( wxEVT_STC_DO_DROP, 1 )
EVT_STC_ZOOM = wx.PyEventBinder( wxEVT_STC_ZOOM, 1 )
EVT_STC_HOTSPOT_CLICK = wx.PyEventBinder( wxEVT_STC_HOTSPOT_CLICK, 1 )
EVT_STC_HOTSPOT_DCLICK = wx.PyEventBinder( wxEVT_STC_HOTSPOT_DCLICK, 1 )
EVT_STC_CALLTIP_CLICK = wx.PyEventBinder( wxEVT_STC_CALLTIP_CLICK, 1 )
EVT_STC_AUTOCOMP_SELECTION = wx.PyEventBinder( wxEVT_STC_AUTOCOMP_SELECTION, 1 )
}

//---------------------------------------------------------------------------

%init %{
%}


//---------------------------------------------------------------------------

