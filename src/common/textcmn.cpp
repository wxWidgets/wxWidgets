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

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

// ----------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------

wxTextCtrlBase::wxTextCtrlBase()
{
#ifndef NO_TEXT_WINDOW_STREAM
    if (allocate())
        setp(base(),ebuf());
#endif // NO_TEXT_WINDOW_STREAM
}

// ----------------------------------------------------------------------------
// file IO functions
// ----------------------------------------------------------------------------

bool wxTextCtrlBase::LoadFile(const wxString& filename)
{
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

    return FALSE;
}

bool wxTextCtrlBase::SaveFile(const wxString& filename)
{
    wxString filenameToUse = filename.IsEmpty() ? m_filename : filename;
    if ( !filenameToUse )
    {
        // what kind of message to give? is it an error or a program bug?
        wxLogDebug(_T("Can't save textctrl to file without filename."));

        return FALSE;
    }

    wxFFile file(filename, "w");
    if ( file.IsOpened() && file.Write(GetValue()) )
    {
        // it's not modified any longer
        DiscardEdits();

        m_filename = filename;

        return TRUE;
    }

    wxLogError(_("The text couldn't be saved."));

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
    str.Printf(_T("%.2f"), f);
    AppendText(str);
    return *TEXTCTRL(this);
}

wxTextCtrl& wxTextCtrlBase::operator<<(double d)
{
    wxString str;
    str.Printf(_T("%.2f"), d);
    AppendText(str);
    return *TEXTCTRL(this);
}

wxTextCtrl& wxTextCtrlBase::operator<<(int i)
{
    wxString str;
    str.Printf(_T("%d"), i);
    AppendText(str);
    return *TEXTCTRL(this);
}

wxTextCtrl& wxTextCtrlBase::operator<<(long i)
{
    wxString str;
    str.Printf(_T("%ld"), i);
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

int wxTextCtrlBase::overflow( int WXUNUSED(c) )
{
    int len = pptr() - pbase();
    char *txt = new char[len+1];
    strncpy(txt, pbase(), len);
    txt[len] = '\0';
    (*this) << txt;
    setp(pbase(), epptr());
    delete[] txt;
    return EOF;
}

int wxTextCtrlBase::sync()
{
    int len = pptr() - pbase();
    char *txt = new char[len+1];
    strncpy(txt, pbase(), len);
    txt[len] = '\0';
    (*this) << txt;
    setp(pbase(), epptr());
    delete[] txt;
    return 0;
}

int wxTextCtrlBase::underflow()
{
    return EOF;
}

#endif // NO_TEXT_WINDOW_STREAM

