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

%{
#include <wx/statline.h>

    DECLARE_DEF_STRING(StaticBitmapNameStr);
    DECLARE_DEF_STRING(StaticBoxNameStr);
    DECLARE_DEF_STRING(StaticTextNameStr);
%}

//---------------------------------------------------------------------------
%newgroup

class wxStaticBox : public wxControl {
public:
    %addtofunc wxStaticBox         "self._setOORInfo(self)"
    %addtofunc wxStaticBox()       ""

    wxStaticBox(wxWindow* parent, wxWindowID id, const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyStaticBoxNameStr);
    %name(PreStaticBox)wxStaticBox();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyStaticBoxNameStr);
};


//---------------------------------------------------------------------------
%newgroup


class wxStaticLine : public wxControl {
public:
    %addtofunc wxStaticLine         "self._setOORInfo(self)"
    %addtofunc wxStaticLine()       ""

    wxStaticLine( wxWindow *parent, wxWindowID id,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString& name = wxPyStaticTextNameStr);
    %name(PreStaticLine)wxStaticLine();

    bool Create( wxWindow *parent, wxWindowID id,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString& name = wxPyStaticTextNameStr);

    // is the line vertical?
    bool IsVertical() const;

    // get the default size for the "lesser" dimension of the static line
    static int GetDefaultSize() { return 2; }

};


//---------------------------------------------------------------------------
%newgroup

class wxStaticText : public wxControl {
public:
    %addtofunc wxStaticText         "self._setOORInfo(self)"
    %addtofunc wxStaticText()       ""

    wxStaticText(wxWindow* parent, wxWindowID id, const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyStaticTextNameStr);
    %name(PreStaticText)wxStaticText();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyStaticTextNameStr);
};

//---------------------------------------------------------------------------
%newgroup

class wxStaticBitmap : public wxControl {
public:
    %addtofunc wxStaticText         "self._setOORInfo(self)"
    %addtofunc wxStaticText()       ""

    wxStaticBitmap(wxWindow* parent, wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxPyStaticBitmapNameStr);
    %name(PreStaticBitmap)wxStaticBitmap();

    bool Create(wxWindow* parent, wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxPyStaticBitmapNameStr);

    wxBitmap GetBitmap();
    void SetBitmap(const wxBitmap& bitmap);
    void SetIcon(const wxIcon& icon);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
