/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/dir.cpp
// Purpose:     wxDir implementation for classic Mac OS using File Manager
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // PCH

#include "wx/dir.h"
#include "wx/filefn.h"          // for wxMatchWild

#include "wx/retro68/private.h"

#include <string.h>

#define M_DIR       ((wxDirData *)m_data)

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

namespace
{

// Finder flag indicating that the file or folder is invisible: we consider
// such objects to be "hidden".
const unsigned short wxMAC_FINDER_INVISIBLE = 0x4000;

} // anonymous namespace

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this class stores everything we need to enumerate the files
class wxDirData
{
public:
    explicit wxDirData(const wxString& dirname);

    bool IsOk() const { return m_ok; }

    void SetFileSpec(const wxString& filespec) { m_filespec = filespec; }
    void SetFlags(int flags) { m_flags = flags; }

    void Rewind() { m_index = 1; }
    bool Read(wxString *filename);

    const wxString& GetName() const { return m_dirname; }

private:
    wxString m_dirname;
    wxString m_filespec;

    short m_vRefNum = 0;
    long m_dirID = 0;
    short m_index = 1;

    int m_flags = wxDIR_DEFAULT;

    bool m_ok = false;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDirData
// ----------------------------------------------------------------------------

wxDirData::wxDirData(const wxString& dirname)
         : m_dirname(dirname)
{
    wxCHECK_RET( !m_dirname.empty(), wxT("empty dir name in wxDir") );

    FSSpec spec;
    OSErr err = wxMacPathToFSSpec(m_dirname, &spec);
    if ( err != noErr )
        return;

    CInfoPBRec pb;
    Str255 name;
    err = wxMacGetCatInfo(spec, pb, name);
    if ( err != noErr || !(pb.dirInfo.ioFlAttrib & wxMAC_IOFL_ATTRIB_DIR) )
        return;

    m_vRefNum = spec.vRefNum;
    m_dirID = pb.dirInfo.ioDrDirID;

    // Ensure that the name has a trailing separator: unlike under Unix, this
    // is the usual way to refer to the directories under Mac OS and is
    // required to distinguish relative paths of the form ":dir:" from just
    // "dir", which is a file name.
    if ( m_dirname.Last() != wxFILE_SEP_PATH )
        m_dirname += wxFILE_SEP_PATH;

    m_ok = true;
}

bool wxDirData::Read(wxString *filename)
{
    CInfoPBRec pb;
    Str255 name;

    for ( ;; )
    {
        memset(&pb, 0, sizeof(pb));
        name[0] = 0;
        pb.hFileInfo.ioNamePtr = name;
        pb.hFileInfo.ioVRefNum = m_vRefNum;
        pb.hFileInfo.ioDirID = m_dirID;
        pb.hFileInfo.ioFDirIndex = m_index++;

        if ( PBGetCatInfoSync(&pb) != noErr )
        {
            // This includes fnfErr returned when there are no more files.
            return false;
        }

        const bool isDir = (pb.hFileInfo.ioFlAttrib & wxMAC_IOFL_ATTRIB_DIR) != 0;
        if ( isDir )
        {
            if ( !(m_flags & wxDIR_DIRS) )
                continue;
        }
        else
        {
            if ( !(m_flags & wxDIR_FILES) )
                continue;
        }

        const unsigned short finderFlags = isDir
            ? pb.dirInfo.ioDrUsrWds.frFlags
            : pb.hFileInfo.ioFlFndrInfo.fdFlags;
        const bool isHidden = (finderFlags & wxMAC_FINDER_INVISIBLE) != 0;

        if ( isHidden && !(m_flags & wxDIR_HIDDEN) )
            continue;

        const wxString fname = wxMacMakeStringFromPascal(name);

        if ( !m_filespec.empty() && !wxMatchWild(m_filespec, fname, false) )
            continue;

        *filename = fname;
        return true;
    }
}

// ----------------------------------------------------------------------------
// wxDir construction/destruction
// ----------------------------------------------------------------------------

wxDir::wxDir(const wxString& dirname)
{
    m_data = nullptr;

    (void)Open(dirname);
}

bool wxDir::Open(const wxString& dirname)
{
    delete M_DIR;
    m_data = new wxDirData(dirname);

    if ( !M_DIR->IsOk() )
    {
        delete M_DIR;
        m_data = nullptr;

        return false;
    }

    return true;
}

bool wxDir::IsOpened() const
{
    return m_data != nullptr;
}

wxString wxDir::GetName() const
{
    wxString name;
    if ( m_data )
    {
        name = M_DIR->GetName();

        // Chop off the trailing separator unless this is the root directory
        // of a volume, where it is required as otherwise the path would be
        // interpreted as a relative path to a file.
        if ( name.length() > 1 && name.Last() == wxFILE_SEP_PATH &&
                name.find(wxFILE_SEP_PATH) != name.length() - 1 )
        {
            name.RemoveLast();
        }
    }

    return name;
}

void wxDir::Close()
{
    if ( m_data )
    {
        delete M_DIR;
        m_data = nullptr;
    }
}

// ----------------------------------------------------------------------------
// wxDir enumerating
// ----------------------------------------------------------------------------

bool wxDir::GetFirst(wxString *filename,
                     const wxString& filespec,
                     int flags) const
{
    wxCHECK_MSG( IsOpened(), false, wxT("must wxDir::Open() first") );

    M_DIR->Rewind();

    M_DIR->SetFileSpec(filespec);
    M_DIR->SetFlags(flags);

    return GetNext(filename);
}

bool wxDir::GetNext(wxString *filename) const
{
    wxCHECK_MSG( IsOpened(), false, wxT("must wxDir::Open() first") );

    wxCHECK_MSG( filename, false, wxT("bad pointer in wxDir::GetNext()") );

    return M_DIR->Read(filename);
}
