///////////////////////////////////////////////////////////////////////////////
// Name:        msw/statbr95.h
// Purpose:     native implementation of wxStatusBar
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _STATBR95_H
#define   _STATBR95_H

#ifdef __GNUG__
#pragma interface "statbr95.h"
#endif

#if wxUSE_NATIVE_STATUSBAR

class WXDLLEXPORT wxStatusBar95 : public wxStatusBarBase
{
public:
    // ctors and such
    wxStatusBar95();
    wxStatusBar95(wxWindow *parent,
                  wxWindowID id = -1,
                  long style = wxST_SIZEGRIP,
                  const wxString& name = wxEmptyString)
    {
        (void)Create(parent, id, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                long style = wxST_SIZEGRIP,
                const wxString& name = wxEmptyString);

    virtual ~wxStatusBar95();

    // a status line can have several (<256) fields numbered from 0
    virtual void SetFieldsCount(int number = 1, const int *widths = NULL);

    // each field of status line has it's own text
    virtual void     SetStatusText(const wxString& text, int number = 0);
    virtual wxString GetStatusText(int number = 0) const;

    // set status line fields' widths
    virtual void SetStatusWidths(int n, const int widths_field[]);

    // sets the minimal vertical size of the status bar
    virtual void SetMinHeight(int height);

    // get the position and size of the field's internal bounding rectangle
    virtual bool GetFieldRect(int i, wxRect& rect) const;

    // get the border size
    virtual int GetBorderX() const;
    virtual int GetBorderY() const;

    void OnSize(wxSizeEvent& event);

protected:
    void CopyFieldsWidth(const int widths[]);
    void SetFieldsWidth();

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxStatusBar95);
};

#endif  // wxUSE_NATIVE_STATUSBAR

#endif
