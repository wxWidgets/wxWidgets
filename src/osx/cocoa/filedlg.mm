/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/filedlg.mm
// Purpose:     wxFileDialog for wxCocoa
// Author:      Ryan Norton
// Modified by:
// Created:     2004-10-02
// RCS-ID:      $Id: filedlg.mm 40007 2006-07-05 13:10:46Z SC $
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
#endif

#include "wx/filename.h"

#include "wx/osx/private.h"

// ============================================================================
// implementation
// ============================================================================

// Open Items:
// - support for old style MacOS creator / type combos
// - parameter support for descending into packages as directories (setTreatsFilePackagesAsDirectories)

IMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase)

wxFileDialog::wxFileDialog(
    wxWindow *parent, const wxString& message,
    const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
    long style, const wxPoint& pos, const wxSize& sz, const wxString& name)
    : wxFileDialogBase(parent, message, defaultDir, defaultFileName, wildCard, style, pos, sz, name)
{
}

NSArray* GetTypesFromFilter( const wxString filter )
{
    NSMutableArray* types = nil;
    if ( !filter.empty() )
    {
        wxArrayString names ;
        wxArrayString extensions;

        wxString filter2(filter) ;
        int filterIndex = 0;
        bool isName = true ;
        wxString current ;

        for ( unsigned int i = 0; i < filter2.length() ; i++ )
        {
            if ( filter2.GetChar(i) == wxT('|') )
            {
                if ( isName )
                {
                    names.Add( current ) ;
                }
                else
                {
                    extensions.Add( current ) ;
                    ++filterIndex ;
                }

                isName = !isName ;
                current = wxEmptyString ;
            }
            else
            {
                current += filter2.GetChar(i) ;
            }
        }
        // we allow for compatibility reason to have a single filter expression (like *.*) without
        // an explanatory text, in that case the first part is name and extension at the same time

        wxASSERT_MSG( filterIndex == 0 || !isName , wxT("incorrect format of format string") ) ;
        if ( current.empty() )
            extensions.Add( names[filterIndex] ) ;
        else
            extensions.Add( current ) ;
        if ( filterIndex == 0 || isName )
            names.Add( current ) ;

        ++filterIndex ;

        const size_t extCount = extensions.GetCount();
        for ( size_t i = 0 ; i < extCount; i++ )
        {
            wxString extension = extensions[i];

            // Remove leading '*'
            if (extension.length() && (extension.GetChar(0) == '*'))
                extension = extension.Mid( 1 );

            // Remove leading '.'
            if (extension.length() && (extension.GetChar(0) == '.'))
                extension = extension.Mid( 1 );

            if ( extension.IsEmpty() )
            {
                if ( types != nil )
                    [types release];
                return nil;
            }


            if ( types == nil )
                types = [[NSMutableArray alloc] init];

            wxCFStringRef cfext(extension);
            [types addObject: (NSString*)cfext.AsNSString()  ];
#if 0
            // add support for classic fileType / creator here
            wxUint32 fileType, creator;
            // extension -> mactypes
#endif
        }
    }
    return types;
}

int wxFileDialog::ShowModal()
{
    int result = wxID_CANCEL;

    NSSavePanel *panel = nil;

    wxCFStringRef cf( m_message );

    wxCFStringRef dir( m_dir );
    wxCFStringRef file( m_fileName );

    m_path = wxEmptyString;
    m_fileNames.Clear();
    
    wxNonOwnedWindow* parentWindow = NULL;
    int returnCode = -1;
    
    if (GetParent()) 
    {
        parentWindow = dynamic_cast<wxNonOwnedWindow*>(wxGetTopLevelParent(GetParent()));
    }

    if (HasFlag(wxFD_SAVE))
    {
        NSSavePanel* sPanel = [NSSavePanel savePanel];
        // makes things more convenient:
        [sPanel setCanCreateDirectories:YES];
        [sPanel setMessage:cf.AsNSString()];
        // if we should be able to descend into pacakges we must somehow
        // be able to pass this in
        [sPanel setTreatsFilePackagesAsDirectories:NO];
        [sPanel setCanSelectHiddenExtension:YES];

        if ( HasFlag(wxFD_OVERWRITE_PROMPT) )
        {
        }
    
        if (parentWindow)
        {
            NSWindow* nativeParent = parentWindow->GetWXWindow();
            ModalDialogDelegate* sheetDelegate = [[ModalDialogDelegate alloc] init]; 
            [sPanel beginSheetForDirectory:dir.AsNSString() file:file.AsNSString() 
                modalForWindow: nativeParent modalDelegate: sheetDelegate 
                didEndSelector: @selector(sheetDidEnd:returnCode:contextInfo:) 
                contextInfo: nil];
            [sheetDelegate waitForSheetToFinish];
            result = [sheetDelegate code];
            [sheetDelegate release];
        }
        else
        {
            result = [sPanel runModalForDirectory:dir.AsNSString() file:file.AsNSString() ];
        }
        
        if (result == NSOKButton )
        {
            panel = sPanel;
            result = wxID_OK;

            wxCFStringRef filename( [[sPanel filename] retain] );

            m_path = filename.AsString();
            m_fileName = wxFileNameFromPath(m_path);
            m_dir = wxPathOnly( m_path );
        }
    }
    else
    {
        NSArray* types = GetTypesFromFilter( m_wildCard ) ;
        NSOpenPanel* oPanel = [NSOpenPanel openPanel];
        [oPanel setTreatsFilePackagesAsDirectories:NO];
        [oPanel setCanChooseDirectories:NO];
        [oPanel setResolvesAliases:YES];
        [oPanel setCanChooseFiles:YES];
        [oPanel setMessage:cf.AsNSString()];

        if (parentWindow)
        {
            NSWindow* nativeParent = parentWindow->GetWXWindow();
            ModalDialogDelegate* sheetDelegate = [[ModalDialogDelegate alloc] init]; 
            [oPanel beginSheetForDirectory:dir.AsNSString() file:file.AsNSString() 
                types: types modalForWindow: nativeParent 
                modalDelegate: sheetDelegate 
                didEndSelector: @selector(sheetDidEnd:returnCode:contextInfo:) 
                contextInfo: nil];
            [sheetDelegate waitForSheetToFinish];
            result = [sheetDelegate code];
            [sheetDelegate release];
        }
        else
        {
            result = [oPanel runModalForDirectory:dir.AsNSString() 
                        file:file.AsNSString() types:types];
        }
        if (result == NSOKButton )
        {
            panel = oPanel;
            result = wxID_OK;
            NSArray* filenames = [oPanel filenames];
            for ( size_t i = 0 ; i < [filenames count] ; ++ i )
            {
                wxCFStringRef filename( [(NSString*) [filenames objectAtIndex:i] retain] );
                wxString fnstr = filename.AsString();
                m_paths.Add( fnstr );
                m_fileNames.Add( wxFileNameFromPath(fnstr) );
                if ( i == 0 )
                {
                    m_path = fnstr;
                    m_fileName = wxFileNameFromPath(fnstr);
                    m_dir = wxPathOnly( fnstr );
                }
            }
        }
        if ( types != nil )
            [types release];
    }

    return result;
}

#endif // wxUSE_FILEDLG
