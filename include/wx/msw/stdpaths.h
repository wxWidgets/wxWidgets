///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/stdpaths.h
// Purpose:     wxStandardPaths for Win32
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-10-19
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_STDPATHS_H_
#define _WX_MSW_STDPATHS_H_

// ----------------------------------------------------------------------------
// wxStandardPaths
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPaths : public wxStandardPathsBase
{
public:
    // implement base class pure virtuals
    virtual wxString GetConfigDir() const;
    virtual wxString GetUserConfigDir() const;
    virtual wxString GetDataDir() const;
    virtual wxString GetUserDataDir() const;
    virtual wxString GetUserLocalDataDir() const;
    virtual wxString GetPluginsDir() const;

protected:
    // append "/appname" suffix if the app name is set
    static wxString AppendAppName(const wxString& dir);

    // get the path corresponding to the given standard CSIDL_XXX constant
    static wxString DoGetDirectory(int csidl);
};

// ----------------------------------------------------------------------------
// wxStandardPathsWin16: this class is for internal use only
// ----------------------------------------------------------------------------

// override config file locations to be compatible with the values used by
// wxFileConfig (dating from Win16 days which explains the class name)
class WXDLLIMPEXP_BASE wxStandardPathsWin16 : public wxStandardPaths
{
public:
    virtual wxString GetConfigDir() const;
    virtual wxString GetUserConfigDir() const;
};

#endif // _WX_MSW_STDPATHS_H_
