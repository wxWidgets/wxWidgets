///////////////////////////////////////////////////////////////////////////////
// Name:        wx/apptrait.h
// Purpose:     declaration of wxAppTraits and derived classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APPTRAIT_H_
#define _WX_APPTRAIT_H_

#include "wx/string.h"

class WXDLLIMPEXP_BASE wxObject;
class WXDLLEXPORT wxAppTraits;
#if wxUSE_FONTMAP
    class WXDLLEXPORT wxFontMapper;
#endif // wxUSE_FONTMAP
class WXDLLIMPEXP_BASE wxLog;
class WXDLLIMPEXP_BASE wxMessageOutput;
class WXDLLEXPORT wxRendererNative;
class WXDLLIMPEXP_BASE wxString;

class GSocketGUIFunctionsTable;

// ----------------------------------------------------------------------------
// toolkit information
// ----------------------------------------------------------------------------

// Information about the toolkit that the app is running under (e.g. wxMSW):
struct WXDLLIMPEXP_BASE wxToolkitInfo
{
    // Short name of the toolkit (e.g. "msw" or "mswuniv"); empty for console:
    wxString shortName;
    // Descriptive name of the toolkit, human readable (e.g. "wxMSW" or
    // "wxMSW/Universal"); "wxBase" for console apps:
    wxString name;
    // Version of the underlying toolkit or of the OS for console apps:
    int versionMajor, versionMinor;
    // OS mnenomics, e.g. wxGTK or wxMSW:
    int os;
};


// ----------------------------------------------------------------------------
// wxAppTraits: this class defines various configurable aspects of wxApp
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPathsBase;

class WXDLLIMPEXP_BASE wxAppTraitsBase
{
public:
    // needed since this class declares virtual members
    virtual ~wxAppTraitsBase() { }

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

    // get the renderer to use for drawing the generic controls (return value
    // may be NULL in which case the default renderer for the current platform
    // is used); this is used in GUI only and always returns NULL in console
    //
    // NB: returned pointer will be deleted by the caller
    virtual wxRendererNative *CreateRenderer() = 0;

#if wxUSE_STDPATHS
    // wxStandardPaths object is normally the same for wxBase and wxGUI
    // except in the case of wxMac and wxCocoa
    virtual wxStandardPathsBase& GetStandardPaths();
#endif // wxUSE_STDPATHS

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

#if wxUSE_SOCKETS
    // return table of GUI callbacks for GSocket code or NULL in wxBase. This
    // is needed because networking classes are in their own library and so
    // they can't directly call GUI functions (the same net library can be
    // used in both GUI and base apps). To complicate it further, GUI library
    // ("wxCore") doesn't depend on networking library and so only a functions
    // table can be passed around
    virtual GSocketGUIFunctionsTable* GetSocketGUIFunctionsTable() = 0;
#endif


    // return information about what toolkit is running; we need for two things
    // that are both contained in wxBase:
    //  - wxGetOsVersion() behaves differently in GUI and non-GUI builds under
    //    Unix: in the former case it returns the information about the toolkit
    //    and in the latter -- about the OS, so we need to virtualize it
    //  - wxDynamicLibrary::CanonicalizePluginName() must embed toolkit
    //    signature in DLL name
    virtual wxToolkitInfo& GetToolkitInfo() = 0;
};

// ----------------------------------------------------------------------------
// include the platform-specific version of the class
// ----------------------------------------------------------------------------

// NB:  test for __UNIX__ before __WXMAC__ as under Darwin we want to use the
//      Unix code (and otherwise __UNIX__ wouldn't be defined)
// ABX: check __WIN32__ instead of __WXMSW__ for the same MSWBase in any Win32 port
#if defined(__WXPALMOS__)
    #include "wx/palmos/apptbase.h"
#elif defined(__WIN32__)
    #include "wx/msw/apptbase.h"
#elif defined(__UNIX__) && !defined(__EMX__)
    #include "wx/unix/apptbase.h"
#elif defined(__WXMAC__)
    #include "wx/mac/apptbase.h"
#elif defined(__OS2__)
    #include "wx/os2/apptbase.h"
#else // no platform-specific methods to add to wxAppTraits
    // wxAppTraits must be a class because it was forward declared as class
    class WXDLLIMPEXP_BASE wxAppTraits : public wxAppTraitsBase
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
    virtual wxRendererNative *CreateRenderer();
#if wxUSE_SOCKETS
    virtual GSocketGUIFunctionsTable* GetSocketGUIFunctionsTable();
#endif

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
    virtual wxRendererNative *CreateRenderer();
#if wxUSE_SOCKETS
    virtual GSocketGUIFunctionsTable* GetSocketGUIFunctionsTable();
#endif

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

// ABX: check __WIN32__ instead of __WXMSW__ for the same MSWBase in any Win32 port
#if defined(__WXPALMOS__)
    #include "wx/palmos/apptrait.h"
#elif defined(__WIN32__)
    #include "wx/msw/apptrait.h"
#elif defined(__UNIX__) && !defined(__EMX__)
    #include "wx/unix/apptrait.h"
#elif defined(__WXMAC__)
    #include "wx/mac/apptrait.h"
#elif defined(__WXPM__)
    #include "wx/os2/apptrait.h"
#else
    // at least, we need an implementation of GetToolkitInfo !
    #if wxUSE_GUI
        class wxGUIAppTraits : public wxGUIAppTraitsBase
        {
            virtual wxToolkitInfo& GetToolkitInfo();
        };
    #endif // wxUSE_GUI
    class wxConsoleAppTraits: public wxConsoleAppTraitsBase
    {
        virtual wxToolkitInfo& GetToolkitInfo();
    };
#endif // platform

#endif // _WX_APPTRAIT_H_

