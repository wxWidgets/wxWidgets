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
