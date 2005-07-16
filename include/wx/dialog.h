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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "dialogbase.h"
#endif

#include "wx/defs.h"
#include "wx/containr.h"
#include "wx/toplevel.h"

class WXDLLEXPORT wxSizer;
class WXDLLEXPORT wxStdDialogButtonSizer;

#define wxDIALOG_NO_PARENT      0x0001  // Don't make owned by apps top window

#ifdef __WXWINCE__
#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxMAXIMIZE | wxCLOSE_BOX | wxNO_BORDER)
#else
#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#endif

extern WXDLLEXPORT_DATA(const wxChar*) wxDialogNameStr;

class WXDLLEXPORT wxDialogBase : public wxTopLevelWindow
{
public:
    wxDialogBase() { Init(); }
    virtual ~wxDialogBase() { }

    void Init();

    // Modal dialogs have a return code - usually the id of the last
    // pressed button
    void SetReturnCode(int returnCode) { m_returnCode = returnCode; }
    int GetReturnCode() const { return m_returnCode; }

    // The identifier for the affirmative button
    void SetAffirmativeId(int affirmativeId) { m_affirmativeId = affirmativeId; }
    int GetAffirmativeId() const { return m_affirmativeId; }

    // Identifier for Esc key translation
#if wxCHECK_VERSION(2, 7, 0)
    #error "Uncomment SetEscapeId() implementation"

    // this is what we should do in 2.7: remove the "#else" part and add
    // m_escapeId declaration and the docs for Set/GetEscapeId()
    void SetEscapeId(int escapeId) { m_escapeId = escapeId; }
    int GetEscapeId() const { return m_escapeId; }
#elif wxABI_VERSION > 20601
    // just a stub for 2.6
    int GetEscapeId() const { return wxID_ANY; }
#endif

#if wxUSE_STATTEXT // && wxUSE_TEXTCTRL
    // splits text up at newlines and places the
    // lines into a vertical wxBoxSizer
    wxSizer *CreateTextSizer( const wxString &message );
#endif // wxUSE_STATTEXT // && wxUSE_TEXTCTRL

#if wxUSE_BUTTON
    // places buttons into a horizontal wxBoxSizer
    wxSizer *CreateButtonSizer( long flags );
    wxStdDialogButtonSizer *CreateStdDialogButtonSizer( long flags );
#endif // wxUSE_BUTTON

protected:
    // The return code from modal dialog
    int m_returnCode;

    // The identifier for the affirmative button (usually wxID_OK)
    int m_affirmativeId;

    DECLARE_NO_COPY_CLASS(wxDialogBase)
    DECLARE_EVENT_TABLE()
    WX_DECLARE_CONTROL_CONTAINER();
};


#if defined(__WXUNIVERSAL__) && !defined(__WXMICROWIN__)
    #include "wx/univ/dialog.h"
#else
    #if defined(__WXPALMOS__)
        #include "wx/palmos/dialog.h"
    #elif defined(__WXMSW__)
        #include "wx/msw/dialog.h"
    #elif defined(__WXMOTIF__)
        #include "wx/motif/dialog.h"
    #elif defined(__WXGTK__)
        #include "wx/gtk/dialog.h"
    #elif defined(__WXMAC__)
        #include "wx/mac/dialog.h"
    #elif defined(__WXCOCOA__)
        #include "wx/cocoa/dialog.h"
    #elif defined(__WXPM__)
        #include "wx/os2/dialog.h"
    #endif
#endif

#endif
    // _WX_DIALOG_H_BASE_
