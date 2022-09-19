/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/filedlg.mm
// Purpose:     wxFileDialog for wxCocoa
// Author:      Ryan Norton
// Modified by:
// Created:     2004-10-02
// Copyright:   (c) Ryan Norton
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

#if wxUSE_FILEDLG

#include "wx/filedlg.h"

#ifndef WX_PRECOMP
    #include "wx/msgdlg.h"
    #include "wx/app.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
    #include "wx/choice.h"
#endif

#include "wx/filename.h"
#include "wx/tokenzr.h"
#include "wx/evtloop.h"

#include "wx/osx/private.h"
#include "wx/sysopt.h"
#include "wx/modalhook.h"

#include <mach-o/dyld.h>

// ============================================================================
// delegate for filtering by wildcard
// ============================================================================

@interface wxOpenSavePanelDelegate : NSObject<NSOpenSavePanelDelegate>

- (void)setAllowedExtensions:(const wxArrayString &)extensions;

@end

@implementation wxOpenSavePanelDelegate
{
    wxArrayString m_extensions;
}

- (BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url
{
    if ( [url isFileURL] )
    {
        NSString* filename = [url path];
        NSString* resolvedLink = [[NSFileManager defaultManager] destinationOfSymbolicLinkAtPath:filename error:nil];

        if ( resolvedLink != nil )
            filename = resolvedLink;

        BOOL isDir = NO;
        if( [[NSFileManager defaultManager]
            fileExistsAtPath:filename isDirectory:&isDir] && isDir )
        {
            // allow ordinary folders to be enabled, but for packages apply our extensions check

            if ([[NSWorkspace sharedWorkspace] isFilePackageAtPath:filename] == NO)
                return YES;    // it's a folder, OK to show
        }

        if ( m_extensions.GetCount() == 0 )
            return YES;

        NSString *ext = [filename pathExtension];
        wxString wxext = wxCFStringRef([ext retain]).AsString().Lower();

        for( const wxString& extension : m_extensions )
        {
            if( wxext == extension )
                return YES;
        }
    }
    return NO;
}

- (void)setAllowedExtensions:(const wxArrayString &)extensions
{
    m_extensions = extensions;
}

@end

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase);

void wxFileDialog::Init()
{
    m_filterIndex = -1;
    m_delegate = nil;
    m_filterPanel = NULL;
    m_filterChoice = NULL;
    m_useFileTypeFilter = false;
    m_firstFileTypeFilter = 0;
}

void wxFileDialog::Create(
    wxWindow *parent, const wxString& message,
    const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
    long style, const wxPoint& pos, const wxSize& sz, const wxString& name)
{
    wxFileDialogBase::Create(parent, message, defaultDir, defaultFileName, wildCard, style, pos, sz, name);

}

wxFileDialog::~wxFileDialog()
{
}

bool wxFileDialog::SupportsExtraControl() const
{
    return true;
}

NSArray* GetTypesFromExtension( const wxString extensiongroup, wxArrayString& extensions )
{
    NSMutableArray* types = nil;
    extensions.Clear();

    wxStringTokenizer tokenizer( extensiongroup, wxT(";") ) ;
    while ( tokenizer.HasMoreTokens() )
    {
        wxString extension = tokenizer.GetNextToken() ;
        // Remove leading '*'
        if ( extension.length() && (extension.GetChar(0) == '*') )
            extension = extension.Mid( 1 );

        // Remove leading '.'
        if ( extension.length() && (extension.GetChar(0) == '.') )
            extension = extension.Mid( 1 );

        // Remove leading '*', this is for handling *.*
        if ( extension.length() && (extension.GetChar(0) == '*') )
            extension = extension.Mid( 1 );

        if ( extension.IsEmpty() )
        {
            extensions.Clear();
            [types release];
            types = nil;
            return nil;
        }

        if ( types == nil )
            types = [[NSMutableArray alloc] init];

        extensions.Add(extension.Lower());
        wxCFStringRef cfext(extension);
        [types addObject: (NSString*)cfext.AsNSString()  ];
    }
    [types autorelease];
    return types;
}

