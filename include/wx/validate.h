/////////////////////////////////////////////////////////////////////////////
// Name:        validate.h
// Purpose:     wxValidator class
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALIDATEH__
#define _WX_VALIDATEH__

#ifdef __GNUG__
    #pragma interface "validate.h"
#endif

#include "wx/event.h"

class WXDLLEXPORT wxWindow;

/*
 A validator has up to three purposes:

 1) To validate the data in the window that's associated
    with the validator.
 2) To transfer data to and from the window.
 3) To filter input, using its role as a wxEvtHandler
    to intercept e.g. OnChar.

 Note that wxValidator and derived classes use reference counting.
*/

class WXDLLEXPORT wxValidator : public wxEvtHandler
{
public:
    wxValidator();
    ~wxValidator();

    // Make a clone of this validator (or return NULL) - currently necessary
    // if you're passing a reference to a validator.
    // Another possibility is to always pass a pointer to a new validator
    // (so the calling code can use a copy constructor of the relevant class).
    virtual wxObject *Clone() const
        { return (wxValidator *)NULL; }
    bool Copy(const wxValidator& val)
        { m_validatorWindow = val.m_validatorWindow; return TRUE; }

    // Called when the value in the window must be validated.
    // This function can pop up an error message.
    virtual bool Validate(wxWindow *WXUNUSED(parent)) { return FALSE; };

    // Called to transfer data to the window
    virtual bool TransferToWindow() { return FALSE; }

    // Called to transfer data from the window
    virtual bool TransferFromWindow() { return FALSE; };

    // accessors
    wxWindow *GetWindow() const { return m_validatorWindow; }
    void SetWindow(wxWindow *win) { m_validatorWindow = win; }

    // validators beep by default if invalid key is pressed, these functions
    // allow to change it
    static bool IsSilent() { return ms_isSilent; }
    static void SetBellOnError(bool doIt = TRUE) { ms_isSilent = doIt; }

protected:
    wxWindow *m_validatorWindow;

private:
    static bool ms_isSilent;

    DECLARE_DYNAMIC_CLASS(wxValidator)
};

WXDLLEXPORT_DATA(extern const wxValidator) wxDefaultValidator;

#endif
    // _WX_VALIDATEH__
