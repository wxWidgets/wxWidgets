///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/stackwalk.h
// Purpose:     declaration of wxStackWalker for Unix
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-19
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_STACKWALK_H_
#define _WX_UNIX_STACKWALK_H_

// ----------------------------------------------------------------------------
// wxStackFrame
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStackFrame : public wxStackFrameBase
{
public:
    // arguments are the stack depth of this frame, its address and the return
    // value of backtrace_symbols() for it
    //
    // NB: we don't copy syminfo pointer so it should have lifetime at least as
    //     long as ours
    wxStackFrame(size_t level, void *address, const char *syminfo)
        : wxStackFrameBase(level, address)
    {
        m_hasName =
        m_hasLocation = false;

        m_syminfo = syminfo;
    }

protected:
    virtual void OnGetName();
    virtual void OnGetLocation();

private:
    const char *m_syminfo;

    bool m_hasName,
         m_hasLocation;
};

// ----------------------------------------------------------------------------
// wxStackWalker
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStackWalker : public wxStackWalkerBase
{
public:
    // we need the full path to the program executable to be able to use
    // addr2line, normally we can retrieve it from wxTheApp but if wxTheApp
    // doesn't exist or doesn't have the correct value, the path may be given
    // explicitly
    wxStackWalker(const char *argv0 = NULL)
    {
        ms_exepath = wxString::FromAscii(argv0);
    }

    virtual void Walk(size_t skip = 1);
    virtual void WalkFromException() { Walk(2); }

    static const wxString& GetExePath() { return ms_exepath; }

private:
    static wxString ms_exepath;
};

#endif // _WX_UNIX_STACKWALK_H_
