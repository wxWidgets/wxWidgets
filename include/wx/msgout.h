///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msgout.h
// Purpose:     wxMessageOutput class. Shows a message to the user
// Author:      Mattia Barbon
// Modified by:
// Created:     17.07.02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGOUT_H_
#define _WX_MSGOUT_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "msgout.h"
#endif

#include "wx/defs.h"
#include "wx/wxchar.h"

// ----------------------------------------------------------------------------
// wxMessageOutput is a class abstracting formatted output target, i.e.
// something you can printf() to
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMessageOutput
{
public:
    virtual ~wxMessageOutput() { }

    // show a message to the user
    virtual void Printf(const wxChar* format, ...)  ATTRIBUTE_PRINTF_2 = 0;

    // gets the current wxMessageOutput object
    static wxMessageOutput* Get();

    // sets the global wxMessageOutput instance; returns the previous one
    static wxMessageOutput* Set(wxMessageOutput* msgout);

private:
    static wxMessageOutput* ms_msgOut;
};

// ----------------------------------------------------------------------------
// implementation which sends output to stderr
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMessageOutputStderr : public wxMessageOutput
{
public:
    wxMessageOutputStderr() { }

    virtual void Printf(const wxChar* format, ...) ATTRIBUTE_PRINTF_2;
};

// ----------------------------------------------------------------------------
// implementation which shows output in a message box
// ----------------------------------------------------------------------------

#if wxUSE_GUI

class WXDLLEXPORT wxMessageOutputMessageBox : public wxMessageOutput
{
public:
    wxMessageOutputMessageBox() { }

    virtual void Printf(const wxChar* format, ...) ATTRIBUTE_PRINTF_2;
};

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// implementation using wxLog (mainly for backwards compatibility)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMessageOutputLog : public wxMessageOutput
{
public:
    wxMessageOutputLog() { }

    virtual void Printf(const wxChar* format, ...) ATTRIBUTE_PRINTF_2;
};

#endif
    // _WX_MSGOUT_H_
