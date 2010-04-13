/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/filedlg.cpp
// Purpose:     wxFileDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_FILEDLG

#include "wx/filedlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
#endif

#include "wx/tokenzr.h"
#include "wx/filename.h"

#include "wx/osx/private.h"

#ifndef __DARWIN__
    #include <Navigation.h>
    #include "PLStringFuncs.h"
#endif

IMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase)

// the data we need to pass to our standard file hook routine
// includes a pointer to the dialog, a pointer to the standard
// file reply record (so we can inspect the current selection)
// and a copy of the "previous" file spec of the reply record
// so we can see if the selection has changed

class OpenUserDataRec
{
public:
    OpenUserDataRec( wxFileDialog* dialog );
    
    bool FilterCallback( AEDesc *theItem, void *info, NavFilterModes filterMode );
    void EventProc( NavEventCallbackMessage inSelector, NavCBRecPtr ioParams );

    int GetCurrentFilter() const {return currentfilter;}
    CFArrayRef GetMenuItems() const { return menuitems;}
    
     
private:
    void EventProcCBEvent( NavCBRecPtr ioParams );
    void EventProcCBEventMouseDown( NavCBRecPtr ioParams);
    void EventProcCBStart( NavCBRecPtr ioParams );
    void EventProcCBPopupMenuSelect( NavCBRecPtr ioParams );
    void EventProcCBCustomize( NavCBRecPtr ioParams );
    void EventProcCBAdjustRect( NavCBRecPtr ioParams );
    bool CheckFile( const wxString &filename , OSType type);
    void MakeUserDataRec( const wxString& filter);
    
    wxFileDialog*      dialog;
    int                currentfilter ;
    wxString           defaultLocation;
    wxArrayString      extensions ;
    wxArrayLong        filtermactypes ;
    CFMutableArrayRef         menuitems ;
    wxArrayString      name ;
    bool               saveMode ;
};

OpenUserDataRec::OpenUserDataRec( wxFileDialog* d)
{
    dialog = d;
    saveMode = dialog->HasFdFlag(wxFD_SAVE);
    
    defaultLocation = dialog->GetDirectory();
    MakeUserDataRec(dialog->GetWildcard());
    currentfilter = dialog->GetFilterIndex();
    
    menuitems = NULL;
    
    size_t numFilters = extensions.GetCount();
    if (numFilters)
    {
        menuitems = CFArrayCreateMutable( kCFAllocatorDefault ,
                                         numFilters , &kCFTypeArrayCallBacks ) ;
        for ( size_t i = 0 ; i < numFilters ; ++i )
        {
            CFArrayAppendValue( menuitems , (CFStringRef) wxCFStringRef( name[i] ) ) ;
        }
    }
    
}

void OpenUserDataRec::EventProc(NavEventCallbackMessage inSelector,NavCBRecPtr ioParams)
{
    switch (inSelector)
    {
        case kNavCBEvent:
            EventProcCBEvent(ioParams);
            break;
        case kNavCBStart:
            EventProcCBStart(ioParams);
            break;
        case kNavCBPopupMenuSelect:
            EventProcCBPopupMenuSelect(ioParams);
            break;
        case kNavCBCustomize:
            EventProcCBCustomize(ioParams);
            break;
        case kNavCBAdjustRect:
            EventProcCBAdjustRect(ioParams);
            break;
        default:
            break;
    }
}

void OpenUserDataRec::EventProcCBEvent(NavCBRecPtr callBackParms)
{
    switch (callBackParms->eventData.eventDataParms.event->what)
    {
        case mouseDown:
        {
            EventProcCBEventMouseDown(callBackParms);
            break;
        }
    }            
}

void OpenUserDataRec::EventProcCBEventMouseDown(NavCBRecPtr callBackParms)
{
    EventRecord *evt = callBackParms->eventData.eventDataParms.event;
    Point where = evt->where;
    GlobalToLocal(&where);
    
    ControlRef whichControl = FindControlUnderMouse(where, callBackParms->window, NULL);
    if (whichControl != NULL)
    {
        ControlKind theKind;
        GetControlKind(whichControl, &theKind);
        
        // Moving the focus if we clicked in an editable text control
        // In this sample, we only have a Clock and an Unicode Edit controls
        if ((theKind.kind == kControlKindEditUnicodeText) || (theKind.kind == kControlKindClock))
        {
            ControlRef currentlyFocusedControl;
            GetKeyboardFocus(callBackParms->window, &currentlyFocusedControl);
            if (currentlyFocusedControl != whichControl)
                SetKeyboardFocus(callBackParms->window, whichControl, kControlFocusNextPart);
        }
        HandleControlClick(whichControl, where, evt->modifiers, NULL);
    }
}

