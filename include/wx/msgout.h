///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msgout.h
// Purpose:     wxMessageOutput class. Shows a message to the user
// Author:      Mattia Barbon
// Modified by:
// Created:     17.07.02
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGOUT_H_
#define _WX_MSGOUT_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA) && !defined(__EMX__)
// Some older compilers (such as EMX) cannot handle
// #pragma interface/implementation correctly, iff
// #pragma implementation is used in _two_ translation
// units (as created by e.g. event.cpp compiled for
// libwx_base and event.cpp compiled for libwx_gui_core).
// So we must not use those pragmas for those compilers in
// such files.
    #pragma interface "msgout.h"
#endif

#include "wx/defs.h"
#include "wx/wxchar.h"

// ----------------------------------------------------------------------------
// wxMessageOutput is a class abstracting formatted output target, i.e.
// something you can printf() to
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMessageOutput
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

class WXDLLIMPEXP_BASE wxMessageOutputStderr : public wxMessageOutput
{
public:
    wxMessageOutputStderr() { }

    virtual void Printf(const wxChar* format, ...) ATTRIBUTE_PRINTF_2;
};

// ----------------------------------------------------------------------------
// implementation which shows output in a message box
// ----------------------------------------------------------------------------

#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxMessageOutputMessageBox : public wxMessageOutput
{
public:
    wxMessageOutputMessageBox() { }

    virtual void Printf(const wxChar* format, ...) ATTRIBUTE_PRINTF_2;
};

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// implementation using the native way of outputting debug messages
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMessageOutputDebug : public wxMessageOutput
{
public:
    wxMessageOutputDebug() { }

    virtual void Printf(const wxChar* format, ...) ATTRIBUTE_PRINTF_2;
};

// ----------------------------------------------------------------------------
// implementation using wxLog (mainly for backwards compatibility)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMessageOutputLog : public wxMessageOutput
{
public:
    wxMessageOutputLog() { }

    virtual void Printf(const wxChar* format, ...) ATTRIBUTE_PRINTF_2;
};

#endif
    // _WX_MSGOUT_H_