NSArray* GetTypesFromFilter( const wxString& filter, wxArrayString& names, wxArrayString& extensiongroups, wxArrayString& allextensions )
{
    NSMutableArray* types = nil;
    bool allowAll = false;

    names.Clear();
    extensiongroups.Clear();
    allextensions.Clear();

    if ( !filter.empty() )
    {
        wxStringTokenizer tokenizer( filter, wxT("|") );
        int numtokens = (int)tokenizer.CountTokens();
        if(numtokens == 1)
        {
            // we allow for compatibility reason to have a single filter expression (like *.*) without
            // an explanatory text, in that case the first part is name and extension at the same time
            wxString extension = tokenizer.GetNextToken();
            names.Add( extension );
            extensiongroups.Add( extension );
        }
        else
        {
            int numextensions = numtokens / 2;
            for(int i = 0; i < numextensions; i++)
            {
                wxString name = tokenizer.GetNextToken();
                wxString extension = tokenizer.GetNextToken();
                names.Add( name );
                extensiongroups.Add( extension );
            }
        }

        const size_t extCount = extensiongroups.GetCount();
        wxArrayString extensions;
        for ( size_t i = 0 ; i < extCount; i++ )
        {
            NSArray* exttypes = GetTypesFromExtension(extensiongroups[i], extensions);
            if ( exttypes != nil )
            {
                if ( allowAll == false )
                {
                    if ( types == nil )
                        types = [[NSMutableArray alloc] init];

                    [types addObjectsFromArray:exttypes];
                    for( auto const& s : extensions )
                        allextensions.Add(s);
                }
            }
            else
            {
                allowAll = true;
                [types release];
                types = nil;
            }
        }
        if ( allowAll )
            allextensions.Clear();
    }
    [types autorelease];
    return types;
}

void wxFileDialog::ShowWindowModal()
{
    wxCFStringRef cf( m_message );
    wxCFStringRef dir( m_dir );
    wxCFStringRef file( m_fileName );

    wxNonOwnedWindow* parentWindow = NULL;
    
    m_modality = wxDIALOG_MODALITY_WINDOW_MODAL;

    if (GetParent())
        parentWindow = dynamic_cast<wxNonOwnedWindow*>(wxGetTopLevelParent(GetParent()));

    wxCHECK_RET(parentWindow, "Window modal display requires parent.");

    NSArray* types = GetTypesFromFilter( m_wildCard, m_filterNames, m_filterExtensions, m_currentExtensions ) ;
    if ( HasFlag(wxFD_SAVE) )
    {
        NSSavePanel* sPanel = [NSSavePanel savePanel];

        SetupExtraControls(sPanel);

        // makes things more convenient:
        [sPanel setCanCreateDirectories:YES];
        [sPanel setMessage:cf.AsNSString()];
        // if we should be able to descend into pacakges we must somehow
        // be able to pass this in
        [sPanel setTreatsFilePackagesAsDirectories:NO];
        [sPanel setCanSelectHiddenExtension:YES];
        [sPanel setAllowedFileTypes:types];
        [sPanel setAllowsOtherFileTypes:NO];
        [sPanel setShowsHiddenFiles: HasFlag(wxFD_SHOW_HIDDEN) ? YES : NO];

        NSWindow* nativeParent = parentWindow->GetWXWindow();
        if ( !m_dir.IsEmpty() )
            [sPanel setDirectoryURL:[NSURL fileURLWithPath:dir.AsNSString()
                                               isDirectory:YES]];
        if ( !m_fileName.IsEmpty() )
            [sPanel setNameFieldStringValue: file.AsNSString()];

        [sPanel beginSheetModalForWindow:nativeParent completionHandler:
         ^(NSModalResponse returnCode)
        {
            this->ModalFinishedCallback(sPanel, returnCode);
        }];
    }
    else 
    {
        NSOpenPanel* oPanel = [NSOpenPanel openPanel];
        
        SetupExtraControls(oPanel);

        [oPanel setTreatsFilePackagesAsDirectories:NO];
        [oPanel setCanChooseDirectories:NO];
        [oPanel setResolvesAliases:HasFlag(wxFD_NO_FOLLOW) ? NO : YES];
        [oPanel setCanChooseFiles:YES];
        [oPanel setMessage:cf.AsNSString()];
        [oPanel setAllowsMultipleSelection: (HasFlag(wxFD_MULTIPLE) ? YES : NO )];
        [oPanel setShowsHiddenFiles: HasFlag(wxFD_SHOW_HIDDEN) ? YES : NO];

        NSWindow* nativeParent = parentWindow->GetWXWindow();
        if ( !m_dir.IsEmpty() )
            [oPanel setDirectoryURL:[NSURL fileURLWithPath:dir.AsNSString()
                                               isDirectory:YES]];
        [oPanel beginSheetModalForWindow:nativeParent completionHandler:
         ^(NSModalResponse returnCode)
        {
            this->ModalFinishedCallback(oPanel, returnCode);
        }];
    }

}

