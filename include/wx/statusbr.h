/////////////////////////////////////////////////////////////////////////////
// Name:        wx/statusbr.h
// Purpose:     wxStatusBar class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.02.00
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATUSBR_H_BASE_
#define _WX_STATUSBR_H_BASE_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "statbar.h"
#endif

#include "wx/window.h"

#if wxUSE_STATUSBAR

#include "wx/list.h"

WX_DECLARE_LIST(wxString, wxListString);

// ----------------------------------------------------------------------------
// wxStatusBar: a window near the bottom of the frame used for status info
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStatusBarBase : public wxWindow
{
public:
    wxStatusBarBase();

    virtual ~wxStatusBarBase();

    // field count
    // -----------

    // set the number of fields and call SetStatusWidths(widths) if widths are
    // given
    virtual void SetFieldsCount(int number = 1, const int *widths = NULL);
    int GetFieldsCount() const { return m_nFields; }

    // field text
    // ----------

    virtual void SetStatusText(const wxString& text, int number = 0) = 0;
    virtual wxString GetStatusText(int number = 0) const = 0;

    void PushStatusText(const wxString& text, int number = 0);
    void PopStatusText(int number = 0);

    // fields widths
    // -------------

    // set status field widths as absolute numbers: positive widths mean that
    // the field has the specified absolute width, negative widths are
    // interpreted as the sizer options, i.e. the extra space (total space
    // minus the sum of fixed width fields) is divided between the fields with
    // negative width according to the abs value of the width (field with width
    // -2 grows twice as much as one with width -1 &c)
    virtual void SetStatusWidths(int n, const int widths[]);

    // geometry
    // --------

    // Get the position and size of the field's internal bounding rectangle
    virtual bool GetFieldRect(int i, wxRect& rect) const = 0;

    // sets the minimal vertical size of the status bar
    virtual void SetMinHeight(int height) = 0;

    // get the dimensions of the horizontal and vertical borders
    virtual int GetBorderX() const = 0;
    virtual int GetBorderY() const = 0;

    // don't want status bars to accept the focus at all
    virtual bool AcceptsFocus() const { return FALSE; }

protected:
    // set the widths array to NULL
    void InitWidths();

    // free the status widths arrays
    void FreeWidths();

    // reset the widths
    void ReinitWidths() { FreeWidths(); InitWidths(); }

    // same, for text stacks
    void InitStacks();
    void FreeStacks();
    void ReinitStacks() { FreeStacks(); InitStacks(); }

    // calculate the real field widths for the given total available size
    wxArrayInt CalculateAbsWidths(wxCoord widthTotal) const;

    // use these functions to access the stacks of field strings
    wxListString *GetStatusStack(int i) const;
    wxListString *GetOrCreateStatusStack(int i);

    // the current number of fields
    int        m_nFields;

    // the widths of the fields in pixels if !NULL, all fields have the same
    // width otherwise
    int       *m_statusWidths;

    // stacks of previous values for PushStatusText/PopStatusText
    // this is created on demand, use GetStatusStack/GetOrCreateStatusStack
    wxListString **m_statusTextStacks;
};

// ----------------------------------------------------------------------------
// include the actual wxStatusBar class declaration
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #define wxStatusBarUniv wxStatusBar
    #define sm_classwxStatusBarUniv sm_classwxStatusBar

    #include "wx/univ/statusbr.h"
#elif defined(__WIN32__) && wxUSE_NATIVE_STATUSBAR
    #define wxStatusBar95 wxStatusBar
    #define sm_classwxStatusBar95 sm_classwxStatusBar

    #include "wx/msw/statbr95.h"
#elif defined(__WXMAC__)
    #define wxStatusBarMac wxStatusBar
    #define sm_classwxStatusBarMac sm_classwxStatusBar

    #include "wx/generic/statusbr.h"
    #include "wx/mac/statusbr.h"
#else
    #define wxStatusBarGeneric wxStatusBar
    #define sm_classwxStatusBarGeneric sm_classwxStatusBar

    #include "wx/generic/statusbr.h"
#endif

#endif // wxUSE_STATUSBAR

#endif
    // _WX_STATUSBR_H_BASE_