void OpenUserDataRec::EventProcCBStart(NavCBRecPtr ioParams)
{
    if (!defaultLocation.empty())
    {
        // Set default location for the modern Navigation APIs
        // Apple Technical Q&A 1151
        FSRef theFile;
        wxMacPathToFSRef(defaultLocation, &theFile);
        AEDesc theLocation = { typeNull, NULL };
        if (noErr == ::AECreateDesc(typeFSRef, &theFile, sizeof(FSRef), &theLocation))
            ::NavCustomControl(ioParams->context, kNavCtlSetLocation, (void *) &theLocation);
    }
    
    if( extensions.GetCount() > 0 )
    {
        NavMenuItemSpec  menuItem;
        memset( &menuItem, 0, sizeof(menuItem) );
        menuItem.version = kNavMenuItemSpecVersion;
        menuItem.menuType = currentfilter;
        ::NavCustomControl(ioParams->context, kNavCtlSelectCustomType, &menuItem);
    }
    
    if (dialog->GetExtraControl())
    {
        ControlRef ref = dialog->GetExtraControl()->GetPeer()->GetControlRef();
        NavCustomControl(ioParams->context, kNavCtlAddControl, ref);
    }
    
}

void OpenUserDataRec::EventProcCBPopupMenuSelect(NavCBRecPtr ioParams)
{
    NavMenuItemSpec * menu = (NavMenuItemSpec *) ioParams->eventData.eventDataParms.param ;
    const size_t numFilters = extensions.GetCount();
    
    if ( menu->menuType < numFilters )
    {
        currentfilter = menu->menuType ;
        if ( saveMode )
        {
            int i = menu->menuType ;
            
            // isolate the first extension string
            wxString firstExtension = extensions[i].BeforeFirst('|').BeforeFirst(';');
            
            wxString extension = firstExtension.AfterLast('.') ;
            wxString sfilename ;
            
            wxCFStringRef cfString( wxCFRetain( NavDialogGetSaveFileName( ioParams->context ) ) );
            sfilename = cfString.AsString() ;
            
            int pos = sfilename.Find('.', true) ;
            if ( pos != wxNOT_FOUND && extension != wxT("*") )
            {
                sfilename = sfilename.Left(pos+1)+extension ;
                cfString = wxCFStringRef( sfilename , wxFONTENCODING_DEFAULT ) ;
                NavDialogSetSaveFileName( ioParams->context , cfString ) ;
            }
        }
    }
}

void OpenUserDataRec::EventProcCBCustomize(NavCBRecPtr ioParams)
{
    if (ioParams->customRect.right == 0 && ioParams->customRect.bottom == 0 && dialog->GetExtraControl())
    {
        wxSize size = dialog->GetExtraControl()->GetSize();
        ioParams->customRect.right = size.x;
        ioParams->customRect.bottom = size.y;
    }
}

void OpenUserDataRec::EventProcCBAdjustRect(NavCBRecPtr ioParams)
{
}

void OpenUserDataRec::MakeUserDataRec( const wxString& filter )
{
    menuitems = NULL ;
    currentfilter = 0 ;
    saveMode = false ;

    if ( !filter.empty() )
    {
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
                    name.Add( current ) ;
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
            extensions.Add( name[filterIndex] ) ;
        else
            extensions.Add( current ) ;
        if ( filterIndex == 0 || isName )
            name.Add( current ) ;

        ++filterIndex ;

        const size_t extCount = extensions.GetCount();
        for ( size_t i = 0 ; i < extCount; i++ )
        {
            wxUint32 fileType, creator;
            wxString extension = extensions[i];

            // Remove leading '*'
            if (extension.length() && (extension.GetChar(0) == '*'))
                extension = extension.Mid( 1 );

            // Remove leading '.'
            if (extension.length() && (extension.GetChar(0) == '.'))
                extension = extension.Mid( 1 );

            if (wxFileName::MacFindDefaultTypeAndCreator( extension, &fileType, &creator ))
                filtermactypes.Add( (OSType)fileType );
            else
                filtermactypes.Add( '****' ); // We'll fail safe if it's not recognized
        }
    }
}

