///////////////////////////////////////////////////////////////////////////////
// Name:        palmos/statusbr.h
// Purpose:     Palm OS implementation of wxStatusBar
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_STATUSBR_H_
#define   _WX_STATUSBR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "statusbr.h"
#endif

#if wxUSE_NATIVE_STATUSBAR

class WXDLLEXPORT wxStatusBarPalm : public wxStatusBarBase
{
public:
    // ctors and such
    wxStatusBarPalm();
    wxStatusBarPalm(wxWindow *parent,
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

    virtual ~wxStatusBarPalm();

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
    
    void DrawStatusBar();

protected:
    void CopyFieldsWidth(const int widths[]);
    void SetFieldsWidth();

    // store the text in the status bar
    wxListString **StatusTextBuffer;
    void SetStatusBufferText(const wxString& text, int number);
    wxString GetStatusBufferText(int number);
    wxListString *GetOrCreateStatusBuffer(int i);
    wxListString *GetStatusBufferStack(int i) const;
    void DeleteStatusBuffer();    

    // override base class virtual
    void DoMoveWindow(int x, int y, int width, int height);

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBarPalm)
};

#endif  // wxUSE_NATIVE_STATUSBAR

#endif
