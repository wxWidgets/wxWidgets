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

// ----------------------------------------------------------------------------
// wxStandardPaths
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPathsCF : public wxStandardPathsBase
{
public:
    wxStandardPathsCF();
    ~wxStandardPathsCF();

    // return the global standard paths object
    // Overrides wxStandardPathsBase version for GUI code.
    static wxStandardPathsCF& Get();

    // wxMac specific: allow user to specify a different bundle
    wxStandardPathsCF(struct __CFBundle *bundle);
    void SetBundle(struct __CFBundle *bundle);

    // implement base class pure virtuals
    virtual wxString GetConfigDir() const;
    virtual wxString GetUserConfigDir() const;
    virtual wxString GetDataDir() const;
    virtual wxString GetLocalDataDir() const;
    virtual wxString GetUserDataDir() const;
    virtual wxString GetPluginsDir() const;
protected:
    struct __CFBundle *m_bundle;
};

// wxMac has its own base so it always uses this version.
// Otherwise, we want to use this version only when compiling GUI code.
// The CoreFoundation version is always available by its full name to all code.
#if defined(__WXMAC__) || wxUSE_BASE == 0
#define wxStandardPaths wxStandardPathsCF
#else
#include "wx/unix/stdpaths.h"
#endif

#endif // _WX_MAC_STDPATHS_H_
