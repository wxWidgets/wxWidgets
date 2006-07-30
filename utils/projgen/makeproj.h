/////////////////////////////////////////////////////////////////////////////
// Name:        makeproj.h
// Purpose:     Generate sample VC++ project files
// Author:      Julian Smart
// Modified by:
// Created:     10/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "makeproj.h"
#endif

#ifndef _MAKEPROJ_H_
#define _MAKEPROJ_H_

class wxProject: public wxObject
{
public:
    wxProject();
    ~wxProject();

    bool GenerateVCProject();

    void SetProjectName(const wxString& projectName) { m_projectName = projectName; }
    void SetTargetName(const wxString& targetName) { m_targetName = targetName; }
    void SetProjectPath(const wxString& path) { m_path = path; }
    void SetSourceFiles(const wxStringList& sources) { m_sourceFiles = sources; }
//    void SetHeaders(const wxStringList& headers) { m_headerFiles = headers; }
    void SetIncludeDirs(const wxStringList& dirs) { m_includeDirs = dirs; }
    void SetResourceIncludeDirs(const wxStringList& dirs) { m_resourceIncludeDirs = dirs; }
    void SetLibDirs(const wxStringList& dirs) { m_libDirs = dirs; }
    void SetDebugLibDirs(const wxStringList& dirs) { m_debugLibDirs = dirs; }
    void SetReleaseLibDirs(const wxStringList& dirs) { m_releaseLibDirs = dirs; }
    void SetExtraLibsDebug(const wxStringList& libs) { m_extraLibsDebug = libs; }
    void SetExtraLibsRelease(const wxStringList& libs) { m_extraLibsRelease = libs; }

    inline wxString GetProjectName() const { return m_projectName; }
    inline wxString GetTargetName() const { return m_targetName; }
    inline wxString GetPath() const { return m_path; }
    inline wxStringList GetSourceFiles() const { return m_sourceFiles; }
//    inline wxStringList GetHeaders() const { return m_headerFiles; }
    inline wxStringList GetIncludeDirs() const { return m_includeDirs; }
    inline wxStringList GetResourceIncludeDirs() const { return m_resourceIncludeDirs; }
    inline wxStringList GetLibDirs() const { return m_libDirs; }
    inline wxStringList GetDebugLibDirs() const { return m_debugLibDirs; }
    inline wxStringList GetReleaseLibDirs() const { return m_releaseLibDirs; }
    inline wxStringList GetExtraLibsDebug() const { return m_extraLibsDebug; }
    inline wxStringList GetExtraLibsRelease() const { return m_extraLibsRelease; }

protected:
    wxString        m_projectName;
    wxString        m_targetName;
    wxString        m_path;
    wxStringList    m_sourceFiles;
//    wxStringList    m_headerFiles;
    wxStringList    m_includeDirs;
    wxStringList    m_resourceIncludeDirs;
    wxStringList    m_libDirs;
    wxStringList    m_debugLibDirs;
    wxStringList    m_releaseLibDirs;
    wxStringList    m_extraLibsDebug;
    wxStringList    m_extraLibsRelease;
};


#endif
    // _MAKEPROJ_H_