bool OpenUserDataRec::CheckFile( const wxString &filename , OSType type)
{
    wxString file(filename) ;
    file.MakeUpper() ;

    if ( extensions.GetCount() > 0 )
    {
        //for ( int i = 0 ; i < data->numfilters ; ++i )
        int i = currentfilter ;
        if ( extensions[i].Right(2) == wxT(".*") )
            return true ;

        {
            if ( type == (OSType)filtermactypes[i] )
                return true ;

            wxStringTokenizer tokenizer( extensions[i] , wxT(";") ) ;
            while ( tokenizer.HasMoreTokens() )
            {
                wxString extension = tokenizer.GetNextToken() ;
                if ( extension.GetChar(0) == '*' )
                    extension = extension.Mid(1) ;
                extension.MakeUpper();

                if ( file.length() >= extension.length() && extension == file.Right(extension.length() ) )
                    return true ;
            }
        }

        return false ;
    }

    return true ;
}

bool OpenUserDataRec::FilterCallback(
                                     AEDesc *theItem,
                                     void *info,
                                     NavFilterModes filterMode )
{
    if (filterMode == kNavFilteringBrowserList)
    {
        // We allow navigation to all folders. For files, we check against the current
        // filter string.
        // However, packages should be dealt with like files and not like folders. So
        // check if a folder is a package before deciding what to do.
        NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*) info ;
        FSRef fsref;
        
        if ( theInfo->isFolder )
        {
            // check bundle bit (using Finder Services - used by OS9 on some bundles)
            FSCatalogInfo catalogInfo;
            if (FSGetCatalogInfo (&fsref, kFSCatInfoFinderInfo, &catalogInfo, NULL, NULL, NULL) != noErr)
                return true;
            
            // Check bundle item (using Launch Services - used by OS-X through info.plist or APP)
            LSItemInfoRecord lsInfo;
            if (LSCopyItemInfoForRef(&fsref, kLSRequestBasicFlagsOnly, &lsInfo ) != noErr)
                return true;
            
            // If it's not a bundle, then it's a normal folder and it passes our filter
            FileInfo *fileInfo = (FileInfo *) catalogInfo.finderInfo;
            if ( !(fileInfo->finderFlags & kHasBundle) &&
                !(lsInfo.flags & (kLSItemInfoIsApplication | kLSItemInfoIsPackage)) )
                return true;
        }
        else
        {
            AECoerceDesc (theItem, typeFSRef, theItem);
            if ( AEGetDescData (theItem, &fsref, sizeof (FSRef)) == noErr)
            {
                wxString file = wxMacFSRefToPath( &fsref ) ;
                return CheckFile( file , theInfo->fileAndFolder.fileInfo.finderInfo.fdType ) ;
            }
        }
    }
    
    return true;
}

// end wxmac

pascal Boolean CrossPlatformFilterCallback(
                                           AEDesc *theItem,
                                           void *info,
                                           void *callBackUD,
                                           NavFilterModes filterMode );

pascal Boolean CrossPlatformFilterCallback(
                                           AEDesc *theItem,
                                           void *info,
                                           void *callBackUD,
                                           NavFilterModes filterMode )
{
    OpenUserDataRec* data = (OpenUserDataRec*) callBackUD ;
    return data->FilterCallback(theItem,info,filterMode);
}

static pascal void NavEventProc(
                                NavEventCallbackMessage inSelector,
                                NavCBRecPtr ioParams,
                                NavCallBackUserData ioUserData );

static NavEventUPP sStandardNavEventFilter = NewNavEventUPP(NavEventProc);

static pascal void NavEventProc(
                                NavEventCallbackMessage inSelector,
                                NavCBRecPtr ioParams,
                                NavCallBackUserData ioUserData )
{
    OpenUserDataRec * data = ( OpenUserDataRec *) ioUserData ;
    data->EventProc(inSelector, ioParams);
}


wxFileDialog::wxFileDialog(
    wxWindow *parent, const wxString& message,
    const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
    long style, const wxPoint& pos, const wxSize& sz, const wxString& name)
    : wxFileDialogBase(parent, message, defaultDir, defaultFileName, wildCard, style, pos, sz, name)
{
    wxASSERT_MSG( NavServicesAvailable() , wxT("Navigation Services are not running") ) ;
}

