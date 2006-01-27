/////////////////////////////////////////////////////////////////////////////
// Name:        common/dlgcmn.cpp
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "dialogbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/dialog.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/stattext.h"
    #include "wx/sizer.h"
    #include "wx/button.h"
    #include "wx/containr.h"
#endif

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

// FIXME - temporary hack in absence of wxTopLevelWindow, should be always used
#ifdef wxTopLevelWindowNative
BEGIN_EVENT_TABLE(wxDialogBase, wxTopLevelWindow)
    WX_EVENT_TABLE_CONTROL_CONTAINER(wxDialogBase)
END_EVENT_TABLE()

WX_DELEGATE_TO_CONTROL_CONTAINER(wxDialogBase)
#endif

void wxDialogBase::Init()
{
    m_returnCode = 0;
    m_affirmativeId = wxID_OK;
    
    // the dialogs have this flag on by default to prevent the events from the
    // dialog controls from reaching the parent frame which is usually
    // undesirable and can lead to unexpected and hard to find bugs
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);

#ifdef wxTopLevelWindowNative // FIXME - temporary hack, should be always used!
    m_container.SetContainerWindow(this);
#endif
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

#if wxUSE_BUTTON

wxSizer *wxDialogBase::CreateButtonSizer( long flags )
{
#ifdef __SMARTPHONE__
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
#else
    return CreateStdDialogButtonSizer( flags );
#endif
}

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
