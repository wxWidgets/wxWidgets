///////////////////////////////////////////////////////////////////////////////
// Name:        common/textcmn.cpp
// Purpose:     implementation of platform-independent functions of wxTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     13.07.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "textctrlbase.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TEXTCTRL

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/textctrl.h"
#endif // WX_PRECOMP

#include "wx/ffile.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// we don't have any objects of type wxTextCtrlBase in the program, only
// wxTextCtrl, so this cast is safe
#define TEXTCTRL(ptr)   ((wxTextCtrl *)(ptr))

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxTextUrlEvent, wxCommandEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TEXT_UPDATED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TEXT_ENTER)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TEXT_URL)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TEXT_MAXLEN)

// ----------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------

wxTextCtrlBase::wxTextCtrlBase()
{
}

wxTextCtrlBase::~wxTextCtrlBase()
{
}

// ----------------------------------------------------------------------------
// style functions - not implemented here
// ----------------------------------------------------------------------------

// apply styling to text range
bool wxTextCtrlBase::SetStyle(long WXUNUSED(start), long WXUNUSED(end),
                              const wxTextAttr& WXUNUSED(style))
{
    // to be implemented in derived TextCtrl classes
    return FALSE;
}

// change default text attributes
bool wxTextCtrlBase::SetDefaultStyle(const wxTextAttr &style)
{
    m_defaultStyle = style;
    return TRUE;
}

// get default text attributes
const wxTextAttr& wxTextCtrlBase::GetDefaultStyle() const
{
    return m_defaultStyle;
}

// ----------------------------------------------------------------------------
// file IO functions
// ----------------------------------------------------------------------------

bool wxTextCtrlBase::LoadFile(const wxString& filename)
{
#if wxUSE_FFILE
    wxFFile file(filename);
    if ( file.IsOpened() )
    {
        wxString text;
        if ( file.ReadAll(&text) )
        {
            SetValue(text);

            DiscardEdits();

            m_filename = filename;

            return TRUE;
        }
    }

    wxLogError(_("File couldn't be loaded."));
#endif // wxUSE_FFILE

    return FALSE;
}

bool wxTextCtrlBase::SaveFile(const wxString& filename)
{
    wxString filenameToUse = filename.IsEmpty() ? m_filename : filename;
    if ( !filenameToUse )
    {
        // what kind of message to give? is it an error or a program bug?
        wxLogDebug(wxT("Can't save textctrl to file without filename."));

        return FALSE;
    }

#if wxUSE_FFILE
    wxFFile file(filename, "w");
    if ( file.IsOpened() && file.Write(GetValue()) )
    {
        // it's not modified any longer
        DiscardEdits();

        m_filename = filename;

        return TRUE;
    }

    wxLogError(_("The text couldn't be saved."));
#endif // wxUSE_FFILE

    return FALSE;
}

// ----------------------------------------------------------------------------
// stream-like insertion operator
// ----------------------------------------------------------------------------

wxTextCtrl& wxTextCtrlBase::operator<<(const wxString& s)
{
    AppendText(s);
    return *TEXTCTRL(this);
}

wxTextCtrl& wxTextCtrlBase::operator<<(float f)
{
    wxString str;
    str.Printf(wxT("%.2f"), f);
    AppendText(str);
    return *TEXTCTRL(this);
}

wxTextCtrl& wxTextCtrlBase::operator<<(double d)
{
    wxString str;
    str.Printf(wxT("%.2f"), d);
    AppendText(str);
    return *TEXTCTRL(this);
}

wxTextCtrl& wxTextCtrlBase::operator<<(int i)
{
    wxString str;
    str.Printf(wxT("%d"), i);
    AppendText(str);
    return *TEXTCTRL(this);
}

wxTextCtrl& wxTextCtrlBase::operator<<(long i)
{
    wxString str;
    str.Printf(wxT("%ld"), i);
    AppendText(str);
    return *TEXTCTRL(this);
}

wxTextCtrl& wxTextCtrlBase::operator<<(const wxChar c)
{
    return operator<<(wxString(c));
}

// ----------------------------------------------------------------------------
// streambuf methods implementation
// ----------------------------------------------------------------------------

#ifndef NO_TEXT_WINDOW_STREAM

int wxTextCtrlBase::overflow(int c)
{
    AppendText((wxChar)c);

    // return something different from EOF
    return 0;
}

#endif // NO_TEXT_WINDOW_STREAM

// ----------------------------------------------------------------------------
// clipboard stuff
// ----------------------------------------------------------------------------

bool wxTextCtrlBase::CanCopy() const
{
    // can copy if there's a selection
    long from, to;
    GetSelection(&from, &to);
    return from != to;
}

bool wxTextCtrlBase::CanCut() const
{
    // can cut if there's a selection and if we're not read only
    return CanCopy() && IsEditable();
}

bool wxTextCtrlBase::CanPaste() const
{
    // can paste if we are not read only
    return IsEditable();
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void wxTextCtrlBase::SelectAll()
{
    SetSelection(0, GetLastPosition());
}

wxString wxTextCtrlBase::GetSelection() const
{
    long from, to;
    GetSelection(&from, &to);

    wxString sel;
    if ( from < to )
    {
        sel = GetValue().Mid(from, to - from);
    }

    return sel;
}

#else // !wxUSE_TEXTCTRL

// define this one even if !wxUSE_TEXTCTRL because it is also used by other
// controls (wxComboBox and wxSpinCtrl)
#include "wx/event.h"

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TEXT_UPDATED)

#endif // wxUSE_TEXTCTRL/!wxUSE_TEXTCTRL

