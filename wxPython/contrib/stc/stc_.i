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

%module stc_


%{
#include "wxPython.h"
#include "wx/stc/stc.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern events.i
%extern controls.i


//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(STCNameStr);
%}
//----------------------------------------------------------------------
// Get all our defs from the REAL header file.

%include stc.h

//----------------------------------------------------------------------
// Python functions to act like the event macros

%pragma(python) code = "
def EVT_STC_CHANGE(win, id, func):
    win.Connect(id, -1, wxEVT_STC_CHANGE, func)

def EVT_STC_STYLENEEDED(win, id, func):
    win.Connect(id, -1, wxEVT_STC_STYLENEEDED, func)

def EVT_STC_CHARADDED(win, id, func):
    win.Connect(id, -1, wxEVT_STC_CHARADDED, func)

def EVT_STC_SAVEPOINTREACHED(win, id, func):
    win.Connect(id, -1, wxEVT_STC_SAVEPOINTREACHED, func)

def EVT_STC_SAVEPOINTLEFT(win, id, func):
    win.Connect(id, -1, wxEVT_STC_SAVEPOINTLEFT, func)

def EVT_STC_ROMODIFYATTEMPT(win, id, func):
    win.Connect(id, -1, wxEVT_STC_ROMODIFYATTEMPT, func)

def EVT_STC_KEY(win, id, func):
    win.Connect(id, -1, wxEVT_STC_KEY, func)

def EVT_STC_DOUBLECLICK(win, id, func):
    win.Connect(id, -1, wxEVT_STC_DOUBLECLICK, func)

def EVT_STC_UPDATEUI(win, id, func):
    win.Connect(id, -1, wxEVT_STC_UPDATEUI, func)

def EVT_STC_MODIFIED(win, id, func):
    win.Connect(id, -1, wxEVT_STC_MODIFIED, func)

def EVT_STC_MACRORECORD(win, id, func):
    win.Connect(id, -1, wxEVT_STC_MACRORECORD, func)

def EVT_STC_MARGINCLICK(win, id, func):
    win.Connect(id, -1, wxEVT_STC_MARGINCLICK, func)

def EVT_STC_NEEDSHOWN(win, id, func):
    win.Connect(id, -1, wxEVT_STC_NEEDSHOWN, func)

def EVT_STC_POSCHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_STC_POSCHANGED, func)

def EVT_STC_PAINTED(win, id, func):
    win.Connect(id, -1, wxEVT_STC_PAINTED, func)

def EVT_STC_USERLISTSELECTION(win, id, func):
    win.Connect(id, -1, wxEVT_STC_USERLISTSELECTION, func)

def EVT_STC_URIDROPPED(win, id, func):
    win.Connect(id, -1, wxEVT_STC_URIDROPPED, func)

def EVT_STC_DWELLSTART(win, id, func):
    win.Connect(id, -1, wxEVT_STC_DWELLSTART, func)

def EVT_STC_DWELLEND(win, id, func):
    win.Connect(id, -1, wxEVT_STC_DWELLEND, func)

def EVT_STC_START_DRAG(win, id, func):
    win.Connect(id, -1, wxEVT_STC_START_DRAG, func)

def EVT_STC_DRAG_OVER(win, id, func):
    win.Connect(id, -1, wxEVT_STC_DRAG_OVER, func)

def EVT_STC_DO_DROP(win, id, func):
    win.Connect(id, -1, wxEVT_STC_DO_DROP, func)

def EVT_STC_ZOOM(win, id, func):
    win.Connect(id, -1, wxEVT_STC_ZOOM, func)

"

//----------------------------------------------------------------------

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

%}


//----------------------------------------------------------------------

%pragma(python) include="_stcextras.py";

//----------------------------------------------------------------------
//----------------------------------------------------------------------


