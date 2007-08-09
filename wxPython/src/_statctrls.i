/////////////////////////////////////////////////////////////////////////////
// Name:        _statctrls.i
// Purpose:     SWIG interface defs for wxStaticBox, wxStaticLine,
//              wxStaticText, wxStaticBitmap
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(StaticBitmapNameStr);
MAKE_CONST_WXSTRING(StaticBoxNameStr);
MAKE_CONST_WXSTRING(StaticTextNameStr);
MAKE_CONST_WXSTRING(StaticLineNameStr);

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxStaticBox);

class wxStaticBox : public wxControl {
public:
    %pythonAppend wxStaticBox         "self._setOORInfo(self)"
    %pythonAppend wxStaticBox()       ""
    %typemap(out) wxStaticBox*;    // turn off this typemap

    wxStaticBox(wxWindow* parent, wxWindowID id=-1,
                const wxString& label = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyStaticBoxNameStr);
    %RenameCtor(PreStaticBox, wxStaticBox());

    // Turn it back on again
    %typemap(out) wxStaticBox* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxWindow* parent, wxWindowID id=-1,
                const wxString& label = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyStaticBoxNameStr);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};


//---------------------------------------------------------------------------
%newgroup


MustHaveApp(wxStaticLine);

class wxStaticLine : public wxControl {
public:
    %pythonAppend wxStaticLine         "self._setOORInfo(self)"
    %pythonAppend wxStaticLine()       ""

    wxStaticLine( wxWindow *parent, wxWindowID id=-1,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString& name = wxPyStaticLineNameStr);
    %RenameCtor(PreStaticLine, wxStaticLine());

    bool Create( wxWindow *parent, wxWindowID id=-1,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString& name = wxPyStaticLineNameStr);

    // is the line vertical?
    bool IsVertical() const;

    // get the default size for the "lesser" dimension of the static line
    static int GetDefaultSize();

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};


//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxStaticText);

class wxStaticText : public wxControl {
public:
    %pythonAppend wxStaticText         "self._setOORInfo(self)"
    %pythonAppend wxStaticText()       ""

    wxStaticText(wxWindow* parent, wxWindowID id=-1,
                 const wxString& label = wxPyEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyStaticTextNameStr);
    %RenameCtor(PreStaticText, wxStaticText());

    bool Create(wxWindow* parent, wxWindowID id=-1,
                const wxString& label = wxPyEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyStaticTextNameStr);

    DocDeclStr(
        void , Wrap(int width),
        "This functions wraps the control's label so that each of its lines
becomes at most ``width`` pixels wide if possible (the lines are
broken at words boundaries so it might not be the case if words are
too long). If ``width`` is negative, no wrapping is done.", "");
    
    
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxStaticBitmap);

class wxStaticBitmap : public wxControl {
public:
    %pythonAppend wxStaticBitmap         "self._setOORInfo(self)"
    %pythonAppend wxStaticBitmap()       ""

    wxStaticBitmap(wxWindow* parent, wxWindowID id=-1,
                   const wxBitmap& bitmap = wxNullBitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxPyStaticBitmapNameStr);
    %RenameCtor(PreStaticBitmap, wxStaticBitmap());

    bool Create(wxWindow* parent, wxWindowID id=-1,
                   const wxBitmap& bitmap = wxNullBitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxPyStaticBitmapNameStr);

    wxBitmap GetBitmap();
    void SetBitmap(const wxBitmap& bitmap);
    void SetIcon(const wxIcon& icon);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
