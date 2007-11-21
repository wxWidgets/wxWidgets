/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/filedlg.cpp
// Purpose:     wxFileDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/filedlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
#endif

#include "wx/tokenzr.h"
#include "wx/filename.h"

#ifndef __DARWIN__
    #include "PLStringFuncs.h"
#endif

IMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase)

// begin wxmac

#include "wx/mac/private.h"

#include <Navigation.h>

#ifdef __DARWIN__
#  include "MoreFilesX.h"
#else
#  include "MoreFiles.h"
#  include "MoreFilesExtras.h"
#endif

extern bool gUseNavServices ;

// the data we need to pass to our standard file hook routine
// includes a pointer to the dialog, a pointer to the standard
// file reply record (so we can inspect the current selection)
// and a copy of the "previous" file spec of the reply record
// so we can see if the selection has changed

struct OpenUserDataRec {
  int                currentfilter ;
  bool               saveMode ;
  wxArrayString      name ;
  wxArrayString      extensions ;
  wxArrayLong        filtermactypes ;
  wxString           defaultLocation;
#if TARGET_CARBON
  CFArrayRef         menuitems ;
#else
  NavMenuItemSpecArrayHandle menuitems ;
#endif
};

typedef struct OpenUserDataRec
OpenUserDataRec, *OpenUserDataRecPtr;

static pascal void    NavEventProc(
                                NavEventCallbackMessage        inSelector,
                                NavCBRecPtr                    ioParams,
                                NavCallBackUserData            ioUserData);

#if TARGET_CARBON
        static NavEventUPP    sStandardNavEventFilter = NewNavEventUPP(NavEventProc);
#else
        static NavEventUPP    sStandardNavEventFilter = NewNavEventProc(NavEventProc);
#endif

static pascal void
NavEventProc(
    NavEventCallbackMessage        inSelector,
    NavCBRecPtr                    ioParams,
    NavCallBackUserData    ioUserData    )
{
    OpenUserDataRec * data = ( OpenUserDataRec *) ioUserData ;
    if (inSelector == kNavCBEvent) {
#if TARGET_CARBON
#else
        wxTheApp->MacHandleOneEvent(ioParams->eventData.eventDataParms.event);
#endif
    }
    else if ( inSelector == kNavCBStart )
    {
#if TARGET_CARBON
        if (data && !(data->defaultLocation).empty())
        {
            // Set default location for the modern Navigation APIs
            // Apple Technical Q&A 1151
            FSSpec theFSSpec;
            wxMacFilename2FSSpec(data->defaultLocation, &theFSSpec);
            AEDesc theLocation = {typeNull, NULL};
            if (noErr == ::AECreateDesc(typeFSS, &theFSSpec, sizeof(FSSpec), &theLocation))
                ::NavCustomControl(ioParams->context, kNavCtlSetLocation, (void *) &theLocation);
        }
#else
        if ( data->menuitems )
            NavCustomControl(ioParams->context, kNavCtlSelectCustomType, &(*data->menuitems)[data->currentfilter]);
#endif
    }
    else if ( inSelector == kNavCBPopupMenuSelect )
    {
        NavMenuItemSpec * menu = (NavMenuItemSpec *) ioParams->eventData.eventDataParms.param ;
#if TARGET_CARBON
#else
        if ( menu->menuCreator == 'WXNG' )
#endif
        {
            data->currentfilter = menu->menuType ;
            if ( data->saveMode )
            {
                int i = menu->menuType ;
                wxString extension =  data->extensions[i].AfterLast('.') ;
                extension.MakeLower() ;
                wxString sfilename ;

#if TARGET_CARBON
                wxMacCFStringHolder cfString( NavDialogGetSaveFileName( ioParams->context ) , false  );
                sfilename = cfString.AsString() ;
#else
                Str255 filename ;
                // get the current filename
                NavCustomControl(ioParams->context, kNavCtlGetEditFileName, &filename);
                sfilename = wxMacMakeStringFromPascal( filename ) ;
#endif

                int pos = sfilename.Find('.', true) ;
                if ( pos != wxNOT_FOUND )
                {
                    sfilename = sfilename.Left(pos+1)+extension ;
#if TARGET_CARBON
                    cfString.Assign( sfilename , wxFONTENCODING_DEFAULT ) ;
                    NavDialogSetSaveFileName( ioParams->context , cfString ) ;
#else
                    wxMacStringToPascal( sfilename , filename ) ;
                    NavCustomControl(ioParams->context, kNavCtlSetEditFileName, &filename);
#endif
                }
            }
        }
    }
}


