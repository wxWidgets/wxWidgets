///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/stdpaths.h
// Purpose:     wxStandardPaths for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_STDPATHS_H_
#define _WX_RETRO68_STDPATHS_H_

// ----------------------------------------------------------------------------
// wxStandardPaths: uses the Folder Manager to find the standard folders
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPaths : public wxStandardPathsBase
{
public:
    // implement base class pure virtuals
    virtual wxString GetExecutablePath() const override;
    virtual wxString GetConfigDir() const override;
    virtual wxString GetUserConfigDir() const override;
    virtual wxString GetDataDir() const override;
    virtual wxString GetLocalDataDir() const override;
    virtual wxString GetUserDataDir() const override;
    virtual wxString GetPluginsDir() const override;
    virtual wxString GetDocumentsDir() const override;
    virtual wxString GetTempDir() const override;
    virtual wxString GetUserDir(Dir userDir) const override;
    virtual wxString MakeConfigFileName(const wxString& basename,
                                        ConfigFileConv conv = ConfigFileConv_Ext
                                        ) const override;

protected:
    // Ctor is protected, use wxStandardPaths::Get() instead of instantiating
    // objects of this class directly.
    wxStandardPaths() = default;
};

#endif // _WX_RETRO68_STDPATHS_H_
