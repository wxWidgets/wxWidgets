/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dialog.h
// Purpose:     wxDialogBase class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.06.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_BASE_
#define _WX_DIALOG_H_BASE_

#include "wx/defs.h"
#include "wx/panel.h"

class WXDLLEXPORT wxDialogBase : public wxPanel
{
public:
    // the modal dialogs have a return code - usually the id of the last
    // pressed button
    void SetReturnCode(int returnCode) { m_returnCode = returnCode; }
    int GetReturnCode() const { return m_returnCode; }

protected:
    // functions to help with dialog layout
    // ------------------------------------

    // constants used in dialog layout
    static const long LAYOUT_X_MARGIN;
    static const long LAYOUT_Y_MARGIN;
    static const long MARGIN_BETWEEN_BUTTONS;

    // Split the message in lines putting them into the array and calculating
    // the maximum line width/height which is returned as wxSize.
    wxSize SplitTextMessage(const wxString& message, wxArrayString *lines);

    // Creates the (possibly multiline) message, assuming each line has the
    // size sizeText (which can be retrieved from SplitTextMessage). Returns
    // the bottom border of the multiline text zone.
    long CreateTextMessage(const wxArrayString& lines,
                           const wxPoint& posText,
                           const wxSize& sizeText);

    // Returns the preferred size for the buttons in the dialog
    wxSize GetStandardButtonSize(bool hasCancel = TRUE);

    // Create the standard [Ok] and [Cancel] (if hasCancel) buttons centering
    // them with respect to the dialog width wDialog at vertical position y.
    // wButton and hButton is the size of the button (which can be retrieved
    // from GetStandardButtonSize)
    void CreateStandardButtons(long wDialog,
                               long y,
                               long wButton,
                               long hButton,
                               bool hasCancel = TRUE);

    // Returns the standard height of single line text ctrl (it's not the same
    // as the height of just text which may be retrieved from
    // wxGetCharHeight())
    long GetStandardTextHeight();

    // the return code from modal dialog
    int m_returnCode;
};

#if defined(__WXMSW__)
    #include "wx/msw/dialog.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/dialog.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/dialog.h"
#elif defined(__WXQT__)
    #include "wx/qt/dialog.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dialog.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/dialog.h"
#endif

#endif
    // _WX_DIALOG_H_BASE_