void MakeUserDataRec(OpenUserDataRec    *myData , const wxString& filter )
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
        for( unsigned int i = 0; i < filter2.length() ; i++ )
        {
            if( filter2.GetChar(i) == wxT('|') )
            {
                if( isName ) {
                    myData->name.Add( current ) ;
                }
                else {
                    myData->extensions.Add( current.MakeUpper() ) ;
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
            myData->extensions.Add( current.MakeUpper() ) ;
        if ( filterIndex == 0 || isName )
            myData->name.Add( current.MakeUpper() ) ;

        ++filterIndex ;

        const size_t extCount = myData->extensions.GetCount();
        for ( size_t i = 0 ; i < extCount; i++ )
        {
            wxUint32 fileType;
            wxUint32 creator;
            wxString extension = myData->extensions[i];

            if (extension.GetChar(0) == '*')
                extension = extension.Mid(1);  // Remove leading *

            if (extension.GetChar(0) == '.')
            {
                extension = extension.Mid(1);  // Remove leading .
            }

            if (wxFileName::MacFindDefaultTypeAndCreator( extension, &fileType, &creator ))
            {
                myData->filtermactypes.Add( (OSType)fileType );
            }
            else
            {
                myData->filtermactypes.Add( '****' ) ;   // We'll fail safe if it's not recognized
            }
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
            while( tokenizer.HasMoreTokens() )
            {
                wxString extension = tokenizer.GetNextToken() ;
                if ( extension.GetChar(0) == '*' )
                    extension = extension.Mid(1) ;

                if ( file.length() >= extension.length() && extension == file.Right(extension.length() ) )
                    return true ;
            }
        }
        return false ;
    }
    return true ;
}

#ifndef __DARWIN__
static pascal Boolean CrossPlatformFileFilter(CInfoPBPtr myCInfoPBPtr, void *dataPtr)
{
    OpenUserDataRecPtr data = (OpenUserDataRecPtr) dataPtr ;
    // return true if this item is invisible or a file

    Boolean visibleFlag;
    Boolean folderFlag;

    visibleFlag = ! (myCInfoPBPtr->hFileInfo.ioFlFndrInfo.fdFlags & kIsInvisible);
    folderFlag = (myCInfoPBPtr->hFileInfo.ioFlAttrib & 0x10);

    // because the semantics of the filter proc are "true means don't show
    // it" we need to invert the result that we return

    if ( !visibleFlag )
        return true ;

    if ( !folderFlag )
    {
        wxString file = wxMacMakeStringFromPascal( myCInfoPBPtr->hFileInfo.ioNamePtr ) ;
        return !CheckFile( file , myCInfoPBPtr->hFileInfo.ioFlFndrInfo.fdType , data ) ;
    }

    return false ;
}
#endif

// end wxmac

wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
        const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
        long style, const wxPoint& pos, const wxSize& sz, const wxString& name)
             :wxFileDialogBase(parent, message, defaultDir, defaultFileName, wildCard, style, pos, sz, name)
{
    wxASSERT_MSG( NavServicesAvailable() , wxT("Navigation Services are not running") ) ;
}

