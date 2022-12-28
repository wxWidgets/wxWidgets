///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msgout.h
// Purpose:     wxMessageOutput class. Shows a message to the user
// Author:      Mattia Barbon
// Modified by:
// Created:     17.07.02
// Copyright:   (c) Mattia Barbon
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGOUT_H_
#define _WX_MSGOUT_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"
#include "wx/string.h"

// ----------------------------------------------------------------------------
// wxMessageOutput is a class abstracting formatted output target, i.e.
// something you can printf() to
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMessageOutput
{
public:
    virtual ~wxMessageOutput() { }

    // gets the current wxMessageOutput object (may be null during
    // initialization or shutdown)
    static wxMessageOutput* Get();

    // sets the global wxMessageOutput instance; returns the previous one
    static wxMessageOutput* Set(wxMessageOutput* msgout);

    // show a message to the user
    template <typename FormatString, typename... Targs>
    void Printf(FormatString format, Targs... args)
    {
        Output(wxString::Format(format, args...));
    }

    // called by DoPrintf() to output formatted string but can also be called
    // directly if no formatting is needed
    virtual void Output(const wxString& str) = 0;

private:
    static wxMessageOutput* ms_msgOut;
};

// ----------------------------------------------------------------------------
// helper mix-in for output targets that can use difference encodings
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMessageOutputWithConv
{
protected:
    explicit wxMessageOutputWithConv(const wxMBConv& conv)
        : m_conv(conv.Clone())
    {
    }

    ~wxMessageOutputWithConv()
    {
        delete m_conv;
    }

    // return the string with "\n" appended if it doesn't already terminate
    // with it (in which case it's returned unchanged)
    wxString AppendLineFeedIfNeeded(const wxString& str);

    // Prepare the given string for output by appending a new line to it, if
    // necessary, and converting it to a narrow string using our conversion
    // object.
    wxCharBuffer PrepareForOutput(const wxString& str);

    const wxMBConv* const m_conv;
};

// ----------------------------------------------------------------------------
// implementation which sends output to stderr or specified file
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMessageOutputStderr : public wxMessageOutput,
                                               protected wxMessageOutputWithConv
{
public:
    wxMessageOutputStderr(FILE *fp = stderr,
                          const wxMBConv &conv = wxConvWhateverWorks);

    virtual void Output(const wxString& str) override;

protected:
    FILE *m_fp;

    wxDECLARE_NO_COPY_CLASS(wxMessageOutputStderr);
};

// ----------------------------------------------------------------------------
// implementation showing the message to the user in "best" possible way:
// uses stderr or message box if available according to the flag given to ctor.
// ----------------------------------------------------------------------------

enum wxMessageOutputFlags
{
    wxMSGOUT_PREFER_STDERR = 0, // use stderr if available (this is the default)
    wxMSGOUT_PREFER_MSGBOX = 1  // always use message box if available
};

class WXDLLIMPEXP_BASE wxMessageOutputBest : public wxMessageOutputStderr
{
public:
    wxMessageOutputBest(wxMessageOutputFlags flags = wxMSGOUT_PREFER_STDERR)
        : m_flags(flags) { }

    virtual void Output(const wxString& str) override;

private:
    wxMessageOutputFlags m_flags;
};

// ----------------------------------------------------------------------------
// implementation which shows output in a message box
// ----------------------------------------------------------------------------

#if wxUSE_GUI && wxUSE_MSGDLG

class WXDLLIMPEXP_CORE wxMessageOutputMessageBox : public wxMessageOutput
{
public:
    wxMessageOutputMessageBox() { }

    virtual void Output(const wxString& str) override;
};

#endif // wxUSE_GUI && wxUSE_MSGDLG

// ----------------------------------------------------------------------------
// implementation using the native way of outputting debug messages
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMessageOutputDebug : public wxMessageOutputStderr
{
public:
    wxMessageOutputDebug() { }

    virtual void Output(const wxString& str) override;
};

// ----------------------------------------------------------------------------
// implementation using wxLog (mainly for backwards compatibility)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMessageOutputLog : public wxMessageOutput
{
public:
    wxMessageOutputLog() { }

    virtual void Output(const wxString& str) override;
};

#endif // _WX_MSGOUT_H_
