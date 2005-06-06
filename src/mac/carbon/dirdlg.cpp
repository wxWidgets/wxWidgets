/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.cpp
// Purpose:     wxDirDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dirdlg.h"
#endif

#include "wx/wxprec.h"

#if wxUSE_DIRDLG

#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/dirdlg.h"

#include "wx/cmndata.h"

#include "wx/mac/private.h"

#ifdef __DARWIN__
  #include <Carbon/Carbon.h>
#else
  #include <Navigation.h>
#endif

IMPLEMENT_CLASS(wxDirDialog, wxDialog)

wxDirDialog::wxDirDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& WXUNUSED(pos),
                         const wxSize& WXUNUSED(size),
                         const wxString& WXUNUSED(name))
{
    wxASSERT_MSG( NavServicesAvailable() , wxT("Navigation Services are not running") ) ;
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
    m_path = defaultPath;
}

int wxDirDialog::ShowModal()
{
    NavDialogOptions        mNavOptions;
    NavObjectFilterUPP      mNavFilterUPP = NULL;
    NavReplyRecord          mNavReply;

    ::NavGetDefaultDialogOptions(&mNavOptions);

    mNavReply.validRecord              = false;
    mNavReply.replacing                = false;
    mNavReply.isStationery             = false;
    mNavReply.translationNeeded        = false;
    mNavReply.selection.descriptorType = typeNull;
    mNavReply.selection.dataHandle     = nil;
    mNavReply.keyScript                = smSystemScript;
    mNavReply.fileTranslation          = nil;

    // Set default location, the location
    //   that's displayed when the dialog
    //   first appears

    OSErr err = ::NavChooseFolder(
                        NULL,
                        &mNavReply,
                        &mNavOptions,
                        NULL,
                        mNavFilterUPP,
                        0L);                            // User Data

    if ( (err != noErr) && (err != userCanceledErr) ) {
        m_path = wxEmptyString ;
        return wxID_CANCEL ;
    }

    if (mNavReply.validRecord) {        // User chose a folder

        FSRef folderInfo;
        AEDesc specDesc ;

        OSErr err = ::AECoerceDesc( &mNavReply.selection , typeFSRef, &specDesc);
        if ( err != noErr ) {
            m_path = wxEmptyString ;
            return wxID_CANCEL ;
        }
        folderInfo = **(FSRef**) specDesc.dataHandle;
        if (specDesc.dataHandle != nil) {
            ::AEDisposeDesc(&specDesc);
        }

        m_path = wxMacFSRefToPath( &folderInfo ) ;
        return wxID_OK ;
    }
    return wxID_CANCEL;
}

#endif
