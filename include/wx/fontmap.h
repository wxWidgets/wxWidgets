/////////////////////////////////////////////////////////////////////////////
// Name:        wx/fontmap.h
// Purpose:     wxFontMapper class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.11.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTMAPPER_H_
#define _WX_FONTMAPPER_H_

#ifdef __GNUG__
    #pragma interface "fontmap.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"        // for wxDEFAULT &c

#include "wx/font.h"        // for wxFontEncoding

class WXDLLEXPORT wxConfigBase;

// ----------------------------------------------------------------------------
// wxFontMapper manages user-definable correspondence between logical font
// names and the fonts present on the machine.
//
// The default implementations of all functions will ask the user if they are
// not capable of finding the answer themselves and store the answer in a
// config file (configurable via SetConfigXXX functions). This behaviour may
// be disabled by giving the value of FALSE to "interactive" parameter.
// However, the functions will always consult the config file to allow the
// user-defined values override the default logic and there is no way to
// disable this - which shouldn't be ever needed because if "interactive" was
// never TRUE, the config file is never created anyhow.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontMapper
{
public:
    // default ctor
    wxFontMapper();

    // virtual dtor for a base class
    virtual ~wxFontMapper();

    // returns the encoding for the given charset (in the form of RFC 2046) or
    // wxFONTENCODING_SYSTEM if couldn't decode it
    virtual wxFontEncoding CharsetToEncoding(const wxString& charset,
                                             bool interactive = TRUE);

    // configure the appearance of the dialogs we may popup
    // ----------------------------------------------------

    void SetDialogTitle(const wxString& title) { m_titleDialog = title; }

    // functions which allow to configure the config object used: by default,
    // the global one (from wxConfigBase::Get() will be used) and the default
    // root path for the config settings is the string returned by
    // GetDefaultConfigPath()
    // ----------------------------------------------------------------------

    // set the config object to use (may be NULL to use default)
    void SetConfig(wxConfigBase *config) { m_config = config; }

    // set the root config path to use (should be an absolute path)
    void SetConfigPath(const wxString& prefix);

    // return default config path
    static const wxChar *GetDefaultConfigPath();

protected:
    // get the config object we're using - if it wasn't set explicitly, this
    // function will use wxConfig::Get() to get the global one
    wxConfigBase *GetConfig();

    // gets the root path for our settings - if itwasn't set explicitly, use
    // GetDefaultConfigPath()
    const wxString& GetConfigPath();

    // change to the given (relative) path in the config, return TRUE if ok
    // (then GetConfig() will return something !NULL), FALSE if no config
    // object
    //
    // caller should provide a pointer to the string variable which should be
    // later passed to RestorePath()
    bool ChangePath(const wxString& pathNew, wxString *pathOld);

    // restore the config path after use
    void RestorePath(const wxString& pathOld);

    // config object and path (in it) to use
    wxConfigBase *m_config;
    wxString  m_configRootPath;

    // the title for our dialogs
    wxString m_titleDialog;

    // the parent window for our dialogs
    wxWindow *m_windowParent;
};

#endif // _WX_FONTMAPPER_H_
