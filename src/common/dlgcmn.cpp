/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dlgcmn.cpp
// Purpose:     common (to all ports) wxDialog functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.06.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dialog.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/stattext.h"
    #include "wx/sizer.h"
    #include "wx/containr.h"
#endif

#include "wx/statline.h"
#include "wx/sysopt.h"

#if wxUSE_STATTEXT

// ----------------------------------------------------------------------------
// wxTextWrapper
// ----------------------------------------------------------------------------

// this class is used to wrap the text on word boundary: wrapping is done by
// calling OnStartLine() and OnOutputLine() functions
class wxTextWrapper
{
public:
    wxTextWrapper() { m_eol = false; }

    // win is used for getting the font, text is the text to wrap, width is the
    // max line width or -1 to disable wrapping
    void Wrap(wxWindow *win, const wxString& text, int widthMax);

    // we don't need it, but just to avoid compiler warnings
    virtual ~wxTextWrapper() { }

protected:
    // line may be empty
    virtual void OnOutputLine(const wxString& line) = 0;

    // called at the start of every new line (except the very first one)
    virtual void OnNewLine() { }

private:
    // call OnOutputLine() and set m_eol to true
    void DoOutputLine(const wxString& line)
    {
        OnOutputLine(line);

        m_eol = true;
    }

    // this function is a destructive inspector: when it returns true it also
    // resets the flag to false so calling it again woulnd't return true any
    // more
    bool IsStartOfNewLine()
    {
        if ( !m_eol )
            return false;

        m_eol = false;

        return true;
    }


    bool m_eol;
};

#endif // wxUSE_STATTEXT

// ----------------------------------------------------------------------------
// wxDialogBase
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDialogBase, wxTopLevelWindow)
    EVT_BUTTON(wxID_OK, wxDialogBase::OnAffirmativeButton)
    EVT_BUTTON(wxID_APPLY, wxDialogBase::OnApply)
    EVT_BUTTON(wxID_CANCEL, wxDialogBase::OnCancelButton)

    EVT_CLOSE(wxDialogBase::OnCloseWindow)

    EVT_CHAR_HOOK(wxDialogBase::OnCharHook)

    WX_EVENT_TABLE_CONTROL_CONTAINER(wxDialogBase)
END_EVENT_TABLE()

WX_DELEGATE_TO_CONTROL_CONTAINER(wxDialogBase, wxTopLevelWindow)

void wxDialogBase::Init()
{
    m_returnCode = 0;
    m_affirmativeId = wxID_OK;
    m_escapeId = wxID_ANY;

    // the dialogs have this flag on by default to prevent the events from the
    // dialog controls from reaching the parent frame which is usually
    // undesirable and can lead to unexpected and hard to find bugs
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);

    m_container.SetContainerWindow(this);
}

#if wxUSE_STATTEXT

void wxTextWrapper::Wrap(wxWindow *win, const wxString& text, int widthMax)
{
    const wxChar *lastSpace = NULL;
    wxString line;

    const wxChar *lineStart = text.c_str();
    for ( const wxChar *p = lineStart; ; p++ )
    {
        if ( IsStartOfNewLine() )
        {
            OnNewLine();

            lastSpace = NULL;
            line.clear();
            lineStart = p;
        }

        if ( *p == _T('\n') || *p == _T('\0') )
        {
            DoOutputLine(line);

            if ( *p == _T('\0') )
                break;
        }
        else // not EOL
        {
            if ( *p == _T(' ') )
                lastSpace = p;

            line += *p;

            if ( widthMax >= 0 && lastSpace )
            {
                int width;
                win->GetTextExtent(line, &width, NULL);

                if ( width > widthMax )
                {
                    // remove the last word from this line
                    line.erase(lastSpace - lineStart, p + 1 - lineStart);
                    DoOutputLine(line);

                    // go back to the last word of this line which we didn't
                    // output yet
                    p = lastSpace;
                }
            }
            //else: no wrapping at all or impossible to wrap
        }
    }
}

class wxTextSizerWrapper : public wxTextWrapper
{
public:
    wxTextSizerWrapper(wxWindow *win)
    {
        m_win = win;
        m_hLine = 0;
    }

    wxSizer *CreateSizer(const wxString& text, int widthMax)
    {
        m_sizer = new wxBoxSizer(wxVERTICAL);
        Wrap(m_win, text, widthMax);
        return m_sizer;
    }

protected:
    virtual void OnOutputLine(const wxString& line)
    {
        if ( !line.empty() )
        {
            m_sizer->Add(new wxStaticText(m_win, wxID_ANY, line));
        }
        else // empty line, no need to create a control for it
        {
            if ( !m_hLine )
                m_hLine = m_win->GetCharHeight();

            m_sizer->Add(5, m_hLine);
        }
    }

private:
    wxWindow *m_win;
    wxSizer *m_sizer;
    int m_hLine;
};

