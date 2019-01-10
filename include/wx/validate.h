/////////////////////////////////////////////////////////////////////////////
// Name:        wx/validate.h
// Purpose:     wxValidator class
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VALIDATE_H_
#define _WX_VALIDATE_H_

#include "wx/defs.h"

#if wxUSE_VALIDATORS

#include "wx/event.h"

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxWindowBase;

// ----------------------------------------------------------------------------
// wxValidationStatusEvent
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxValidationStatusEvent : public wxCommandEvent
{
public:
    wxValidationStatusEvent() {}
    wxValidationStatusEvent(wxEventType type, wxWindow *win);

    void SetErrorMessage(const wxString& errormsg) { SetString(errormsg); }
    wxString GetErrorMessage() const { return GetString(); }

    // Return the window associated with the validator generating the event.
    wxWindow *GetWindow() const;

    // default copy ctor and dtor are ok

    virtual wxEvent *Clone() const wxOVERRIDE
      { return new wxValidationStatusEvent(*this); }

private:
    wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxValidationStatusEvent);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_VALIDATE_OK, wxValidationStatusEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_VALIDATE_ERROR, wxValidationStatusEvent);

typedef void (wxEvtHandler::*wxValidationStatusEventFunction)(wxValidationStatusEvent&);

#define wxValidationStatusEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxValidationStatusEventFunction, func)

#define EVT_VALIDATE_OK(id, fn) \
    wx__DECLARE_EVT1(wxEVT_VALIDATE_OK, id, wxValidationStatusEventHandler(fn))

#define EVT_VALIDATE_ERROR(id, fn) \
    wx__DECLARE_EVT1(wxEVT_VALIDATE_ERROR, id, wxValidationStatusEventHandler(fn))

/*
 A validator has up to three purposes:

 1) To validate the data in the window that's associated
    with the validator.
 2) To transfer data to and from the window.
 3) To filter input, using its role as a wxEvtHandler
    to intercept e.g. OnChar.

 Note that wxValidator and derived classes use reference counting.
*/

class WXDLLIMPEXP_CORE wxValidator : public wxEvtHandler
{
public:
    wxValidator();
    wxValidator(const wxValidator& other)
        : wxEvtHandler()
        , m_validatorWindow(other.m_validatorWindow)
    {
    }
    virtual ~wxValidator();

    // Make a clone of this validator (or return NULL) - currently necessary
    // if you're passing a reference to a validator.
    // Another possibility is to always pass a pointer to a new validator
    // (so the calling code can use a copy constructor of the relevant class).
    virtual wxObject *Clone() const
        { return NULL; }
    bool Copy(const wxValidator& val)
        { m_validatorWindow = val.m_validatorWindow; return true; }

    // Called when the value in the window must be validated.
    // This function can pop up an error message.
    virtual bool Validate(wxWindow *WXUNUSED(parent)) { return false; }

    // Called to transfer data to the window
    virtual bool TransferToWindow() { return false; }

    // Called to transfer data from the window
    virtual bool TransferFromWindow() { return false; }

    // Called when the validator is associated with a window, may be useful to
    // override if it needs to somehow initialize the window.
    virtual void SetWindow(wxWindow *win) { m_validatorWindow = win; }

    // accessors
    wxWindow *GetWindow() const { return m_validatorWindow; }

    // validators beep by default if invalid key is pressed, this function
    // allows to change this
    static void SuppressBellOnError(bool suppress = true)
        { ms_isSilent = suppress; }

    // test if beep is currently disabled
    static bool IsSilent() { return ms_isSilent; }

    // this function is deprecated because it handled its parameter
    // unnaturally: it disabled the bell when it was true, not false as could
    // be expected; use SuppressBellOnError() instead
#if WXWIN_COMPATIBILITY_2_8
    static wxDEPRECATED_INLINE(
        void SetBellOnError(bool doIt = true),
        ms_isSilent = doIt;
    )
#endif

protected:

    // Notice that the errormsg may be empty, in which case, the generated
    // event is sent to notify the event handler that the control has just
    // transitioned from invalid to valid state.
    void SendEvent(wxEventType type, const wxString& errormsg = wxString())
    {
        if ( !m_validatorWindow )
            return;

        wxValidationStatusEvent event(type, m_validatorWindow);
        event.SetErrorMessage(errormsg);

        DoProcessEvent(event);
    }

    // Derived classes use these functions to generate the appropriate event.

    void SendOkEvent() { SendEvent(wxEVT_VALIDATE_OK); }

    void SendErrorEvent(const wxString& errormsg)
    {
        wxASSERT_MSG(!errormsg.empty(), "Error message shouldn't be empty.");

        SendEvent(wxEVT_VALIDATE_ERROR, errormsg);
    }

    wxWindow *m_validatorWindow;

private:
    // Process the event. (might pop up error messages).
    void DoProcessEvent(wxValidationStatusEvent& event);

private:
    static bool ms_isSilent;

    wxDECLARE_DYNAMIC_CLASS(wxValidator);
    wxDECLARE_NO_ASSIGN_CLASS(wxValidator);
};

extern WXDLLIMPEXP_DATA_CORE(const wxValidator) wxDefaultValidator;

#define wxVALIDATOR_PARAM(val) val

#else // !wxUSE_VALIDATORS
    // wxWidgets is compiled without support for wxValidator, but we still
    // want to be able to pass wxDefaultValidator to the functions which take
    // a wxValidator parameter to avoid using "#if wxUSE_VALIDATORS"
    // everywhere
    class WXDLLIMPEXP_FWD_CORE wxValidator;
    static const wxValidator* const wxDefaultValidatorPtr = NULL;
    #define wxDefaultValidator (*wxDefaultValidatorPtr)

    // this macro allows to avoid warnings about unused parameters when
    // wxUSE_VALIDATORS == 0
    #define wxVALIDATOR_PARAM(val)
#endif // wxUSE_VALIDATORS/!wxUSE_VALIDATORS

#endif // _WX_VALIDATE_H_

