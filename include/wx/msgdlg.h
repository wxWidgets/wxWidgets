/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msgdlgg.h
// Purpose:     common header and base class for wxMessageDialog
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGDLG_H_BASE_
#define _WX_MSGDLG_H_BASE_

#include "wx/defs.h"

#if wxUSE_MSGDLG

#include "wx/dialog.h"

WXDLLIMPEXP_DATA_CORE(extern const char) wxMessageBoxCaptionStr[];

class WXDLLIMPEXP_CORE wxMessageDialogBase : public wxDialog
{
public:
    // ctors
    wxMessageDialogBase() { m_dialogStyle = 0; }
    wxMessageDialogBase(wxWindow *parent,
                        const wxString& message,
                        const wxString& caption,
                        long style)
        : m_message(message),
          m_caption(caption)
    {
        m_parent = parent;
        SetMessageDialogStyle(style);
    }

    // virtual dtor for the base class
    virtual ~wxMessageDialogBase() { }


    // methods for setting up more custom message dialogs -- all functions
    // return false if they're not implemented
    virtual bool SetYesNoLabels(const wxString& WXUNUSED(yes),
                                const wxString& WXUNUSED(no))
    {
        return false;
    }

    virtual bool SetYesNoCancelLabels(const wxString& WXUNUSED(yes),
                                      const wxString& WXUNUSED(no),
                                      const wxString& WXUNUSED(cancel))
    {
        return false;
    }

    virtual bool SetOKLabel(const wxString& WXUNUSED(ok))
    {
        return false;
    }

    virtual bool SetOKCancelLabels(const wxString& WXUNUSED(ok),
                                   const wxString& WXUNUSED(cancel))
    {
        return false;
    }

    virtual void SetMessage(const wxString& message)
    {
        m_message = message;
    }

    virtual void SetExtendedMessage(const wxString& extendedMessage)
    {
        m_extendedMessage = extendedMessage;
    }

protected:
    // common validation of wxMessageDialog style
    void SetMessageDialogStyle(long style)
    {
        wxASSERT_MSG( ((style & wxYES_NO) == wxYES_NO) || ((style & wxYES_NO) == 0),
                      _T("wxYES and wxNO may only be used together in wxMessageDialog") );

        wxASSERT_MSG( (style & wxID_OK) != wxID_OK,
                      _T("wxMessageBox: Did you mean wxOK (and not wxID_OK)?") );

        m_dialogStyle = style;
    }

    long GetMessageDialogStyle() const { return m_dialogStyle; }


    // for the platforms not supporting separate main and extended messages
    // this function should be used to combine both of them in a single string
    wxString GetFullMessage() const
    {
        wxString msg = m_message;
        if ( !m_extendedMessage.empty() )
            msg << "\n\n" << m_extendedMessage;

        return msg;
    }

    wxString m_message,
             m_extendedMessage,
             m_caption;
    long m_dialogStyle;
};

#if defined(__WX_COMPILING_MSGDLGG_CPP__) || \
    defined(__WXUNIVERSAL__) || defined(__WXGPE__) || \
    (defined(__WXGTK__) && !defined(__WXGTK20__))
    #include "wx/generic/msgdlgg.h"

    #define wxMessageDialog wxGenericMessageDialog
#elif defined(__WXCOCOA__)
	#include "wx/cocoa/msgdlg.h"
#elif defined(__WXPALMOS__)
    #include "wx/palmos/msgdlg.h"
#elif defined(__WXMSW__)
    #include "wx/msw/msgdlg.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/msgdlg.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/msgdlg.h"
#elif defined(__WXMAC__)
    #include "wx/osx/msgdlg.h"
#elif defined(__WXPM__)
    #include "wx/os2/msgdlg.h"
#endif

// ----------------------------------------------------------------------------
// wxMessageBox: the simplest way to use wxMessageDialog
// ----------------------------------------------------------------------------

int WXDLLIMPEXP_CORE wxMessageBox(const wxString& message,
                             const wxString& caption = wxMessageBoxCaptionStr,
                             long style = wxOK | wxCENTRE,
                             wxWindow *parent = NULL,
                             int x = wxDefaultCoord, int y = wxDefaultCoord);

#endif // wxUSE_MSGDLG

#endif // _WX_MSGDLG_H_BASE_
