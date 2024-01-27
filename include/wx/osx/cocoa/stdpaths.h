/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/cocoa/stdpaths.h
// Purpose:     wxStandardPaths for Cocoa
// Author:      Tobias Taschner
// Created:     2015-09-09
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_STDPATHS_H_
#define _WX_COCOA_STDPATHS_H_

// ----------------------------------------------------------------------------
// wxStandardPaths
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPaths : public wxStandardPathsBase
{
public:
    virtual ~wxStandardPaths();

    // implement base class pure virtuals
    virtual wxString GetExecutablePath() const override;
    virtual wxString GetConfigDir() const override;
    virtual wxString GetUserConfigDir() const override;
    virtual wxString GetDataDir() const override;
    virtual wxString GetLocalDataDir() const override;
    virtual wxString GetUserDataDir() const override;
    virtual wxString GetPluginsDir() const override;
    virtual wxString GetSharedLibrariesDir() const override;
    virtual wxString GetResourcesDir() const override;
    virtual wxString
    GetLocalizedResourcesDir(const wxString& lang,
                             ResourceCat category = ResourceCat_None) const override;
    virtual wxString GetUserDir(Dir userDir) const override;
    virtual wxString MakeConfigFileName(const wxString& basename,
                                        ConfigFileConv conv = ConfigFileConv_Ext
                                        ) const override;

protected:
    // Ctor is protected, use wxStandardPaths::Get() instead of instantiating
    // objects of this class directly.
    wxStandardPaths();
};


#endif // _WX_COCOA_STDPATHS_H_
