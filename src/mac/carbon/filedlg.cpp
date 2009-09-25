/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/filedlg.cpp
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

#include "wx/mac/private.h"

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

struct OpenUserDataRec
{
  int                currentfilter ;
  bool               saveMode ;
  wxArrayString      name ;
  wxArrayString      extensions ;
  wxArrayLong        filtermactypes ;
  wxString           defaultLocation;
  CFArrayRef         menuitems ;
};

typedef struct OpenUserDataRec
OpenUserDataRec, *OpenUserDataRecPtr;

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
    if (inSelector == kNavCBEvent)
    {
    }
    else if ( inSelector == kNavCBStart )
    {
        if (data && !(data->defaultLocation).empty())
        {
            // Set default location for the modern Navigation APIs
            // Apple Technical Q&A 1151
            FSRef theFile;
            wxMacPathToFSRef(data->defaultLocation, &theFile);
            AEDesc theLocation = { typeNull, NULL };
            if (noErr == ::AECreateDesc(typeFSRef, &theFile, sizeof(FSRef), &theLocation))
                ::NavCustomControl(ioParams->context, kNavCtlSetLocation, (void *) &theLocation);
        }

        if( data->extensions.GetCount() > 0 )
        {
            NavMenuItemSpec  menuItem;
            memset( &menuItem, 0, sizeof(menuItem) );
            menuItem.version = kNavMenuItemSpecVersion;
            menuItem.menuType = data->currentfilter;
            ::NavCustomControl(ioParams->context, kNavCtlSelectCustomType, &menuItem);
        }
    }
    else if ( inSelector == kNavCBPopupMenuSelect )
    {
        NavMenuItemSpec * menu = (NavMenuItemSpec *) ioParams->eventData.eventDataParms.param ;
        const size_t numFilters = data->extensions.GetCount();

        if ( menu->menuType < numFilters )
        {
            data->currentfilter = menu->menuType ;
            if ( data->saveMode )
            {
                int i = menu->menuType ;
                wxString extension =  data->extensions[i].AfterLast('.') ;
                wxString sfilename ;

                wxMacCFStringHolder cfString( NavDialogGetSaveFileName( ioParams->context ) , false  );
                sfilename = cfString.AsString() ;

                int pos = sfilename.Find('.', true) ;
                if ( pos != wxNOT_FOUND && extension != wxT("*") )
                {
                    sfilename = sfilename.Left(pos+1)+extension ;
                    cfString.Assign( sfilename , wxFONTENCODING_DEFAULT ) ;
                    NavDialogSetSaveFileName( ioParams->context , cfString ) ;
                }
            }
        }
    }
}

void MakeUserDataRec(OpenUserDataRec *myData , const wxString& filter )
{
    myData->menuitems = NULL ;
    myData->currentfilter = 0 ;
    myData->saveMode = false ;

    if ( filter && filter[0] )
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
                    myData->name.Add( current ) ;
                }
                else
                {
                    myData->extensions.Add( current ) ;
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
            myData->extensions.Add( myData->name[filterIndex] ) ;
        else
            myData->extensions.Add( current ) ;
        if ( filterIndex == 0 || isName )
            myData->name.Add( current ) ;

        ++filterIndex ;

        const size_t extCount = myData->extensions.GetCount();
        for ( size_t i = 0 ; i < extCount; i++ )
        {
            wxUint32 fileType, creator;
            wxString extension = myData->extensions[i];

            // Remove leading '*'
            if (extension.length() && (extension.GetChar(0) == '*'))
                extension = extension.Mid( 1 );

            // Remove leading '.'
            if (extension.length() && (extension.GetChar(0) == '.'))
                extension = extension.Mid( 1 );

            if (wxFileName::MacFindDefaultTypeAndCreator( extension, &fileType, &creator ))
                myData->filtermactypes.Add( (OSType)fileType );
            else
                myData->filtermactypes.Add( '****' ); // We'll fail safe if it's not recognized
        }
    }
}

