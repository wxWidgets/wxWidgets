///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/corefoundation/stdpaths.h
// Purpose:     wxStandardPaths for CoreFoundation systems
// Author:      David Elliott
// Modified by:
// Created:     2004-10-27
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_STDPATHS_H_
#define _WX_MAC_STDPATHS_H_

struct __CFBundle;
struct __CFURL;

typedef const __CFURL * wxCFURLRef;
typedef __CFBundle * wxCFBundleRef;

// ----------------------------------------------------------------------------
// wxStandardPaths
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPathsCF : public wxStandardPathsBase
{
public:
    wxStandardPathsCF();
    virtual ~wxStandardPathsCF();

    // wxMac specific: allow user to specify a different bundle
    wxStandardPathsCF(wxCFBundleRef bundle);
    void SetBundle(wxCFBundleRef bundle);

    // implement base class pure virtuals
    virtual wxString GetExecutablePath() const;
    virtual wxString GetConfigDir() const;
    virtual wxString GetUserConfigDir() const;
    virtual wxString GetDataDir() const;
    virtual wxString GetLocalDataDir() const;
    virtual wxString GetUserDataDir() const;
    virtual wxString GetPluginsDir() const;
    virtual wxString GetResourcesDir() const;
    virtual wxString GetLocalizedResourcesDir(const wxChar *lang,
                                              ResourceCat category) const;
    virtual wxString GetDocumentsDir() const;

protected:
    // this function can be called with any of CFBundleCopyXXXURL function
    // pointer as parameter
    wxString GetFromFunc(wxCFURLRef (*func)(wxCFBundleRef)) const;

    wxCFBundleRef m_bundle;
};

// If using UNIX (i.e. darwin) then use UNIX standard paths
#if defined(__UNIX__)
#include "wx/unix/stdpaths.h"
#else
// If compiling wxMac for CarbonLib then we are wxStandardPaths
class WXDLLIMPEXP_BASE wxStandardPaths: public wxStandardPathsCF
{
};
#endif

#endif // _WX_MAC_STDPATHS_H_
