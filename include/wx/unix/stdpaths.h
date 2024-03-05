///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/stdpaths.h
// Purpose:     wxStandardPaths for Unix systems
// Author:      Vadim Zeitlin
// Created:     2004-10-19
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_STDPATHS_H_
#define _WX_UNIX_STDPATHS_H_

// ----------------------------------------------------------------------------
// wxStandardPaths
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPaths : public wxStandardPathsBase
{
public:
    // tries to determine the installation prefix automatically (Linux only right
    // now) and returns /usr/local if it failed
    void DetectPrefix();

    // set the program installation directory which is /usr/local by default
    //
    // under some systems (currently only Linux) the program directory can be
    // determined automatically but for portable programs you should always set
    // it explicitly
    void SetInstallPrefix(const wxString& prefix);

    // get the program installation prefix
    //
    // if the prefix had been previously by SetInstallPrefix, returns that
    // value, otherwise calls DetectPrefix()
    wxString GetInstallPrefix() const;


    // implement base class pure virtuals
    virtual wxString GetExecutablePath() const override;
    virtual wxString GetConfigDir() const override;
    virtual wxString GetUserConfigDir() const override;
    virtual wxString GetDataDir() const override;
    virtual wxString GetLocalDataDir() const override;
    virtual wxString GetUserDataDir() const override;
    virtual wxString GetPluginsDir() const override;
    virtual wxString GetLocalizedResourcesDir(const wxString& lang,
                                              ResourceCat category) const override;
    virtual wxString GetSharedLibrariesDir() const override;
#ifndef __VMS
    virtual wxString GetUserDir(Dir userDir) const override;
#endif
    virtual wxString MakeConfigFileName(const wxString& basename,
                                        ConfigFileConv conv = ConfigFileConv_Ext
                                        ) const override;

protected:
    // Ctor is protected, use wxStandardPaths::Get() instead of instantiating
    // objects of this class directly.
    wxStandardPaths() = default;

private:
    wxString m_prefix;
};

#endif // _WX_UNIX_STDPATHS_H_

