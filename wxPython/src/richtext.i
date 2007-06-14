/////////////////////////////////////////////////////////////////////////////
// Name:        richtext.i
// Purpose:     Classes for wxRichTextCtrl and support classes
//
// Author:      Robin Dunn
//
// Created:     11-April-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
""
%enddef

%module(package="wx", docstring=DOCSTRING) richtext

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/printfw.h"
#include "wx/wxPython/twoitem.h"

#include <wx/richtext/richtextctrl.h>


class wxEffects;
class wxBufferedDC;
class wxBufferedPaintDC;
class wxMetaFile;
class wxMetaFileDC;
class wxPrinterDC;

%}

#define USE_TEXTATTREX 0

//----------------------------------------------------------------------

%import windows.i

%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

MAKE_CONST_WXSTRING_NOSWIG(EmptyString);

//----------------------------------------------------------------------

// TODO:  These are already defined in _textctrl.i, do we really need them here?
enum wxTextAttrAlignment
{
    wxTEXT_ALIGNMENT_DEFAULT,
    wxTEXT_ALIGNMENT_LEFT,
    wxTEXT_ALIGNMENT_CENTRE,
    wxTEXT_ALIGNMENT_CENTER = wxTEXT_ALIGNMENT_CENTRE,
    wxTEXT_ALIGNMENT_RIGHT,
    wxTEXT_ALIGNMENT_JUSTIFIED
};

//----------------------------------------------------------------------

%include _richtextbuffer.i
%include _richtextctrl.i
%include _richtexthtml.i
%include _richtextxml.i

//----------------------------------------------------------------------

%init %{
    wxRichTextModuleInit();
%}

//----------------------------------------------------------------------