wxSizer *wxDialogBase::CreateTextSizer(const wxString& message)
{
    // I admit that this is complete bogus, but it makes
    // message boxes work for pda screens temporarily..
    int widthMax = -1;
    const bool is_pda = wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA;
    if (is_pda)
    {
        widthMax = wxSystemSettings::GetMetric( wxSYS_SCREEN_X ) - 25;
    }

    // '&' is used as accel mnemonic prefix in the wxWidgets controls but in
    // the static messages created by CreateTextSizer() (used by wxMessageBox,
    // for example), we don't want this special meaning, so we need to quote it
    wxString text(message);
    text.Replace(_T("&"), _T("&&"));

    wxTextSizerWrapper wrapper(this);

    return wrapper.CreateSizer(text, widthMax);
}

class wxLabelWrapper : public wxTextWrapper
{
public:
    void WrapLabel(wxWindow *text, int widthMax)
    {
        m_text.clear();
        Wrap(text, text->GetLabel(), widthMax);
        text->SetLabel(m_text);
    }

protected:
    virtual void OnOutputLine(const wxString& line)
    {
        m_text += line;
    }

    virtual void OnNewLine()
    {
        m_text += _T('\n');
    }

private:
    wxString m_text;
};

// NB: don't "factor out" the scope operator, SGI MIPSpro 7.3 (but not 7.4)
//     gets confused if it doesn't immediately follow the class name
void
#if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
wxStaticText::
#else
wxStaticTextBase::
#endif
Wrap(int width)
{
    wxLabelWrapper wrapper;
    wrapper.WrapLabel(this, width);
}

#endif // wxUSE_STATTEXT

wxSizer *wxDialogBase::CreateButtonSizer( long flags, bool separated, wxCoord distance )
{
#ifdef __SMARTPHONE__
    wxUnusedVar(separated);
    wxUnusedVar(distance);

    wxDialog* dialog = (wxDialog*) this;
    if (flags & wxOK){
        dialog->SetLeftMenu(wxID_OK);
    }

    if (flags & wxCANCEL){
        dialog->SetRightMenu(wxID_CANCEL);
    }

    if (flags & wxYES){
        dialog->SetLeftMenu(wxID_YES);
    }

    if (flags & wxNO){
        dialog->SetLeftMenu(wxID_NO);
    }
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    return sizer;

#else // !__SMARTPHONE__

#ifdef __POCKETPC__
    // PocketPC guidelines recommend for Ok/Cancel dialogs to use
    // OK button located inside caption bar and implement Cancel functionality
    // through Undo outside dialog. As native behaviour this will be default
    // here but can be easily replaced with real wxButtons
    // with "wince.dialog.real-ok-cancel" option set to 1
    if ( ((flags & ~(wxCANCEL|wxNO_DEFAULT))== wxOK) &&
         (wxSystemOptions::GetOptionInt(wxT("wince.dialog.real-ok-cancel"))==0)
       )
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        return sizer;
    }
#endif // __POCKETPC__

#if wxUSE_BUTTON

    wxSizer* buttonSizer = CreateStdDialogButtonSizer( flags );

    // Mac Human Interface Guidelines recommend not to use static lines as grouping elements
#if wxUSE_STATLINE && !defined(__WXMAC__)
    if(!separated)
        return buttonSizer;

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    topsizer->Add( new wxStaticLine( this, wxID_ANY ), 0, wxEXPAND | wxBOTTOM, distance );
    topsizer->Add( buttonSizer, 0, wxEXPAND );
    return topsizer;

#else // !wxUSE_STATLINE

    wxUnusedVar(separated);
    wxUnusedVar(distance);
    return buttonSizer;

#endif // wxUSE_STATLINE/!wxUSE_STATLINE

#else // !wxUSE_BUTTON

    wxUnusedVar(separated);
    wxUnusedVar(distance);
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    return sizer;

#endif // wxUSE_BUTTON/!wxUSE_BUTTON

#endif // __SMARTPHONE__/!__SMARTPHONE__
}

#if wxUSE_BUTTON

wxStdDialogButtonSizer *wxDialogBase::CreateStdDialogButtonSizer( long flags )
{
    wxStdDialogButtonSizer *sizer = new wxStdDialogButtonSizer();

    wxButton *ok = NULL;
    wxButton *yes = NULL;
    wxButton *no = NULL;

    if (flags & wxOK){
        ok = new wxButton(this, wxID_OK);
        sizer->AddButton(ok);
    }

    if (flags & wxCANCEL){
        wxButton *cancel = new wxButton(this, wxID_CANCEL);
        sizer->AddButton(cancel);
    }

    if (flags & wxYES){
        yes = new wxButton(this, wxID_YES);
        sizer->AddButton(yes);
    }

    if (flags & wxNO){
        no = new wxButton(this, wxID_NO);
        sizer->AddButton(no);
    }

    if (flags & wxHELP){
        wxButton *help = new wxButton(this, wxID_HELP);
        sizer->AddButton(help);
    }

    if (flags & wxNO_DEFAULT)
    {
        if (no)
        {
            no->SetDefault();
            no->SetFocus();
        }
    }
    else
    {
        if (ok)
        {
            ok->SetDefault();
            ok->SetFocus();
        }
        else if (yes)
        {
            yes->SetDefault();
            yes->SetFocus();
        }
    }

    if (flags & wxOK)
        SetAffirmativeId(wxID_OK);
    else if (flags & wxYES)
        SetAffirmativeId(wxID_YES);

    sizer->Realize();

    return sizer;
}

