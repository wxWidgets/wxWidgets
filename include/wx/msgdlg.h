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

    DECLARE_NO_COPY_CLASS(wxMessageDialogBase)
};

// this is a helper class for native wxMessageDialog implementations which need
// to store the custom button labels as member variables and then use them in
// ShowModal() (there could conceivably be a port which would have some native
// functions for setting these labels immediately and we also don't need to
// store them at all if custom labels are not supported, which is why we do
// this in a separate class and not wxMessageDialogBase itself)
#if defined(__WXCOCOA__) || defined(__WXMAC__) || defined(__WXMSW__)

class WXDLLIMPEXP_CORE wxMessageDialogWithCustomLabels
    : public wxMessageDialogBase
{
public:
    // ctors
    wxMessageDialogWithCustomLabels() { }
    wxMessageDialogWithCustomLabels(wxWindow *parent,
                                    const wxString& message,
                                    const wxString& caption,
                                    long style)
        : wxMessageDialogBase(parent, message, caption, style)
    {
    }

    // customization of the message box buttons
    virtual bool SetYesNoLabels(const wxString& yes,const wxString& no)
    {
        DoSetCustomLabel(m_yes, yes);
        DoSetCustomLabel(m_no, no);
        return true;
    }

    virtual bool SetYesNoCancelLabels(const wxString& yes,
                                      const wxString& no,
                                      const wxString& cancel)
    {
        DoSetCustomLabel(m_yes, yes);
        DoSetCustomLabel(m_no, no);
        DoSetCustomLabel(m_cancel, cancel);
        return true;
    }

    virtual bool SetOKLabel(const wxString& ok)
    {
        DoSetCustomLabel(m_ok, ok);
        return true;
    }

    virtual bool SetOKCancelLabels(const wxString& ok, const wxString& cancel)
    {
        DoSetCustomLabel(m_ok, ok);
        DoSetCustomLabel(m_cancel, cancel);
        return true;
    }

protected:
    // test if any custom labels were set
    bool HasCustomLabels() const
    {
        return !(m_ok.empty() && m_cancel.empty() &&
                 m_yes.empty() && m_no.empty());
    }

    // these functions return the label to be used for the button which is
    // either a custom label explicitly set by the user or the default label,
    // i.e. they always return a valid string
    wxString GetYesLabel() const { return m_yes.empty() ? _("Yes") : m_yes; }
    wxString GetNoLabel() const { return m_no.empty() ? _("No") : m_no; }
    wxString GetOKLabel() const { return m_ok.empty() ? _("OK") : m_ok; }
    wxString GetCancelLabel() const
        { return m_cancel.empty() ? _("Cancel") : m_cancel; }

private:
    // this function is called by our public SetXXXLabels() and should assign
    // the value to var with possibly some transformation (e.g. Cocoa version
    // currently uses this to remove any accelerators from the button strings)
    virtual void DoSetCustomLabel(wxString& var, const wxString& value)
    {
        var = value;
    }

    // labels for the buttons, initially empty meaning that the defaults should
    // be used, use GetYes/No/OK/CancelLabel() to access them
    wxString m_yes,
             m_no,
             m_ok,
             m_cancel;

    DECLARE_NO_COPY_CLASS(wxMessageDialogWithCustomLabels)
};

#endif // ports needing wxMessageDialogWithCustomLabels

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