int wxFileDialog::ShowModal()
{
    m_paths.Empty();
    m_fileNames.Empty();
    
    OSErr err;
    NavDialogCreationOptions dialogCreateOptions;

    // set default options
    ::NavGetDefaultDialogCreationOptions(&dialogCreateOptions);

    // this was always unset in the old code
    dialogCreateOptions.optionFlags &= ~kNavSelectDefaultLocation;

    wxCFStringRef message(m_message, GetFont().GetEncoding());
    dialogCreateOptions.windowTitle = message;

    wxCFStringRef defaultFileName(m_fileName, GetFont().GetEncoding());
    dialogCreateOptions.saveFileName = defaultFileName;

    NavDialogRef dialog;
    NavObjectFilterUPP navFilterUPP = NULL;
    OpenUserDataRec myData( this );
            
    dialogCreateOptions.popupExtension = myData.GetMenuItems();
    
    if (HasFdFlag(wxFD_SAVE))
    {
        dialogCreateOptions.optionFlags |= kNavDontAutoTranslate;
        dialogCreateOptions.optionFlags |= kNavDontAddTranslateItems;
        if (dialogCreateOptions.popupExtension == NULL)
            dialogCreateOptions.optionFlags |= kNavNoTypePopup;

        // The extension is important
        if ( dialogCreateOptions.popupExtension == NULL || CFArrayGetCount(dialogCreateOptions.popupExtension)<2)
            dialogCreateOptions.optionFlags |= kNavPreserveSaveFileExtension;

        if (!(m_windowStyle & wxFD_OVERWRITE_PROMPT))
            dialogCreateOptions.optionFlags |= kNavDontConfirmReplacement;

        err = ::NavCreatePutFileDialog(
            &dialogCreateOptions,
            kNavGenericSignature, // Suppresses the 'Default' (top) menu item
            kNavGenericSignature,
            sStandardNavEventFilter,
            &myData, // for defaultLocation
            &dialog );
    }
    else
    {
        // let the user select bundles/programs in dialogs
        dialogCreateOptions.optionFlags |= kNavSupportPackages;

        navFilterUPP = NewNavObjectFilterUPP(CrossPlatformFilterCallback);
        err = ::NavCreateGetFileDialog(
            &dialogCreateOptions,
            NULL, // NavTypeListHandle
            sStandardNavEventFilter,
            NULL, // NavPreviewUPP
            navFilterUPP,
            (void *) &myData, // inClientData
            &dialog );
    }
    
    wxNonOwnedWindow::Create( GetParent(), NavDialogGetWindow(dialog) );

    if (HasExtraControlCreator())
    {
        CreateExtraControl();
    }
    
    if (err == noErr)
        err = ::NavDialogRun(dialog);

    // clean up filter related data, etc.
    if (navFilterUPP)
        ::DisposeNavObjectFilterUPP(navFilterUPP);

    if (err != noErr)
    {
        ::NavDialogDispose(dialog);
        return wxID_CANCEL;
    }

    NavReplyRecord navReply;
    err = ::NavDialogGetReply(dialog, &navReply);
    if (err == noErr && navReply.validRecord)
    {
        AEKeyword   theKeyword;
        DescType    actualType;
        Size        actualSize;
        FSRef       theFSRef;
        wxString thePath ;
        long count;

        m_filterIndex = myData.GetCurrentFilter();
        ::AECountItems( &navReply.selection, &count );
        for (long i = 1; i <= count; ++i)
        {
            err = ::AEGetNthPtr(
                &(navReply.selection), i, typeFSRef, &theKeyword, &actualType,
                &theFSRef, sizeof(theFSRef), &actualSize );
            if (err != noErr)
                break;

            if (HasFdFlag(wxFD_SAVE))
                thePath = wxMacFSRefToPath( &theFSRef, navReply.saveFileName );
            else
                thePath = wxMacFSRefToPath( &theFSRef );

            if (!thePath)
            {
                ::NavDisposeReply(&navReply);
                ::NavDialogDispose(dialog);
                return wxID_CANCEL;
            }

            m_path = thePath;
            m_paths.Add(m_path);
            m_fileName = wxFileNameFromPath(m_path);
            m_fileNames.Add(m_fileName);
        }

        // set these to the first hit
        m_path = m_paths[0];
        m_fileName = wxFileNameFromPath(m_path);
        m_dir = wxPathOnly(m_path);
    }

    ::NavDisposeReply(&navReply);
    ::NavDialogDispose(dialog);

    return (err == noErr) ? wxID_OK : wxID_CANCEL;
}

bool wxFileDialog::SupportsExtraControl() const
{
    return true;
}

#endif // wxUSE_FILEDLG

