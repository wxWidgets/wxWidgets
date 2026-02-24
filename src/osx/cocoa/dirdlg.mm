/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/dirdlg.mm
// Purpose:     wxDirDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     2008-08-30
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
#include "wx/evtloop.h"
#include "wx/modalhook.h"
#include "wx/generic/dirctrlg.h" // for wxFileIconsTable

#include "wx/osx/private.h"

wxIMPLEMENT_CLASS(wxDirDialog, wxDialog);

void wxDirDialog::Init()
{
}

void wxDirDialog::Create(wxWindow *parent, const wxString& message,
        const wxString& defaultPath, long style, const wxPoint& WXUNUSED(pos),
        const wxSize& WXUNUSED(size), const wxString& WXUNUSED(name))
{
    m_parent = parent;

    wxASSERT_MSG( !( (style & wxDD_MULTIPLE) && (style & wxDD_CHANGE_DIR) ),
                  "wxDD_CHANGE_DIR can't be used together with wxDD_MULTIPLE" );

    SetMessage( message );
    SetWindowStyle(style);
    SetPath(defaultPath);
}

wxDirDialog::~wxDirDialog()
{
}

WX_NSOpenPanel wxDirDialog::OSXCreatePanel() const
{
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
    [oPanel setCanChooseDirectories:YES];
    [oPanel setResolvesAliases:YES];
    [oPanel setCanChooseFiles:NO];

    wxCFStringRef cf( m_message );
    [oPanel setMessage:cf.AsNSString()];

    if ( !m_title.empty() )
    {
        wxCFStringRef cfTitle(m_title);
        [oPanel setTitle:cfTitle.AsNSString()];
    }

    if ( !HasFlag(wxDD_DIR_MUST_EXIST) )
        [oPanel setCanCreateDirectories:YES];

    if ( HasFlag(wxDD_MULTIPLE) )
        [oPanel setAllowsMultipleSelection:YES];

    if ( HasFlag(wxDD_SHOW_HIDDEN) )
        [oPanel setShowsHiddenFiles:YES];

    // Set the directory to use
    if ( !m_path.IsEmpty() )
    {
        wxCFStringRef dir(m_path);
        NSURL* dirUrl = [NSURL fileURLWithPath: dir.AsNSString() isDirectory: YES];
        [oPanel setDirectoryURL: dirUrl];
    }

    return oPanel;
}

void wxDirDialog::ShowWindowModal()
{
    wxNonOwnedWindow* parentWindow = NULL;

    if (GetParent())
        parentWindow = dynamic_cast<wxNonOwnedWindow*>(wxGetTopLevelParent(GetParent()));

    wxCHECK_RET(parentWindow, "Window modal display requires parent.");

    m_modality = wxDIALOG_MODALITY_WINDOW_MODAL;

    NSOpenPanel *oPanel = OSXCreatePanel();

    NSWindow* nativeParent = parentWindow->GetWXWindow();

    // Create the window and have it call the ModalFinishedCallback on completion
    [oPanel beginSheetModalForWindow: nativeParent completionHandler: ^(NSModalResponse returnCode){
        this->ModalFinishedCallback(oPanel, returnCode);
    }];
}

int wxDirDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxCFEventLoopPauseIdleEvents pause;

    NSOpenPanel *oPanel = OSXCreatePanel();

    OSXBeginModalDialog();

    // Display the panel and process the result on completion
    int returnCode = (NSInteger)[oPanel runModal];
    ModalFinishedCallback(oPanel, returnCode);

    OSXEndModalDialog();


    return GetReturnCode();
}

// Bricsys added
int wxDirDialog::ShowModalChooseApp()
{
    WX_HOOK_MODAL_DIALOG();
    
    wxCFEventLoopPauseIdleEvents pause;
    
    NSOpenPanel* p_Panel = OSXCreatePanel();
    [p_Panel setPrompt:@"Select App"];
    [p_Panel setResolvesAliases:NO];
    [p_Panel setCanChooseFiles:NO];
    [p_Panel setAllowedFileTypes:[[NSArray alloc] initWithObjects:@"app",@"APP"]];
    [p_Panel setTreatsFilePackagesAsDirectories:YES];
    
    wxCFStringRef dir(@"/Applications");
    
    CFURLRef cfURLCurrentFile = (CFURLRef)[NSURL fileURLWithPath:wxNSStringWithWxString(m_path)];
    
    m_path.clear();
    
    int returnCode = -1;
    
    OSXBeginModalDialog();
    
    returnCode = (NSInteger)[p_Panel runModalForDirectory:dir.AsNSString() file:nil types:nil];
    ModalFinishedCallback(p_Panel, returnCode);
    
    OSXEndModalDialog();
    
    if(returnCode)
    {
        NSArray<NSURL *>* p_URLs = [p_Panel URLs];

        NSString* p_SelectedAppURL = [p_URLs[0] path];

        CFURLRef appURL = (CFURLRef)[NSURL fileURLWithPath:p_SelectedAppURL];

        CFMutableArrayRef cfFiles = CFArrayCreateMutable(kCFAllocatorDefault, 1, &kCFTypeArrayCallBacks);
        CFArrayAppendValue(cfFiles,cfURLCurrentFile);

        LSLaunchURLSpec launchSpec;
        launchSpec.appURL = appURL;
        launchSpec.itemURLs = cfFiles;
        launchSpec.passThruParams = nullptr;
        launchSpec.launchFlags = kLSLaunchDefaults;
        launchSpec.asyncRefCon = nullptr;

        LSOpenFromURLSpec(&launchSpec, nullptr);
    }
    
    return GetReturnCode();
}
// end Bricsys added

void wxDirDialog::ModalFinishedCallback(void* panel, int returnCode)
{
    int result = wxID_CANCEL;

    if (returnCode == NSModalResponseOK )
    {
        NSOpenPanel* oPanel = (NSOpenPanel*)panel;

        NSArray<NSURL*>* selectedURL = [oPanel URLs];

        for ( NSURL* url in selectedURL )
        {
            NSString* unsafePath = [NSString stringWithUTF8String:[url fileSystemRepresentation]];
            wxCFStringRef safePath([[unsafePath precomposedStringWithCanonicalMapping] retain]);
            m_paths.Add(safePath.AsString());
        }

        if ( !HasFlag(wxDD_MULTIPLE) )
        {
            m_path = m_paths.Last();
        }

        result = wxID_OK;
    }
    SetReturnCode(result);

    if (GetModality() == wxDIALOG_MODALITY_WINDOW_MODAL)
        SendWindowModalDialogEvent ( wxEVT_WINDOW_MODAL_DIALOG_CLOSED  );
}

void wxDirDialog::SetTitle(const wxString &title)
{
    m_title = title;
    wxDialog::SetTitle(title);
}

#endif // wxUSE_DIRDLG