pascal Boolean CrossPlatformFilterCallback (
    AEDesc *theItem,
    void *info,
    void *callBackUD,
    NavFilterModes filterMode
)
{
    bool display = true;
    OpenUserDataRecPtr data = (OpenUserDataRecPtr) callBackUD ;

    if (filterMode == kNavFilteringBrowserList)
    {
        NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*) info ;
        if ( !theInfo->isFolder )
        {
            if (theItem->descriptorType == typeFSS )
            {
                FSSpec    spec;
                memcpy( &spec , *theItem->dataHandle , sizeof(FSSpec) ) ;
                wxString file = wxMacMakeStringFromPascal( spec.name ) ;
                display = CheckFile( file , theInfo->fileAndFolder.fileInfo.finderInfo.fdType , data ) ;
            }
#if TARGET_CARBON
            else if ( theItem->descriptorType == typeFSRef )
            {
                FSRef fsref ;
                memcpy( &fsref , *theItem->dataHandle , sizeof(FSRef) ) ;



                CFURLRef fullURLRef;
                fullURLRef = ::CFURLCreateFromFSRef(NULL, &fsref);
#ifdef __UNIX__
                CFURLPathStyle pathstyle = kCFURLPOSIXPathStyle;
#else
                CFURLPathStyle pathstyle = kCFURLHFSPathStyle;
#endif
                CFStringRef cfString = CFURLCopyFileSystemPath(fullURLRef, pathstyle);
                ::CFRelease( fullURLRef ) ;
                wxString file = wxMacCFStringHolder(cfString).AsString(wxFont::GetDefaultEncoding());

                display = CheckFile( file , theInfo->fileAndFolder.fileInfo.finderInfo.fdType , data ) ;
            }
#endif
        }
    }

    return display;
}