#endif // wxUSE_BUTTON

// ----------------------------------------------------------------------------
// escape/affirmatives button handling
// ----------------------------------------------------------------------------

void wxDialogBase::AcceptAndClose()
{
    if ( Validate() && TransferDataFromWindow() )
    {
        EndDialog(wxID_OK);
    }
}

void wxDialogBase::SetAffirmativeId(int affirmativeId)
{
    if ( affirmativeId == m_affirmativeId )
        return;

    // disconnect the handler for the old affirmative button
    if ( m_affirmativeId != wxID_NONE && m_affirmativeId != wxID_OK )
    {
        if ( !Disconnect
              (
                m_affirmativeId,
                wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(wxDialogBase::OnAffirmativeButton)
              ) )
        {
            wxFAIL_MSG( _T("failed to disconnect old ok handler") );
        }
    }
    //else: wxID_OK is always handled

    // connect the handler to the new button
    if ( affirmativeId != wxID_NONE )
    {
        Connect(m_affirmativeId,
                wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(wxDialogBase::OnAffirmativeButton));
    }
    //else: no affirmative button

    m_affirmativeId = affirmativeId;
}

void wxDialogBase::SetEscapeId(int escapeId)
{
    if ( escapeId == m_escapeId )
        return;

    if ( m_escapeId != wxID_ANY &&
            m_escapeId != wxID_CANCEL &&
                m_escapeId != wxID_ANY )
    {
        if ( !Disconnect
              (
                m_escapeId,
                wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(wxDialogBase::OnCancelButton)
              ) )
        {
            wxFAIL_MSG( _T("failed to disconnect old cancel handler") );
        }
    }
    //else: wxID_CANCEL is always handled

    // connect the handler to the new button
    if ( escapeId != wxID_NONE )
    {
        Connect(m_escapeId,
                wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(wxDialogBase::OnCancelButton));
    }

    m_escapeId = escapeId;
}

// ----------------------------------------------------------------------------
// event handling stuff
// ----------------------------------------------------------------------------

bool wxDialogBase::EmulateButtonClickIfPresent(int id)
{
    wxButton *btn = wxDynamicCast(FindWindow(id), wxButton);

    if ( !btn || !btn->IsEnabled() || !btn->IsShown() )
        return false;

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, id);
    event.SetEventObject(btn);
    btn->GetEventHandler()->ProcessEvent(event);

    return true;
}

void wxDialogBase::EndDialog(int rc)
{
    if ( IsModal() )
        EndModal(rc);
    else
        Hide();
}

bool wxDialogBase::IsEscapeKey(const wxKeyEvent& event)
{
    // for most platforms, Esc key is used to close the dialogs
    return event.GetKeyCode() == WXK_ESCAPE &&
                event.GetModifiers() == wxMOD_NONE;
}

void wxDialogBase::OnCharHook(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == WXK_ESCAPE )
    {
        int idCancel = GetEscapeId();
        switch ( idCancel )
        {
            case wxID_NONE:
                // don't handle Esc specially at all
                break;

            case wxID_ANY:
                // this value is special: it means translate Esc to wxID_CANCEL
                // but if there is no such button, then fall back to wxID_OK
                if ( EmulateButtonClickIfPresent(wxID_CANCEL) )
                    return;
                idCancel = wxID_OK;
                // fall through

            default:
                // translate Esc to button press for the button with given id
                if ( EmulateButtonClickIfPresent(idCancel) )
                    return;
        }
    }

    event.Skip();
}

void wxDialogBase::OnAffirmativeButton(wxCommandEvent& WXUNUSED(event))
{
    AcceptAndClose();
}

void wxDialogBase::OnApply(wxCommandEvent& WXUNUSED(event))
{
    if ( Validate() )
        TransferDataFromWindow();

    // TODO probably need to disable the Apply button until things change again
}

void wxDialogBase::OnCancelButton(wxCommandEvent& WXUNUSED(event))
{
    EndDialog(wxID_CANCEL);
}

void wxDialogBase::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    // We'll send a Cancel message by default, which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().

    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close(). We wouldn't want to destroy the dialog by default, since
    // the dialog may have been created on the stack. However, this does mean
    // that calling dialog->Close() won't delete the dialog unless the handler
    // for wxID_CANCEL does so. So use Destroy() if you want to be sure to
    // destroy the dialog. The default OnCancel (above) simply ends a modal
    // dialog, and hides a modeless dialog.

    // VZ: this is horrible and MT-unsafe. Can't we reuse some of these global
    //     lists here? don't dare to change it now, but should be done later!
    static wxList closing;

    if ( closing.Member(this) )
        return;

    closing.Append(this);

    wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    cancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(cancelEvent); // This may close the dialog

    closing.DeleteObject(this);
}

void wxDialogBase::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
  SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
  Refresh();
}
