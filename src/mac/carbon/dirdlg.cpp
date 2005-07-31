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
    NavDialogRef dialog;
    NavDialogCreationOptions options;
    NavReplyRecord reply ;
    bool disposeReply = false ;
    OSStatus err = noErr;
    
    err = NavGetDefaultDialogCreationOptions(&options);
    if (err == noErr) 
    {
        wxMacCFStringHolder message(m_message, m_font.GetEncoding());
        options.message = message;
        err = NavCreateChooseFolderDialog(&options, NULL, NULL, NULL, &dialog);
        if (err == noErr) 
        {        
            err = NavDialogRun(dialog);
            if ( err == noErr )
            {
                err = NavDialogGetReply(dialog, &reply);
                disposeReply = true ;
            }
        }
    }
    
    if ( err == noErr ) 
    { 
        if ( reply.validRecord )
        {
            FSRef folderInfo;
            AEDesc specDesc ;
            
            OSErr err = ::AECoerceDesc( &reply.selection , typeFSRef, &specDesc);
            if ( err != noErr ) 
            {
                m_path = wxEmptyString ;
            }
            else
            {
                folderInfo = **(FSRef**) specDesc.dataHandle;
                m_path = wxMacFSRefToPath( &folderInfo ) ;
                if (specDesc.dataHandle != nil) 
                {
                    ::AEDisposeDesc(&specDesc);
                }            
            }
        }
        else
        {
            err = paramErr ; // could be any error, only used for giving back wxID_CANCEL
        }
    }
    
    if ( disposeReply )
        ::NavDisposeReply(&reply);
    
    // apparently cancelling shouldn't change m_path
    if ( err != noErr && err != userCanceledErr )
        m_path = wxEmptyString ;
    
    return (err == noErr) ? wxID_OK : wxID_CANCEL ;
}

#endif
