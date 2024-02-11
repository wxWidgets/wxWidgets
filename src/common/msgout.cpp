/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/msgout.cpp
// Purpose:     wxMessageOutput implementation
// Author:      Mattia Barbon
// Created:     17.07.02
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/ffile.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #if wxUSE_GUI
        #include "wx/msgdlg.h"
    #endif // wxUSE_GUI
#endif

#include "wx/msgout.h"
#include "wx/apptrait.h"
#include <stdarg.h>
#include <stdio.h>

#if defined(__WINDOWS__)
    #include "wx/msw/private.h"
#endif

#if defined(__ANDROID__)
    #include <android/log.h>
#endif

// ===========================================================================
// implementation
// ===========================================================================

#if wxUSE_BASE

// ----------------------------------------------------------------------------
// wxMessageOutput
// ----------------------------------------------------------------------------

wxMessageOutput* wxMessageOutput::ms_msgOut = nullptr;

wxMessageOutput* wxMessageOutput::Get()
{
    if ( !ms_msgOut && wxTheApp )
    {
        ms_msgOut = wxTheApp->GetTraits()->CreateMessageOutput();
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
// wxMessageOutputBest
// ----------------------------------------------------------------------------

void wxMessageOutputBest::Output(const wxString& str)
{
#ifdef __WINDOWS__
    // decide whether to use console output or not
    wxAppTraits * const traits = wxApp::GetTraitsIfExists();
    const bool hasStderr = traits ? traits->CanUseStderr() : false;

    if ( !(m_flags & wxMSGOUT_PREFER_MSGBOX) )
    {
        if ( hasStderr && traits->WriteToStderr(AppendLineFeedIfNeeded(str)) )
            return;
    }

    wxString title;
    if ( wxTheApp )
        title = wxTheApp->GetAppDisplayName();
    else // Use some title to avoid default "Error"
        title = _("Message");

    ::MessageBox(nullptr, str.t_str(), title.t_str(), MB_ICONINFORMATION | MB_OK);
#else // !__WINDOWS__
    wxUnusedVar(m_flags);

    // TODO: use the native message box for the other ports too
    wxMessageOutputStderr::Output(str);
#endif // __WINDOWS__/!__WINDOWS__
}

// ----------------------------------------------------------------------------
// wxMessageOutputWithConv
// ----------------------------------------------------------------------------

wxString wxMessageOutputWithConv::AppendLineFeedIfNeeded(const wxString& str)
{
    wxString strLF(str);
    if ( strLF.empty() || *strLF.rbegin() != '\n' )
        strLF += '\n';

    return strLF;
}

wxCharBuffer wxMessageOutputWithConv::PrepareForOutput(const wxString& str)
{
    wxString strWithLF = AppendLineFeedIfNeeded(str);

#if defined(__WINDOWS__)
    // Determine whether the encoding is UTF-16. In that case, the file
    // should have been opened in "wb" mode, and EOL conversion must be done
    // here as it won't be done at stdio level.
    if ( m_conv->GetMBNulLen() == 2 )
    {
        strWithLF.Replace("\n", "\r\n");
    }
#endif // __WINDOWS__

    return m_conv->cWX2MB(strWithLF.c_str());
}

// ----------------------------------------------------------------------------
// wxMessageOutputStderr
// ----------------------------------------------------------------------------

wxMessageOutputStderr::wxMessageOutputStderr(FILE *fp, const wxMBConv& conv)
                     : wxMessageOutputWithConv(conv),
                       m_fp(fp)
{
}

void wxMessageOutputStderr::Output(const wxString& str)
{
    const wxCharBuffer& buf = PrepareForOutput(str);

#if defined(__ANDROID__)
    __android_log_write(ANDROID_LOG_INFO, "wxWidgets", buf.data());
#else
    fwrite(buf, buf.length(), 1, m_fp);
    fflush(m_fp);
#endif
}

// ----------------------------------------------------------------------------
// wxMessageOutputDebug
// ----------------------------------------------------------------------------

void wxMessageOutputDebug::Output(const wxString& str)
{
#if defined(__WINDOWS__)
    wxString out(AppendLineFeedIfNeeded(str));
    out.Replace(wxT("\t"), wxT("        "));
    out.Replace(wxT("\n"), wxT("\r\n"));
    ::OutputDebugString(out.t_str());
#elif defined(__ANDROID__)
    const wxCharBuffer& buf = PrepareForOutput(str);

    __android_log_write(ANDROID_LOG_DEBUG, "wxWidgets", buf.data());
#else
    // TODO: use native debug output function for the other ports too
    wxMessageOutputStderr::Output(str);
#endif // platform
}

// ----------------------------------------------------------------------------
// wxMessageOutputLog
// ----------------------------------------------------------------------------

void wxMessageOutputLog::Output(const wxString& str)
{
    wxString out(str);

    out.Replace(wxT("\t"), wxT("        "));

    wxLogMessage(wxT("%s"), out);
}

#endif // wxUSE_BASE

// ----------------------------------------------------------------------------
// wxMessageOutputMessageBox
// ----------------------------------------------------------------------------

#if wxUSE_GUI && wxUSE_MSGDLG

extern WXDLLEXPORT_DATA(const char) wxMessageBoxCaptionStr[] = "Message";

void wxMessageOutputMessageBox::Output(const wxString& str)
{
    wxString out(str);

    // the native MSW msg box understands the TABs, others don't
#ifndef __WINDOWS__
    out.Replace(wxT("\t"), wxT("        "));
#endif

    wxString title = wxT("wxWidgets") ;
    if (wxTheApp) title = wxTheApp->GetAppDisplayName();

    ::wxMessageBox(out, title);
}

#endif // wxUSE_GUI
