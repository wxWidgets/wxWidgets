/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/dirdlg.cpp
// Purpose:     wxDirDialog implementation using Standard File Package
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DIRDLG

#include "wx/dirdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#include "wx/modalhook.h"

#include "wx/retro68/private/qd.h"
#include "wx/retro68/dc.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxDirDialog, wxDialog);

int wxDirDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

#if !TARGET_API_MAC_CARBON
    // Start in the specified directory, if any.
    if ( !m_path.empty() )
    {
        wxString path(m_path);
        if ( path.Last() != wxFILE_SEP_PATH )
            path += wxFILE_SEP_PATH;

        FSSpec spec;
        CInfoPBRec pb;
        Str255 name;
        if ( wxMacPathToFSSpec(path, &spec) == noErr &&
                wxMacGetCatInfo(spec, pb, name) == noErr &&
                    (pb.dirInfo.ioFlAttrib & wxMAC_IOFL_ATTRIB_DIR) )
        {
            LMSetSFSaveDisk(-spec.vRefNum);
            LMSetCurDirStore(pb.dirInfo.ioDrDirID);
        }
    }
#endif // !TARGET_API_MAC_CARBON

    wxRetro68DCImpl::InvalidateCachedPortState();

    Str255 prompt, defaultName;
    wxMacStringToPascal(m_message.empty() ? wxString(_("Choose a folder:"))
                                          : m_message,
                        prompt);
    wxMacStringToPascal(_("Select This Folder"), defaultName);

    StandardFileReply reply;
    StandardPutFile(prompt, defaultName, &reply);

    wxRetro68DCImpl::InvalidateCachedPortState();

    if ( !reply.sfGood )
        return wxID_CANCEL;

    // We're only interested in the folder in which the dummy file would have
    // been saved.
    m_path = wxMacDirToPath(reply.sfFile.vRefNum, reply.sfFile.parID);
    if ( m_path.empty() )
        return wxID_CANCEL;

    // Remove the trailing separator unless it's the volume root.
    if ( m_path.Last() == wxFILE_SEP_PATH &&
            m_path.find(wxFILE_SEP_PATH) != m_path.length() - 1 )
    {
        m_path.RemoveLast();
    }

    m_paths.clear();
    m_paths.push_back(m_path);

    return wxID_OK;
}

#endif // wxUSE_DIRDLG
