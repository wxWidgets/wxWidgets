/////////////////////////////////////////////////////////////////////////////
// Name:        msw/stackwalk.cpp
// Purpose:     wxStackWalker implementation for Unix/glibc
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-18
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STACKWALKER

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/utils.h"
#endif

#include "wx/stackwalk.h"

#include <execinfo.h>

#ifdef HAVE_CXA_DEMANGLE
    #include <cxxabi.h>
#endif // HAVE_CXA_DEMANGLE

// ----------------------------------------------------------------------------
// tiny helper wrapper around popen/pclose()
// ----------------------------------------------------------------------------

class wxStdioPipe
{
public:
    // ctor parameters are passed to popen()
    wxStdioPipe(const char *command, const char *type)
    {
        m_fp = popen(command, type);
    }

    // conversion to stdio FILE
    operator FILE *() const { return m_fp; }

    // dtor closes the pipe
    ~wxStdioPipe()
    {
        if ( m_fp )
            pclose(m_fp);
    }

private:
    FILE *m_fp;

    DECLARE_NO_COPY_CLASS(wxStdioPipe)
};

// ============================================================================
// implementation
// ============================================================================

wxString wxStackWalker::ms_exepath;

// ----------------------------------------------------------------------------
// wxStackFrame
// ----------------------------------------------------------------------------

void wxStackFrame::OnGetName()
{
    if ( m_hasName )
        return;

    m_hasName = true;

    // try addr2line first because it always gives us demangled names (even if
    // __cxa_demangle is not available) and because it seems less error-prone
    // when it works, backtrace_symbols() sometimes returns incorrect results
    OnGetLocation();

    // format is: "module(funcname+offset) [address]" but the part in
    // parentheses can be not present
    wxString syminfo = wxString::FromAscii(m_syminfo);
    const size_t posOpen = syminfo.find(_T('('));
    if ( posOpen != wxString::npos )
    {
        const size_t posPlus = syminfo.find(_T('+'), posOpen + 1);
        if ( posPlus != wxString::npos )
        {
            const size_t posClose = syminfo.find(_T(')'), posPlus + 1);
            if ( posClose != wxString::npos )
            {
                if ( m_name.empty() )
                {
                    m_name.assign(syminfo, posOpen + 1, posPlus - posOpen - 1);

#ifdef HAVE_CXA_DEMANGLE
                    int rc = -1;
                    char *cppfunc = __cxxabiv1::__cxa_demangle
                                    (
                                        m_name.mb_str(),
                                        NULL, // output buffer (none, alloc it)
                                        NULL, // [out] len of output buffer
                                        &rc
                                    );
                    if ( rc == 0 )
                        m_name = wxString::FromAscii(cppfunc);

                    free(cppfunc);
#endif // HAVE_CXA_DEMANGLE
                }

                unsigned long ofs;
                if ( wxString(syminfo, posPlus + 1, posClose - posPlus - 1).
                        ToULong(&ofs, 0) )
                    m_offset = ofs;
            }
        }

        m_module.assign(syminfo, posOpen);
    }
    else // not in "module(funcname+offset)" format
    {
        m_module = syminfo;
    }
}

void wxStackFrame::OnGetLocation()
{
    if ( m_hasLocation )
        return;

    m_hasLocation = true;

    // we need to launch addr2line tool to get this information and we need to
    // have the program name for this
    wxString exepath = wxStackWalker::GetExePath();
    if ( exepath.empty() )
    {
        if ( !wxTheApp || !wxTheApp->argv )
            return;
        exepath = wxTheApp->argv[0];
    }

    wxStdioPipe fp(wxString::Format(_T("addr2line -C -f -e \"%s\" %p"),
                                    exepath.c_str(), m_address).mb_str(),
                   "r");

    if ( !fp )
        return;

    // parse addr2line output
    char buf[1024];
    if ( !fgets(buf, WXSIZEOF(buf), fp) )
    {
        wxLogDebug(_T("Empty addr2line output?"));
        return;
    }

    // 1st line has function name
    if ( GetName().empty() )
    {
        m_name = wxString::FromAscii(buf);
        m_name.RemoveLast(); // trailing newline

        if ( m_name == _T("??") )
            m_name.clear();
    }

    // 2nd one -- the file/line info
    if ( fgets(buf, WXSIZEOF(buf), fp) )
    {
        wxString output(wxString::FromAscii(buf));
        output.RemoveLast();

        const size_t posColon = output.find(_T(':'));
        if ( posColon != wxString::npos )
        {
            m_filename.assign(output, 0, posColon);
            if ( m_filename == _T("??") )
            {
                m_filename.clear();
            }
            else
            {
                unsigned long line;
                if ( wxString(output, posColon + 1, wxString::npos).
                        ToULong(&line) )
                    m_line = line;
            }
        }
        else
        {
            wxLogDebug(_T("Unexpected addr2line format: \"%s\""),
                       output.c_str());
        }
    }
}

// ----------------------------------------------------------------------------
// wxStackWalker
// ----------------------------------------------------------------------------

void wxStackWalker::Walk(size_t skip)
{
    // that many frames should be enough for everyone
    void *addresses[200];

    int depth = backtrace(addresses, WXSIZEOF(addresses));
    if ( !depth )
        return;

    char **symbols = backtrace_symbols(addresses, depth);

    if (skip > (size_t) depth)
        skip = (size_t) depth;

    for ( int n = skip; n < depth; n++ )
    {
        wxStackFrame frame(n, addresses[n-skip], symbols[n-skip]);
        OnStackFrame(frame);
    }
}

#endif // wxUSE_STACKWALKER
