/////////////////////////////////////////////////////////////////////////////
// Name:        misc2.i
// Purpose:     Definitions of miscelaneous functions and classes that need
//              to know about wxWindow.  (So they can't be in misc.i or an
//              import loop will happen.)
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module misc2

%{
#include "helpers.h"
#include <wx/resource.h>
#include <wx/tooltip.h>
#include <wx/caret.h>
#ifdef NOT_READY_YET
#include <wx/fontenum.h>
#endif
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import windows.i
%import misc.i

//----------------------------------------------------------------------

wxWindow * wxFindWindowByLabel(const wxString& label, wxWindow *parent=NULL);
wxWindow * wxFindWindowByName(const wxString& name, wxWindow *parent=NULL);



//---------------------------------------------------------------------------
// wxToolTip

class wxToolTip {
public:
    wxToolTip(const wxString &tip);

    void SetTip(const wxString& tip);
    wxString GetTip();
    // *** Not in the "public" interface void SetWindow(wxWindow *win);
    wxWindow *GetWindow();
};


%inline %{
    void wxToolTip_Enable(bool flag) {
        wxToolTip::Enable(flag);
    }

    void wxToolTip_SetDelay(long milliseconds) {
        wxToolTip::SetDelay(milliseconds);
    }
%}

//----------------------------------------------------------------------

class wxCaret {
public:
    wxCaret(wxWindow* window, const wxSize& size);
    ~wxCaret();

    bool IsOk();
    bool IsVisible();
    %name(GetPositionTuple)void GetPosition(int *OUTPUT, int *OUTPUT);
    wxPoint GetPosition();
    %name(GetSizeTuple)void GetSize(int *OUTPUT, int *OUTPUT);
    wxSize GetSize();
    wxWindow *GetWindow();
    %name(MoveXY)void Move(int x, int y);
    void Move(const wxPoint& pt);
    void Show(int show = TRUE);
    void Hide();
    void OnSetFocus();
    void OnKillFocus();
};

%inline %{
    int wxCaret_GetBlinkTime() {
        return wxCaret::GetBlinkTime();
    }

    void wxCaret_SetBlinkTime(int milliseconds) {
        wxCaret::SetBlinkTime(milliseconds);
    }
%}

//----------------------------------------------------------------------
#ifdef NOT_READY_YET

%{
class wxPyFontEnumerator : public wxFontEnumerator {
public:
    wxPyFontEnumerator() {}
    ~wxPyFontEnumerator() {}

    bool EnumerateFamilies(int fixedWidthOnly = FALSE);
    bool EnumerateEncodings(const char* family = "");

    DEC_PYCALLBACK_BOOL_STRING(OnFontFamily);
    DEC_PYCALLBACK_BOOL_STRINGSTRING(OnFontEncoding);

    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_STRING(wxPyFontEnumerator, wxFontEnumerator, OnFontFamily);
IMP_PYCALLBACK_BOOL_STRINGSTRING(wxPyFontEnumerator, wxFontEnumerator, OnFontEncoding);

%}

%name(wxFontEnumerator) class wxPyFontEnumerator {
public:
    wxPyFontEnumerator() {}
    ~wxPyFontEnumerator() {}
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    bool EnumerateFamilies(int fixedWidthOnly = FALSE);
    bool EnumerateEncodings(const char* family = "");

    bool base_OnFontFamily(const wxString& family);
    bool base_OnFontEncoding(const wxString& family,
                             const wxString& encoding);
};
#endif
//----------------------------------------------------------------------
//----------------------------------------------------------------------