int wxFileDialog::ShowModal()
{
#if TARGET_CARBON
    OSErr err;
    NavDialogCreationOptions dialogCreateOptions;
    // set default options
    ::NavGetDefaultDialogCreationOptions(&dialogCreateOptions);

    // this was always unset in the old code
    dialogCreateOptions.optionFlags &= ~kNavSelectDefaultLocation;

    wxMacCFStringHolder message(m_message, m_font.GetEncoding());
    dialogCreateOptions.windowTitle = message;

    wxMacCFStringHolder defaultFileName(m_fileName, m_font.GetEncoding());
    dialogCreateOptions.saveFileName = defaultFileName;


    NavDialogRef dialog;
    NavObjectFilterUPP navFilterUPP = NULL;
    CFArrayRef cfArray = NULL; // for popupExtension
    OpenUserDataRec myData;
    myData.defaultLocation = m_dir;

    if (HasFlag(wxFD_SAVE))
    {
        dialogCreateOptions.optionFlags |= kNavNoTypePopup;
        dialogCreateOptions.optionFlags |= kNavDontAutoTranslate;
        dialogCreateOptions.optionFlags |= kNavDontAddTranslateItems;

        // The extension is important
        dialogCreateOptions.optionFlags |= kNavPreserveSaveFileExtension;

        err = ::NavCreatePutFileDialog(&dialogCreateOptions,
                                       'TEXT',
                                       'TEXT',
                                       sStandardNavEventFilter,
                                       &myData, // for defaultLocation
                                       &dialog);
    }
    else
    {
        MakeUserDataRec(&myData , m_wildCard);
        size_t numfilters = myData.extensions.GetCount();
        if (numfilters > 0)
        {
            CFMutableArrayRef popup = CFArrayCreateMutable( kCFAllocatorDefault ,
                numfilters , &kCFTypeArrayCallBacks ) ;
            dialogCreateOptions.popupExtension = popup ;
            myData.menuitems = dialogCreateOptions.popupExtension ;
            for ( size_t i = 0 ; i < numfilters ; ++i )
            {
                CFArrayAppendValue( popup , (CFStringRef) wxMacCFStringHolder( myData.name[i] , m_font.GetEncoding() ) ) ;
            }
        }

        navFilterUPP = NewNavObjectFilterUPP(CrossPlatformFilterCallback);
        err = ::NavCreateGetFileDialog(&dialogCreateOptions,
                                       NULL, // NavTypeListHandle
                                       sStandardNavEventFilter,
                                       NULL, // NavPreviewUPP
                                       navFilterUPP,
                                       (void *) &myData, // inClientData
                                       &dialog);
    }

    if (err == noErr)
        err = ::NavDialogRun(dialog);

    // clean up filter related data, etc.
    if (navFilterUPP)
        ::DisposeNavObjectFilterUPP(navFilterUPP);
    if (cfArray)
        ::CFRelease(cfArray);

    if (err != noErr)
        return wxID_CANCEL;

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
        ::AECountItems(&navReply.selection , &count);
        for (long i = 1; i <= count; ++i)
        {
            err = ::AEGetNthPtr(&(navReply.selection), i, typeFSRef, &theKeyword, &actualType,
                                &theFSRef, sizeof(theFSRef), &actualSize);
            if (err != noErr)
                break;

            CFURLRef fullURLRef;
            if (HasFlag(wxFD_SAVE))
            {
                CFURLRef parentURLRef = ::CFURLCreateFromFSRef(NULL, &theFSRef);

                if (parentURLRef)
                {
                    fullURLRef =
                        ::CFURLCreateCopyAppendingPathComponent(NULL,
                                                                parentURLRef,
                                                                navReply.saveFileName,
                                                                false);
                    ::CFRelease(parentURLRef);
                }
            }
            else
            {
                fullURLRef = ::CFURLCreateFromFSRef(NULL, &theFSRef);
            }
#ifdef __UNIX__
            CFURLPathStyle pathstyle = kCFURLPOSIXPathStyle;
#else
            CFURLPathStyle pathstyle = kCFURLHFSPathStyle;
#endif
            CFStringRef cfString = CFURLCopyFileSystemPath(fullURLRef, pathstyle);
            thePath = wxMacCFStringHolder(cfString).AsString(m_font.GetEncoding());
            if (!thePath)
            {
                ::NavDisposeReply(&navReply);
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

    return (err == noErr) ? wxID_OK : wxID_CANCEL;
#else // TARGET_CARBON

    NavDialogOptions           mNavOptions;
    NavObjectFilterUPP           mNavFilterUPP = NULL;
    NavPreviewUPP           mNavPreviewUPP = NULL ;
    NavReplyRecord           mNavReply;
    AEDesc               mDefaultLocation ;
    bool               mSelectDefault = false ;
    OSStatus            err = noErr ;
    // setup dialog

    mNavFilterUPP    = nil;
    mNavPreviewUPP    = nil;
    mSelectDefault    = false;
    mDefaultLocation.descriptorType = typeNull;
    mDefaultLocation.dataHandle     = nil;

    NavGetDefaultDialogOptions(&mNavOptions);
    wxMacStringToPascal( m_message , (StringPtr)mNavOptions.message ) ;
    wxMacStringToPascal( m_fileName , (StringPtr)mNavOptions.savedFileName ) ;

    // Set default location, the location
    //   that's displayed when the dialog
    //   first appears

    FSSpec location ;
    wxMacFilename2FSSpec( m_dir , &location ) ;

    err = ::AECreateDesc(typeFSS, &location, sizeof(FSSpec), &mDefaultLocation );

    if ( mDefaultLocation.dataHandle )
    {
        if (mSelectDefault)
        {
            mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
        } else {
            mNavOptions.dialogOptionFlags &= ~kNavSelectDefaultLocation;
        }
    }

    memset( &mNavReply , 0 , sizeof( mNavReply ) ) ;
    mNavReply.validRecord = false;
    mNavReply.replacing = false;
    mNavReply.isStationery = false;
    mNavReply.translationNeeded = false;
    mNavReply.selection.descriptorType = typeNull;
    mNavReply.selection.dataHandle = nil;
    mNavReply.keyScript = smSystemScript;
    mNavReply.fileTranslation = nil;
    mNavReply.version = kNavReplyRecordVersion ;

    // zero all data

    m_path = wxEmptyString ;
    m_fileName = wxEmptyString ;
    m_paths.Empty();
    m_fileNames.Empty();

    OpenUserDataRec            myData;
    MakeUserDataRec( &myData , m_wildCard ) ;
    myData.currentfilter = m_filterIndex ;
    if ( myData.extensions.GetCount() > 0 )
    {
        mNavOptions.popupExtension = (NavMenuItemSpecArrayHandle) NewHandle( sizeof( NavMenuItemSpec ) * myData.extensions.GetCount() ) ;
        myData.menuitems = mNavOptions.popupExtension ;
        for ( size_t i = 0 ; i < myData.extensions.GetCount() ; ++i )
        {
            (*mNavOptions.popupExtension)[i].version     = kNavMenuItemSpecVersion ;
            (*mNavOptions.popupExtension)[i].menuCreator = 'WXNG' ;
            // TODO : according to the new docs  -1 to 10 are reserved for the OS
            (*mNavOptions.popupExtension)[i].menuType    = i ;
            wxMacStringToPascal( myData.name[i] , (StringPtr)(*mNavOptions.popupExtension)[i].menuItemName ) ;
        }
    }
    if ( HasFlag(wxFD_SAVE) )
    {
        myData.saveMode = true ;

        mNavOptions.dialogOptionFlags |= kNavDontAutoTranslate ;
        mNavOptions.dialogOptionFlags |= kNavDontAddTranslateItems ;

        err = ::NavPutFile(
                           &mDefaultLocation,
                           &mNavReply,
                           &mNavOptions,
                           sStandardNavEventFilter ,
                           NULL,
                           kNavGenericSignature,
                           &myData);                    // User Data
        m_filterIndex = myData.currentfilter ;
    }
    else
    {
        myData.saveMode = false ;

        mNavFilterUPP = NewNavObjectFilterUPP( CrossPlatformFilterCallback ) ;
        if ( m_windowStyle & wxFD_MULTIPLE )
            mNavOptions.dialogOptionFlags |= kNavAllowMultipleFiles ;
        else
            mNavOptions.dialogOptionFlags &= ~kNavAllowMultipleFiles ;

        err = ::NavGetFile(
                           &mDefaultLocation,
                           &mNavReply,
                           &mNavOptions,
                           sStandardNavEventFilter ,
                           mNavPreviewUPP,
                           mNavFilterUPP,
                           NULL ,
                           &myData);
        m_filterIndex = myData.currentfilter ;
    }

    DisposeNavObjectFilterUPP(mNavFilterUPP);
    if ( mDefaultLocation.dataHandle != nil )
    {
        ::AEDisposeDesc(&mDefaultLocation);
    }

    if ( (err != noErr) && (err != userCanceledErr) ) {
        return wxID_CANCEL ;
    }

    if (mNavReply.validRecord)
    {
        FSSpec  outFileSpec ;
        AEDesc specDesc ;
        AEKeyword keyWord ;

        long count ;
        ::AECountItems( &mNavReply.selection , &count ) ;
        for ( long i = 1 ; i <= count ; ++i )
        {
            OSErr err = ::AEGetNthDesc( &mNavReply.selection , i , typeFSS, &keyWord , &specDesc);
            if ( err != noErr )
            {
                m_path = wxEmptyString ;
                return wxID_CANCEL ;
            }
            outFileSpec = **(FSSpec**) specDesc.dataHandle;
            if (specDesc.dataHandle != nil) {
                ::AEDisposeDesc(&specDesc);
            }
            m_path = wxMacFSSpec2MacFilename( &outFileSpec ) ;

            m_paths.Add( m_path ) ;
            m_fileName = wxFileNameFromPath(m_path);
            m_fileNames.Add(m_fileName);
        }
        // set these to the first hit
        m_path = m_paths[ 0 ] ;
        m_fileName = wxFileNameFromPath(m_path);
        m_dir = wxPathOnly(m_path);
        NavDisposeReply( &mNavReply ) ;
        return wxID_OK ;
    }
    return wxID_CANCEL;
#endif // TARGET_CARBON
}
