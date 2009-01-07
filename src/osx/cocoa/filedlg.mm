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

    wxCFStringRef dir( m_path );
    wxCFStringRef file( m_fileName );

    m_path = wxEmptyString;
    m_fileNames.Clear();

    if (HasFlag(wxFD_SAVE))
    {
        NSSavePanel* sPanel = [NSSavePanel savePanel];
        // makes things more convenient:
        [sPanel setCanCreateDirectories:YES];
        [sPanel setMessage:cf.AsNSString()];
        [sPanel setTreatsFilePackagesAsDirectories:NO];

        if ( HasFlag(wxFD_OVERWRITE_PROMPT) )
        {
        }

        if ( [sPanel runModalForDirectory:dir.AsNSString() file:file.AsNSString() ] == NSOKButton )
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

        if ( [oPanel runModalForDirectory:dir.AsNSString() file:file.AsNSString() types:types] == NSOKButton )
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

#if 0

    wxASSERT(CreateBase(parent,wxID_ANY,pos,wxDefaultSize,style,wxDefaultValidator,wxDialogNameStr));

    if ( parent )
        parent->AddChild(this);

    m_cocoaNSWindow = nil;
    m_cocoaNSView = nil;

    //Init the wildcard array
    m_wildcards = [[NSMutableArray alloc] initWithCapacity:0];

    //If the user requests to save - use a NSSavePanel
    //else use a NSOpenPanel
    if (HasFlag(wxFD_SAVE))
    {
        SetNSPanel([NSSavePanel savePanel]);

        [GetNSSavePanel() setTitle:wxNSStringWithWxString(message)];

        [GetNSSavePanel() setPrompt:@"Save"];
        [GetNSSavePanel() setTreatsFilePackagesAsDirectories:YES];
        [GetNSSavePanel() setCanSelectHiddenExtension:YES];

//        Cached as per-app in obj-c
//        [GetNSSavePanel() setExtensionHidden:YES];

        //
        // NB:  Note that only Panther supports wildcards
        // with save dialogs - not that wildcards in save
        // dialogs are all that useful, anyway :)
        //
    }
    else //m_dialogStyle & wxFD_OPEN
    {
        SetNSPanel([NSOpenPanel openPanel]);
        [m_cocoaNSWindow setTitle:wxNSStringWithWxString(message)];

        [(NSOpenPanel*)m_cocoaNSWindow setAllowsMultipleSelection:(HasFlag(wxFD_MULTIPLE))];
        [(NSOpenPanel*)m_cocoaNSWindow setResolvesAliases:YES];
        [(NSOpenPanel*)m_cocoaNSWindow setCanChooseFiles:YES];
        [(NSOpenPanel*)m_cocoaNSWindow setCanChooseDirectories:NO];
        [GetNSSavePanel() setPrompt:@"Open"];

        //convert wildcards - open panel only takes file extensions -
        //no actual wildcards here :)
        size_t lastwcpos = 0;
        bool bDescription = true;
        size_t i;
        for(i = wildCard.find('|');
                i != wxString::npos;
                i = wildCard.find('|', lastwcpos+1))
        {
            size_t oldi = i;

            if(!bDescription)
            {
                bDescription = !bDescription;

                //work backwards looking for a period
                while(i != lastwcpos && wildCard[--i] != '.') {}

                if(i == lastwcpos)
                {
                    //no extension - can't use this wildcard
                    lastwcpos = oldi;
                    continue;
                }

                [m_wildcards addObject:wxNSStringWithWxString(wildCard.substr(i+1, oldi-i-1))];
            }
            else
                bDescription = !bDescription;
            lastwcpos = oldi;
        }

        if (!bDescription)
        {
            //get last wildcard
            size_t oldi = wildCard.length();
            i = oldi;

            //work backwards looking for a period
            while(i != lastwcpos && wildCard[--i] != '.') {}

            if(i != lastwcpos)
                [m_wildcards addObject:wxNSStringWithWxString(wildCard.substr(i+1, oldi-i-1))];
        }

        if ([m_wildcards count] == 0)
        {
            [m_wildcards release];
            m_wildcards = nil;
        }
    }
}

wxFileDialog::~wxFileDialog()
{
    [m_wildcards release];
}

void wxFileDialog::GetPaths(wxArrayString& paths) const
{
    paths.Empty();

    wxString dir(m_dir);
    if ( m_dir.Last() != _T('\\') )
        dir += _T('\\');

    size_t count = m_fileNames.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if (wxFileName(m_fileNames[n]).IsAbsolute())
            paths.Add(m_fileNames[n]);
        else
            paths.Add(dir + m_fileNames[n]);
    }
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{
    files = m_fileNames;
}

void wxFileDialog::SetPath(const wxString& path)
{
    wxString ext;
    wxFileName::SplitPath(path, &m_dir, &m_fileName, &ext);
    if ( !ext.empty() )
        m_fileName << _T('.') << ext;
}

int wxFileDialog::ShowModal()
{
    wxAutoNSAutoreleasePool thePool;

    m_fileNames.Empty();

    int nResult;

    if (HasFlag(wxFD_SAVE))
    {
        nResult = [GetNSSavePanel()
                    runModalForDirectory:wxNSStringWithWxString(m_dir)
                    file:wxNSStringWithWxString(m_fileName)];

        if (nResult == NSOKButton)
        {
            m_fileNames.Add(wxStringWithNSString([GetNSSavePanel() filename]));
            m_path = m_fileNames[0];
        }
    }
    else //m_dialogStyle & wxFD_OPEN
    {
        nResult = [(NSOpenPanel*)m_cocoaNSWindow
                    runModalForDirectory:wxNSStringWithWxString(m_dir)
                    file:wxNSStringWithWxString(m_fileName)
                    types:m_wildcards];

        if (nResult == NSOKButton)
        {
            for(unsigned i = 0; i < [[(NSOpenPanel*)m_cocoaNSWindow filenames] count]; ++i)
            {
                m_fileNames.Add(wxStringWithNSString([[(NSOpenPanel*)m_cocoaNSWindow filenames] objectAtIndex:(i)]));
            }

            m_path = m_fileNames[0];
        }
    }

    return nResult == NSOKButton ? wxID_OK : wxID_CANCEL;
}

#endif

#endif // wxUSE_FILEDLG
