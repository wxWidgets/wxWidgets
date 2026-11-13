/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/filedlg.cpp
// Purpose:     wxFileDialog implementation using Standard File Package
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FILEDLG

#include "wx/filedlg.h"

#ifndef WX_PRECOMP
    #include "wx/filefn.h"
    #include "wx/tokenzr.h"
#endif

#include "wx/filename.h"
#include "wx/modalhook.h"

#include "wx/retro68/private/qd.h"
#include "wx/retro68/dc.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxFileDialog, wxFileDialogBase);

namespace
{

// Wildcard patterns used by the file filter callback.
wxArrayString gs_patterns;

// Return true if the file should be excluded from the list.
pascal Boolean FilterFilesByName(CInfoPBPtr pb)
{
    // Never filter out the directories.
    if ( pb->hFileInfo.ioFlAttrib & wxMAC_IOFL_ATTRIB_DIR )
        return false;

    if ( gs_patterns.empty() )
        return false;

    const wxString name = wxMacMakeStringFromPascal(pb->hFileInfo.ioNamePtr);
    for ( const wxString& pattern : gs_patterns )
    {
        if ( wxMatchWild(pattern, name, false) )
            return false;
    }

    return true;
}

// Make the given directory the one initially shown in Standard File dialogs.
void SetInitialDirectory(const wxString& dir)
{
    if ( dir.empty() )
        return;

    wxString path(dir);
    if ( path.Last() != wxFILE_SEP_PATH )
        path += wxFILE_SEP_PATH;

    FSSpec spec;
    if ( wxMacPathToFSSpec(path, &spec) != noErr )
        return;

    CInfoPBRec pb;
    Str255 name;
    if ( wxMacGetCatInfo(spec, pb, name) != noErr ||
            !(pb.dirInfo.ioFlAttrib & wxMAC_IOFL_ATTRIB_DIR) )
        return;

#if !TARGET_API_MAC_CARBON
    // This is the documented way to set the directory used by the Standard
    // File Package.
    LMSetSFSaveDisk(-spec.vRefNum);
    LMSetCurDirStore(pb.dirInfo.ioDrDirID);
#endif
}

} // anonymous namespace

int wxFileDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    m_path.clear();
    m_fileName.clear();

    SetInitialDirectory(m_dir);

    // The native dialog uses its own event loop, so forget about the
    // currently selected port.
    wxRetro68DCImpl::InvalidateCachedPortState();

    StandardFileReply reply;

    if ( HasFdFlag(wxFD_SAVE) )
    {
        Str255 prompt, defaultName;
        wxMacStringToPascal(m_message.empty() ? wxString(_("Save as:"))
                                              : m_message,
                            prompt);
        wxMacStringToPascal(m_fileName, defaultName);

        StandardPutFile(prompt, defaultName, &reply);
    }
    else
    {
        // Use the patterns of the selected filter, if any.
        gs_patterns.clear();

        wxArrayString descriptions, filters;
        if ( wxParseCommonDialogsFilter(m_wildCard, descriptions, filters) )
        {
            const int index = m_filterIndex >= 0 &&
                                m_filterIndex < static_cast<int>(filters.size())
                                    ? m_filterIndex : 0;

            wxStringTokenizer tk(filters[index], ";");
            while ( tk.HasMoreTokens() )
            {
                const wxString pattern = tk.GetNextToken().Strip(wxString::both);
                if ( pattern == "*" || pattern == "*.*" )
                {
                    // Show all files.
                    gs_patterns.clear();
                    break;
                }

                if ( !pattern.empty() )
                    gs_patterns.push_back(pattern);
            }
        }

        static FileFilterUPP s_filterUPP = nullptr;
        if ( !s_filterUPP )
            s_filterUPP = NewFileFilterUPP(FilterFilesByName);

        StandardGetFile(gs_patterns.empty() ? nullptr : s_filterUPP,
                        -1, nullptr, &reply);

        gs_patterns.clear();
    }

    wxRetro68DCImpl::InvalidateCachedPortState();

    if ( !reply.sfGood )
        return wxID_CANCEL;

    m_path = wxMacFSSpecToPath(reply.sfFile);

    wxFileName fn(m_path);
    m_dir = fn.GetPath();
    m_fileName = fn.GetFullName();

    return wxID_OK;
}

void wxFileDialog::GetPaths(wxArrayString& paths) const
{
    paths.clear();
    if ( !m_path.empty() )
        paths.push_back(m_path);
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{
    files.clear();
    if ( !m_fileName.empty() )
        files.push_back(m_fileName);
}

#endif // wxUSE_FILEDLG