static Boolean CheckFile( const wxString &filename , OSType type , OpenUserDataRecPtr data)
{
    wxString file(filename) ;
    file.MakeUpper() ;

    if ( data->extensions.GetCount() > 0 )
    {
        //for ( int i = 0 ; i < data->numfilters ; ++i )
        int i = data->currentfilter ;
        if ( data->extensions[i].Right(2) == wxT(".*") )
            return true ;

        {
            if ( type == (OSType)data->filtermactypes[i] )
                return true ;

            wxStringTokenizer tokenizer( data->extensions[i] , wxT(";") ) ;
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

// end wxmac

wxFileDialog::wxFileDialog(
    wxWindow *parent, const wxString& message,
    const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
    long style, const wxPoint& pos, const wxSize& sz, const wxString& name)
    : wxFileDialogBase(parent, message, defaultDir, defaultFileName, wildCard, style, pos, sz, name)
{
    wxASSERT_MSG( NavServicesAvailable() , wxT("Navigation Services are not running") ) ;
}

pascal Boolean CrossPlatformFilterCallback(
    AEDesc *theItem,
    void *info,
    void *callBackUD,
    NavFilterModes filterMode )
{
    bool display = true;
    OpenUserDataRecPtr data = (OpenUserDataRecPtr) callBackUD ;

    if (filterMode == kNavFilteringBrowserList)
    {
        NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*) info ;
        if ( !theInfo->isFolder )
        {
            AECoerceDesc (theItem, typeFSRef, theItem); 
            
            FSRef fsref ;
            if ( AEGetDescData (theItem, &fsref, sizeof (FSRef)) == noErr )
            {
#if 1
                memcpy( &fsref , *theItem->dataHandle , sizeof(FSRef) ) ;
                wxString file = wxMacFSRefToPath( &fsref ) ;
                display = CheckFile( file , theInfo->fileAndFolder.fileInfo.finderInfo.fdType , data ) ;
#else
                CFStringRef itemUTI = NULL;
                OSStatus status = LSCopyItemAttribute (&fsref, kLSRolesAll, kLSItemContentType, (CFTypeRef*)&itemUTI);
                if (status == noErr)
                {
                    display = UTTypeConformsTo (itemUTI, CFSTR("public.text") );
                    CFRelease (itemUTI);  
                }
#endif
            }
        }
    }

    return display;
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

    wxMacCFStringHolder message(m_message, GetFont().GetEncoding());
    dialogCreateOptions.windowTitle = message;

    wxMacCFStringHolder defaultFileName(m_fileName, GetFont().GetEncoding());
    dialogCreateOptions.saveFileName = defaultFileName;


    NavDialogRef dialog;
    NavObjectFilterUPP navFilterUPP = NULL;
    OpenUserDataRec myData;
    myData.defaultLocation = m_dir;

    MakeUserDataRec(&myData , m_wildCard);
    myData.currentfilter = m_filterIndex;
    size_t numFilters = myData.extensions.GetCount();
    if (numFilters)
    {
        CFMutableArrayRef popup = CFArrayCreateMutable( kCFAllocatorDefault ,
            numFilters , &kCFTypeArrayCallBacks ) ;
        dialogCreateOptions.popupExtension = popup ;
        myData.menuitems = dialogCreateOptions.popupExtension ;
        for ( size_t i = 0 ; i < numFilters ; ++i )
        {
            CFArrayAppendValue( popup , (CFStringRef) wxMacCFStringHolder( myData.name[i] , GetFont().GetEncoding() ) ) ;
        }
    }

    if (HasFdFlag(wxFD_SAVE))
    {
        myData.saveMode = true;

        dialogCreateOptions.optionFlags |= kNavDontAutoTranslate;
        dialogCreateOptions.optionFlags |= kNavDontAddTranslateItems;
        if (!numFilters)
            dialogCreateOptions.optionFlags |= kNavNoTypePopup;

#if TARGET_API_MAC_OSX
        if (!(m_windowStyle & wxFD_OVERWRITE_PROMPT))
            dialogCreateOptions.optionFlags |= kNavDontConfirmReplacement;
#endif

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

        m_filterIndex = myData.currentfilter;
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

#endif // wxUSE_FILEDLG

