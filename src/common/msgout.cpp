/////////////////////////////////////////////////////////////////////////////
// Name:        common/msgout.cpp
// Purpose:     wxMessageOutput implementation
// Author:      Mattia Barbon
// Modified by:
// Created:     17.07.02
// RCS-ID:      $Id$
// Copyright:   (c) the wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "msgout.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/ffile.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #if wxUSE_GUI
        #include "wx/msgdlg.h"
    #endif // wxUSE_GUI
#endif

#include "wx/msgout.h"
#include "wx/log.h"

#include <stdarg.h>
#include <stdio.h>

// ===========================================================================
// implementation
// ===========================================================================

wxMessageOutput* wxMessageOutput::ms_msgOut = 0;

wxMessageOutput* wxMessageOutput::Get()
{
    if ( !ms_msgOut && wxTheApp )
    {
        ms_msgOut = wxTheApp->CreateMessageOutput();
    }

    return ms_msgOut;
}

wxMessageOutput* wxMessageOutput::Set(wxMessageOutput* msgout)
{
    wxMessageOutput* old = ms_msgOut;
    ms_msgOut = msgout;
    return old;
}

// ----------------------------------------------------------------------------
// wxMessageOutputStderr
// ----------------------------------------------------------------------------

void wxMessageOutputStderr::Printf(const wxChar* format, ...)
{
    va_list args;
    va_start(args, format);
    wxString out;

    out.PrintfV(format, args);
    va_end(args);

    fprintf(stderr, "%s", (const char*) out.mb_str());
}

// ----------------------------------------------------------------------------
// wxMessageOutputMessageBox
// ----------------------------------------------------------------------------

#if wxUSE_GUI

void wxMessageOutputMessageBox::Printf(const wxChar* format, ...)
{
    va_list args;
    va_start(args, format);
    wxString out;

    out.PrintfV(format, args);
    va_end(args);

    // the native MSW msg box understands the TABs, others don't
#ifndef __WXMSW__
    out.Replace(wxT("\t"), wxT("        "));
#endif

    wxString title;
    if ( wxTheApp )
        title.Printf(_("%s message"), wxTheApp->GetAppName().c_str());

    ::wxMessageBox(out, title);
}

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// wxMessageOutputLog
// ----------------------------------------------------------------------------

void wxMessageOutputLog::Printf(const wxChar* format, ...)
{
    wxString out;

    va_list args;
    va_start(args, format);

    out.PrintfV(format, args);
    va_end(args);

    out.Replace(wxT("\t"), wxT("        "));

    ::wxLogMessage(wxT("%s"), out.c_str());
}
