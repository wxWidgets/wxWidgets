/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/dirdlg.mm
// Purpose:     wxDirDialog  
// Author:      Stefan Csomor
// Modified by: 
// Created:     2008-08-30
// RCS-ID:      $Id: dirdlg.mm 40007 2006-07-05 13:10:46Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DIRDLG && !defined(__WXUNIVERSAL__)

#include "wx/dirdlg.h"

#ifndef WX_PRECOMP
    #include "wx/msgdlg.h"
    #include "wx/filedlg.h"
    #include "wx/app.h"
#endif

#include "wx/filename.h"

#include "wx/osx/private.h"

IMPLEMENT_CLASS(wxDirDialog, wxDialog)

wxDirDialog::wxDirDialog(wxWindow *parent, const wxString& message,
        const wxString& defaultPath, long style, const wxPoint& pos,
        const wxSize& size, const wxString& name)
{
    m_parent = parent;

    SetMessage( message );
    SetWindowStyle(style);
    SetPath(defaultPath);
}


int wxDirDialog::ShowModal()
{
    int result = wxID_CANCEL;
    
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
    [oPanel setCanChooseDirectories:YES];
    [oPanel setResolvesAliases:YES];
    [oPanel setCanChooseFiles:NO];
    
    wxCFStringRef cf( m_message );
    [oPanel setMessage:cf.AsNSString()];
 
    if ( HasFlag(wxDD_NEW_DIR_BUTTON) ) 
        [oPanel setCanCreateDirectories:YES];

    wxCFStringRef dir( m_path );
    
    m_path = wxEmptyString;

    wxNonOwnedWindow* parentWindow = NULL;
    int returnCode = -1;
    
    if (GetParent()) 
    {
        parentWindow = dynamic_cast<wxNonOwnedWindow*>(wxGetTopLevelParent(GetParent()));
    }
    else
    {
        fprintf(stderr, "No parent!\n");
    }
    
    if (parentWindow)
    {
        NSWindow* nativeParent = parentWindow->GetWXWindow();
        ModalDialogDelegate* sheetDelegate = [[ModalDialogDelegate alloc] init]; 
        [oPanel beginSheetForDirectory:dir.AsNSString() file:nil types: nil 
            modalForWindow: nativeParent modalDelegate: sheetDelegate 
            didEndSelector: @selector(sheetDidEnd:returnCode:contextInfo:) 
            contextInfo: nil];
        [sheetDelegate waitForSheetToFinish];
        returnCode = [sheetDelegate code];
        [sheetDelegate release];
    }
    else
    {
        returnCode = (NSInteger)[oPanel runModalForDirectory:dir.AsNSString() file:nil types:nil];
    }
    if (returnCode == NSOKButton )
    {
        wxCFStringRef resultpath( [[[oPanel filenames] objectAtIndex:0] retain] );
        
        SetPath( resultpath.AsString() );
        result = wxID_OK;
    }
    return result;
}

#endif // wxUSE_DIRDLG
