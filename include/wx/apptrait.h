///////////////////////////////////////////////////////////////////////////////
// Name:        wx/apptrait.h
// Purpose:     declaration of wxAppTraits and derived classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APPTRAIT_H_
#define _WX_APPTRAIT_H_

class WXDLLEXPORT wxObject;
class WXDLLEXPORT wxAppTraits;
#if wxUSE_FONTMAP
    class WXDLLEXPORT wxFontMapper;
#endif // wxUSE_FONTMAP
class WXDLLEXPORT wxLog;
class WXDLLEXPORT wxMessageOutput;

// ----------------------------------------------------------------------------
// wxAppTraits: this class defines various configurable aspects of wxApp
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxAppTraitsBase
{
public:
    // hooks for creating the global objects, may be overridden by the user
    // ------------------------------------------------------------------------

#if wxUSE_LOG
    // create the default log target
    virtual wxLog *CreateLogTarget() = 0;
#endif // wxUSE_LOG

    // create the global object used for printing out messages
    virtual wxMessageOutput *CreateMessageOutput() = 0;

#if wxUSE_FONTMAP
    // create the global font mapper object used for encodings/charset mapping
    virtual wxFontMapper *CreateFontMapper() = 0;
#endif // wxUSE_FONTMAP


    // functions abstracting differences between GUI and console modes
    // ------------------------------------------------------------------------

#ifdef __WXDEBUG__
    // show the assert dialog with the specified message in GUI or just print
    // the string to stderr in console mode
    //
    // base class version has an implementation (in spite of being pure
    // virtual) in base/appbase.cpp which can be called as last resort.
    //
    // return true to suppress subsequent asserts, false to continue as before
    virtual bool ShowAssertDialog(const wxString& msg) = 0;
#endif // __WXDEBUG__

    // return true if fprintf(stderr) goes somewhere, false otherwise
    virtual bool HasStderr() = 0;

    // managing "pending delete" list: in GUI mode we can't immediately delete
    // some objects because there may be unprocessed events for them and so we
    // only do it during the next idle loop iteration while this is, of course,
    // unnecessary in wxBase, so we have a few functions to abstract these
    // operations

    // add the object to the pending delete list in GUI, delete it immediately
    // in wxBase
    virtual void ScheduleForDestroy(wxObject *object) = 0;

    // remove this object from the pending delete list in GUI, do nothing in
    // wxBase
    virtual void RemoveFromPendingDelete(wxObject *object) = 0;


    // other miscellaneous helpers
    // ---------------------------

    // wxGetOsVersion() behaves differently in GUI and non-GUI builds under
    // Unix: in the former case it returns the information about the toolkit
    // and in the latter -- about the OS, so we need to virtualize it
    virtual int GetOSVersion(int *verMaj, int *verMin) = 0;
};

// ----------------------------------------------------------------------------
// include the platform-specific version of the class
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/apptbase.h"
#elif defined(__UNIX__)
    #include "wx/unix/apptbase.h"
#else // no platform-specific methods to add to wxAppTraits
    typedef 
    // wxAppTraits must be a class because it was forward declared as class
    class WXDLLEXPORT wxAppTraits : public wxAppTraitsBase
    {
    };
#endif // platform

// ============================================================================
// standard traits for console and GUI applications
// ============================================================================

// ----------------------------------------------------------------------------
// wxConsoleAppTraitsBase: wxAppTraits implementation for the console apps
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxConsoleAppTraitsBase : public wxAppTraits
{
public:
#if wxUSE_LOG
    virtual wxLog *CreateLogTarget();
#endif // wxUSE_LOG
    virtual wxMessageOutput *CreateMessageOutput();
#if wxUSE_FONTMAP
    virtual wxFontMapper *CreateFontMapper();
#endif // wxUSE_FONTMAP

#ifdef __WXDEBUG__
    virtual bool ShowAssertDialog(const wxString& msg);
#endif // __WXDEBUG__
    virtual bool HasStderr();

    virtual void ScheduleForDestroy(wxObject *object);
    virtual void RemoveFromPendingDelete(wxObject *object);
};

// ----------------------------------------------------------------------------
// wxGUIAppTraitsBase: wxAppTraits implementation for the GUI apps
// ----------------------------------------------------------------------------

#if wxUSE_GUI

class WXDLLEXPORT wxGUIAppTraitsBase : public wxAppTraits
{
public:
#if wxUSE_LOG
    virtual wxLog *CreateLogTarget();
#endif // wxUSE_LOG
    virtual wxMessageOutput *CreateMessageOutput();
#if wxUSE_FONTMAP
    virtual wxFontMapper *CreateFontMapper();
#endif // wxUSE_FONTMAP

#ifdef __WXDEBUG__
    virtual bool ShowAssertDialog(const wxString& msg);
#endif // __WXDEBUG__
    virtual bool HasStderr();

    virtual void ScheduleForDestroy(wxObject *object);
    virtual void RemoveFromPendingDelete(wxObject *object);
};

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// include the platform-specific version of the classes above
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/apptrait.h"
#elif defined(__UNIX__)
    #include "wx/unix/apptrait.h"
#else // no platform-specific methods to add to wxAppTraits
    #if wxUSE_GUI
        typedef wxGUIAppTraitsBase wxGUIAppTraits;
    #endif // wxUSE_GUI
    typedef wxConsoleAppTraitsBase wxConsoleAppTraits;
#endif // platform

#endif // _WX_APPTRAIT_H_

