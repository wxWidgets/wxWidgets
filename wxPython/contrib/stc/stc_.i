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
#include "export.h"
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
// Get all our defs from the REAL header file.

%include stc.h

//----------------------------------------------------------------------
// Python functions to act like the event macros

%pragma(python) code = "
def EVT_STC_CHANGE(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_CHANGE, fn)

def EVT_STC_STYLENEEDED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_STYLENEEDED, fn)

def EVT_STC_CHARADDED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_CHARADDED, fn)

def EVT_STC_UPDATEUI(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_UPDATEUI, fn)

def EVT_STC_SAVEPOINTREACHED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_SAVEPOINTREACHED, fn)

def EVT_STC_SAVEPOINTLEFT(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_SAVEPOINTLEFT, fn)

def EVT_STC_ROMODIFYATTEMPT(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_ROMODIFYATTEMPT, fn)

def EVT_STC_DOUBLECLICK(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_DOUBLECLICK, fn)

def EVT_STC_MODIFIED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MODIFIED, fn)

def EVT_STC_KEY(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_KEY, fn)

def EVT_STC_MACRORECORD(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MACRORECORD, fn)

def EVT_STC_MARGINCLICK(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_MARGINCLICK, fn)

def EVT_STC_NEEDSHOWN(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_NEEDSHOWN, fn)

def EVT_STC_POSCHANGED(win, id, fn):
    win.Connect(id, -1, wxEVT_STC_POSCHANGED, fn)


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