// Fill a new or existing panel with the file type drop down list.
// If extra controls need to be added (see wxFileDialog::SetExtraControlCreator),
// use that as a panel if possible, otherwise add them to a new panel.
// Returns a newly created wxPanel or extracontrol if it's suitable as a filter
// panel.

wxWindow* wxFileDialog::CreateFilterPanel(wxWindow *extracontrol)
{
    // Try to use extracontrol as a filter panel instead of creating a new one
    // and then reparenting extracontrol. Reparenting is less desired as user
    // code may expect the parent to be a wxFileDialog as on other platforms.
    const bool useExtraControlAsPanel = extracontrol &&
        wxDynamicCast(extracontrol, wxPanel) != NULL;

    wxWindow* extrapanel = useExtraControlAsPanel
                            ? extracontrol
                            : static_cast<wxWindow*>(new wxPanel(this));

    wxBoxSizer *verticalSizer = new wxBoxSizer(wxVERTICAL);
    
    // the file type control
    {
        wxBoxSizer *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
        verticalSizer->Add(horizontalSizer, 0, wxEXPAND, 0);
        horizontalSizer->AddStretchSpacer();
        wxStaticText *stattext = new wxStaticText( extrapanel, wxID_ANY, _("File type:") );
        horizontalSizer->Add(stattext, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        m_filterChoice = new wxChoice(extrapanel, wxID_ANY);
        horizontalSizer->Add(m_filterChoice, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        horizontalSizer->AddStretchSpacer();
        m_filterChoice->Append(m_filterNames);
        if( m_filterNames.GetCount() > 0)
        {
            if ( m_firstFileTypeFilter >= 0 )
                m_filterChoice->SetSelection(m_firstFileTypeFilter);
        }
        m_filterChoice->Bind(wxEVT_CHOICE, &wxFileDialog::OnFilterSelected, this);
    }
        
    if(extracontrol)
    {
        // Either use an extra control's existing sizer or the extra control
        // itself, to be in the vertical sizer.

        wxSizer* existingSizer = extracontrol->GetSizer();
        if ( useExtraControlAsPanel && existingSizer )
        {
            // Move extra control's sizer to verticalSizer.
            extracontrol->SetSizer(NULL, /* deleteOld = */ false);
            verticalSizer->Add(existingSizer);
        }
        else
        {
            wxBoxSizer* horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
            verticalSizer->Add(horizontalSizer, 0, wxEXPAND, 0);

            if ( !useExtraControlAsPanel )
                extracontrol->Reparent(extrapanel);
            horizontalSizer->Add(extracontrol);
        }
    }

    extrapanel->SetSizer(verticalSizer);

    verticalSizer->Layout();
    verticalSizer->SetSizeHints(extrapanel);
    return extrapanel;
}

void wxFileDialog::DoOnFilterSelected(int index)
{
    NSArray* types = GetTypesFromExtension(m_filterExtensions[index],m_currentExtensions);
    NSSavePanel* panel = (NSSavePanel*) GetWXWindow();
    if ( m_delegate )
    {
        [(wxOpenSavePanelDelegate*)m_delegate setAllowedExtensions:m_currentExtensions];
        [panel validateVisibleColumns];
    }
    else
        [panel setAllowedFileTypes:types];

    m_currentlySelectedFilterIndex = index;

    UpdateExtraControlUI();
}

// An item has been selected in the file filter wxChoice:
void wxFileDialog::OnFilterSelected( wxCommandEvent &WXUNUSED(event) )
{
    DoOnFilterSelected( m_filterChoice->GetSelection() );
}

void wxFileDialog::SetupExtraControls(WXWindow nativeWindow)
{
    NSSavePanel* panel = (NSSavePanel*) nativeWindow;
    // for sandboxed app we cannot access the outer structures
    // this leads to problems with extra controls, so as a temporary
    // workaround for crashes we don't support those yet
    if ( [panel contentView] == nil || getenv("APP_SANDBOX_CONTAINER_ID") != NULL )
        return;
    
    wxNonOwnedWindow::Create( GetParent(), nativeWindow );

    // This won't do anything if there are no extra controls to create and
    // extracontrol will be NULL in this case.
    CreateExtraControl();
    wxWindow* const extracontrol = GetExtraControl();

    NSView* accView = nil;

    if ( m_useFileTypeFilter )
    {
        m_filterPanel = CreateFilterPanel(extracontrol);
        accView = m_filterPanel->GetHandle();
    }
    else
    {
        m_filterPanel = NULL;
        m_filterChoice = NULL;
        if ( extracontrol != nil )
            accView = extracontrol->GetHandle();
    }

    if ( accView != nil )
    {
        [accView removeFromSuperview];
        [panel setAccessoryView:accView];
        if ([panel respondsToSelector:@selector(setAccessoryViewDisclosed)])
        {
            [(id)panel setAccessoryViewDisclosed:YES];
        }
    }
    else
    {
        [panel setAccessoryView:nil];
    }
}

int wxFileDialog::GetMatchingFilterExtension(const wxString& filename)
{
    int index = -1;
    for ( size_t i = 0; i < m_filterExtensions.GetCount(); ++i )
    {
        NSArray* types = GetTypesFromExtension(m_filterExtensions[i], m_currentExtensions);
        wxUnusedVar(types);
        if ( m_currentExtensions.GetCount() == 0 )
        {
            index = i;
            break;
        }

        for ( size_t j = 0; j < m_currentExtensions.GetCount(); ++j )
        {
            if ( filename.EndsWith(m_currentExtensions[j]) )
            {
                index = i;
                break;
            }
        }
        if ( index >= 0 )
            break;
    }
    if ( index == -1 )
        index = 0;

    return index;
}

int wxFileDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxCFEventLoopPauseIdleEvents pause;

    wxMacAutoreleasePool autoreleasepool;
    
    wxCFStringRef cf( m_message );

    wxCFStringRef dir( m_dir );
    wxCFStringRef file( m_fileName );

    m_path.clear();
    m_fileNames.Clear();
    m_paths.Clear();

    int returnCode = -1;

    NSArray* types = GetTypesFromFilter( m_wildCard, m_filterNames, m_filterExtensions, m_currentExtensions ) ;

    m_useFileTypeFilter = m_filterExtensions.GetCount() > 1;

    // native behaviour on macos is to enable just every file type
    // in a file open dialog that could be handled, without a file
    // filter choice control
    // wxOSX_FILEDIALOG_ALWAYS_SHOW_TYPES allows to override
    // that and get the same behaviour as other platforms have ...

    if( HasFlag(wxFD_OPEN) )
    {
        if ( !(wxSystemOptions::HasOption( wxOSX_FILEDIALOG_ALWAYS_SHOW_TYPES ) && (wxSystemOptions::GetOptionInt( wxOSX_FILEDIALOG_ALWAYS_SHOW_TYPES ) == 1)) )
            m_useFileTypeFilter = false;            
    }

    m_firstFileTypeFilter = -1;

    if ( m_useFileTypeFilter
        && m_filterIndex >= 0 && (size_t)m_filterIndex < m_filterExtensions.GetCount() )
    {
        m_firstFileTypeFilter = m_filterIndex;
    }
    else if ( m_useFileTypeFilter )
    {
        m_firstFileTypeFilter = GetMatchingFilterExtension(m_fileName);
        types = GetTypesFromExtension(m_filterExtensions[m_firstFileTypeFilter], m_currentExtensions);
    }

    OSXBeginModalDialog();

    if ( HasFlag(wxFD_SAVE) )
    {
        NSSavePanel* sPanel = [NSSavePanel savePanel];

        SetupExtraControls(sPanel);

        // makes things more convenient:
        [sPanel setCanCreateDirectories:YES];
        [sPanel setMessage:cf.AsNSString()];
        // if we should be able to descend into pacakges we must somehow
        // be able to pass this in
        [sPanel setTreatsFilePackagesAsDirectories:NO];
        [sPanel setCanSelectHiddenExtension:YES];
        [sPanel setAllowedFileTypes:types];
        [sPanel setAllowsOtherFileTypes:NO];
        [sPanel setShowsHiddenFiles: HasFlag(wxFD_SHOW_HIDDEN) ? YES : NO];

        if ( HasFlag(wxFD_OVERWRITE_PROMPT) )
        {
        }

        /*
        Let the file dialog know what file type should be used initially.
        If this is not done then when setting the filter index
        programmatically to 1 the file will still have the extension
        of the first file type instead of the second one. E.g. when file
        types are foo and bar, a filename "myletter" with SetDialogIndex(1)
        would result in saving as myletter.foo, while we want myletter.bar.
        */
        if(m_firstFileTypeFilter > 0)
        {
            DoOnFilterSelected(m_firstFileTypeFilter);
        }

        if ( !m_dir.IsEmpty() )
            [sPanel setDirectoryURL:[NSURL fileURLWithPath:dir.AsNSString()
                                               isDirectory:YES]];

        if ( !m_fileName.IsEmpty() )
            [sPanel setNameFieldStringValue: file.AsNSString()];

        returnCode = [sPanel runModal];
        ModalFinishedCallback(sPanel, returnCode);
    }
    else
    {
        NSOpenPanel* oPanel = [NSOpenPanel openPanel];
        
        SetupExtraControls(oPanel);

        wxOpenSavePanelDelegate* del = [[wxOpenSavePanelDelegate alloc]init];
        [oPanel setDelegate:del];
        m_delegate = del;

        [oPanel setTreatsFilePackagesAsDirectories:NO];
        [oPanel setCanChooseDirectories:NO];
        [oPanel setResolvesAliases:HasFlag(wxFD_NO_FOLLOW) ? NO : YES];
        [oPanel setCanChooseFiles:YES];
        [oPanel setMessage:cf.AsNSString()];
        [oPanel setAllowsMultipleSelection: (HasFlag(wxFD_MULTIPLE) ? YES : NO )];
        [oPanel setShowsHiddenFiles: HasFlag(wxFD_SHOW_HIDDEN) ? YES : NO];

        // Note that the test here is intentionally different from the one
        // above, in the wxFD_SAVE case: we need to call DoOnFilterSelected()
        // even for m_firstFileTypeFilter == 0, i.e. when using the default
        // filter.
        if ( m_firstFileTypeFilter >= 0 )
        {
            DoOnFilterSelected(m_firstFileTypeFilter);
        }
        else
        {
            if ( m_delegate )
                [(wxOpenSavePanelDelegate*) m_delegate setAllowedExtensions: m_currentExtensions];
            else
                [oPanel setAllowedFileTypes: types];
        }
        if ( !m_dir.IsEmpty() )
            [oPanel setDirectoryURL:[NSURL fileURLWithPath:dir.AsNSString() 
                                               isDirectory:YES]];
        returnCode = [oPanel runModal];
            
        ModalFinishedCallback(oPanel, returnCode);
    }
    
    OSXEndModalDialog();


    return GetReturnCode();
}

void wxFileDialog::ModalFinishedCallback(void* panel, int returnCode)
{
    NSSavePanel* const sPanel = static_cast<NSSavePanel*>(panel);

    const bool wasAccepted = returnCode == NSModalResponseOK;
    if (HasFlag(wxFD_SAVE))
    {
        if (wasAccepted)
        {
            NSString* unsafePath = [NSString stringWithUTF8String:[[sPanel URL] fileSystemRepresentation]];
            m_path = wxCFStringRef([[unsafePath precomposedStringWithCanonicalMapping] retain]).AsString();
            m_fileName = wxFileNameFromPath(m_path);
            m_dir = wxPathOnly( m_path );
        }
    }
    else
    {
        NSOpenPanel* const oPanel = static_cast<NSOpenPanel*>(sPanel);
        if (wasAccepted)
        {
            bool isFirst = true;
            for (NSURL* filename in [oPanel URLs])
            {
                NSString* unsafePath = [NSString stringWithUTF8String:[filename fileSystemRepresentation]];
                wxString fnstr = wxCFStringRef([[unsafePath precomposedStringWithCanonicalMapping] retain]).AsString();
                m_paths.Add( fnstr );
                m_fileNames.Add( wxFileNameFromPath(fnstr) );
                if ( isFirst )
                {
                    m_path = fnstr;
                    m_fileName = wxFileNameFromPath(fnstr);
                    m_dir = wxPathOnly( fnstr );
                    isFirst = false;
                }
            }
        }
        if ( m_delegate )
        {
            [oPanel setDelegate:nil];
            [m_delegate release];
            m_delegate = nil;
        }
    }

    if (wasAccepted)
    {
        if (m_filterChoice)
            m_filterIndex = m_filterChoice->GetSelection();
        else
            m_filterIndex = GetMatchingFilterExtension(m_fileName);

        TransferDataFromExtraControl();
    }

    SetReturnCode(wasAccepted ? wxID_OK : wxID_CANCEL);
    
    // workaround for sandboxed app, see above, must be executed before window modal handler
    // because there this instance will be deleted
    if ( m_isNativeWindowWrapper )
        UnsubclassWin();

    if (GetModality() == wxDIALOG_MODALITY_WINDOW_MODAL)
        SendWindowModalDialogEvent ( wxEVT_WINDOW_MODAL_DIALOG_CLOSED  );
    
    [sPanel setAccessoryView:nil];
}

#endif // wxUSE_FILEDLG
