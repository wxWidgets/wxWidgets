/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/combobox.h
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#ifdef __GNUG__
    #pragma interface "combobox.h"
#endif

#include "wx/choice.h"

#if wxUSE_COMBOBOX

WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

// ----------------------------------------------------------------------------
// Combobox control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxComboBox: public wxChoice
{
    DECLARE_DYNAMIC_CLASS(wxComboBox)

public:
    wxComboBox() { }

    wxComboBox(wxWindow *parent, wxWindowID id,
            const wxString& value = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxComboBoxNameStr)
    {
        Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    // List functions: see wxChoice

    // Text field functions
    wxString GetValue() const { return GetLabel(); }
    virtual void SetValue(const wxString& value);

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(int n) { wxChoice::SetSelection(n); }
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);

    // implementation only from now on
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    bool MSWProcessEditMsg(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

    WXHWND GetEditHWND() const;

protected:
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual wxSize DoGetBestSize() const;
};

#endif // wxUSE_COMBOBOX
#endif
    // _WX_COMBOBOX_H_
