///////////////////////////////////////////////////////////////////////////////
// Name:        wx/radiobox.h
// Purpose:     wxRadioBox declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_BASE_
#define _WX_RADIOBOX_H_BASE_

#if wxUSE_RADIOBOX

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxRadioBoxNameStr;

// ----------------------------------------------------------------------------
// wxRadioBoxBase is not a base class at all, but rather a mix-in because the
// real wxRadioBox derives from different classes on different platforms: for
// example, it is a 
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRadioBoxBase
{
public:
    // selection
    virtual void SetSelection(int n) = 0;
    virtual int GetSelection() const = 0;

    virtual wxString GetStringSelection() const
    {
        wxString s;
        int sel = GetSelection();
        if ( sel != wxNOT_FOUND )
            s = GetString(sel);

        return s;
    }

    virtual bool SetStringSelection(const wxString& s)
    {
        int sel = FindString(s);
        if ( sel != wxNOT_FOUND )
        {
            SetSelection(sel);

            return TRUE;
        }

        return FALSE;
    }

    // string access
    virtual int GetCount() const = 0;
    virtual int FindString(const wxString& s) const
    {
        int count = GetCount();
        for ( int n = 0; n < count; n++ )
        {
            if ( GetString(n) == s )
                return n;
        }

        return wxNOT_FOUND;
    }

    virtual wxString GetString(int n) const = 0;
    virtual void SetString(int n, const wxString& label) = 0;

    // change the individual radio button state
    virtual void Enable(int n, bool enable = TRUE) = 0;
    virtual void Show(int n, bool show = TRUE) = 0;

    // for compatibility only, don't use
    int Number() const { return GetCount(); }
    wxString GetLabel(int n) const { return GetString(n); }
    void SetLabel(int n, const wxString& label) { SetString(n, label); }
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/radiobox.h"
#elif defined(__WXMSW__)
    #include "wx/msw/radiobox.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/radiobox.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/radiobox.h"
#elif defined(__WXQT__)
    #include "wx/qt/radiobox.h"
#elif defined(__WXMAC__)
    #include "wx/mac/radiobox.h"
#elif defined(__WXPM__)
    #include "wx/os2/radiobox.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/radiobox.h"
#endif

#endif // wxUSE_RADIOBOX

#endif
    // _WX_RADIOBOX_H_BASE_
