/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.cpp
// Purpose:     wxFileDialog 
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "filedlg.h"
#endif

#include "wx/defs.h"
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/filedlg.h"
#include "wx/intl.h"
#include "wx/tokenzr.h"

#ifndef __DARWIN__
  #include "PLStringFuncs.h"
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase)
#endif

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
  int           currentfilter ;
  bool                saveMode ;
  wxArrayString      name ;
  wxArrayString      extensions ;
  wxArrayLong        filtermactypes ;
#if TARGET_CARBON
  CFArrayRef           menuitems ;
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
                wxMacCFStringHolder cfString = NavDialogGetSaveFileName( ioParams->context ) ;
                sfilename = cfString.AsString() ;
#else
                Str255 filename ;
                // get the current filename
                NavCustomControl(ioParams->context, kNavCtlGetEditFileName, &filename);
                sfilename = wxMacMakeStringFromPascal( filename ) ;
#endif

                int pos = sfilename.Find('.',TRUE) ;
                if ( pos != wxNOT_FOUND )
                {
                    sfilename = sfilename.Left(pos+1)+extension ;
#if TARGET_CARBON
                    cfString = sfilename ;
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

const wxChar * gfilters[] =
{
    wxT("*.TXT") ,
    wxT("*.TIF") ,
    wxT("*.JPG") ,
    
    NULL 
} ;

OSType gfiltersmac[] =
{
    'TEXT' ,
    'TIFF' ,
    'JPEG' ,
    
    '****'
} ;



void MakeUserDataRec(OpenUserDataRec    *myData , const wxString& filter )
{
    myData->menuitems = NULL ;
    myData->currentfilter = 0 ;
    myData->saveMode = FALSE ;
    
    if ( filter && filter[0] )
    {
        wxString filter2(filter) ;
        int filterIndex = 0;
        bool isName = true ;
        wxString current ;
        for( unsigned int i = 0; i < filter2.Len() ; i++ )
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
        if ( current.IsEmpty() )
            myData->extensions.Add( myData->name[filterIndex] ) ;
        else
            myData->extensions.Add( current.MakeUpper() ) ;
        if ( filterIndex == 0 || isName )
            myData->name.Add( current.MakeUpper() ) ;
        
        ++filterIndex ;
        
        
        const size_t extCount = myData->extensions.GetCount();
        for ( size_t i = 0 ; i < extCount; i++ )
        {
            int j ;
            for ( j = 0 ; gfilters[j] ; j++ )
            {
                if ( myData->extensions[i] == gfilters[j]  )
                {
                    myData->filtermactypes.Add( gfiltersmac[j] ) ;
                    break ;
                }
            }
            if( gfilters[j] == NULL )
            {
                myData->filtermactypes.Add( '****' ) ;
            }
        }
    }
}

static Boolean CheckFile( const wxString &filename , OSType type , OpenUserDataRecPtr data)
{
	wxString file = filename ;
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
                
                if ( file.Len() >= extension.Len() && extension == file.Right(extension.Len() ) )
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
        long style, const wxPoint& pos)
             :wxFileDialogBase(parent, message, defaultDir, defaultFileName, wildCard, style, pos)
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
            else if ( theItem->descriptorType == typeFSRef )
            {
                FSRef fsref ;
                memcpy( &fsref , *theItem->dataHandle , sizeof(FSRef) ) ;
                wxString file ;
                const short maxpath = 1024 ;
                FSRefMakePath( &fsref , (UInt8*) file.GetWriteBuf(maxpath+1),maxpath) ;
                file.UngetWriteBuf() ;
                display = CheckFile( file , theInfo->fileAndFolder.fileInfo.finderInfo.fdType , data ) ;
            }
        }
    }
    
    return display;
}

int wxFileDialog::ShowModal()
{
#if TARGET_CARBON
    NavDialogCreationOptions   mNavOptions;
    NavDialogRef navDialogRef = NULL ;
// since the same field has been renamed ...
#define dialogOptionFlags optionFlags
#else
    NavDialogOptions           mNavOptions;
#endif
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

#if TARGET_CARBON
    NavGetDefaultDialogCreationOptions( &mNavOptions ) ;
    wxMacCFStringHolder cfMessage(m_message) ;
    wxMacCFStringHolder cfFileName(m_fileName) ;
    mNavOptions.saveFileName = cfFileName ;
    mNavOptions.message = cfMessage ;
#else
    NavGetDefaultDialogOptions(&mNavOptions);
    wxMacStringToPascal( m_message , (StringPtr)mNavOptions.message ) ;
    wxMacStringToPascal( m_fileName , (StringPtr)mNavOptions.savedFileName ) ;

    // Set default location, the location
    //   that's displayed when the dialog
    //   first appears
        
    FSSpec location ;
    wxMacFilename2FSSpec( m_dir , &location ) ;
        
    err = ::AECreateDesc(typeFSS, &location, sizeof(FSSpec), &mDefaultLocation );

    if ( mDefaultLocation.dataHandle ) {
            
        if (mSelectDefault) {
            mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
        } else {
            mNavOptions.dialogOptionFlags &= ~kNavSelectDefaultLocation;
        }
    }
#endif

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
#if TARGET_CARBON
        CFMutableArrayRef popup = CFArrayCreateMutable( kCFAllocatorDefault ,
            myData.extensions.GetCount() , &kCFTypeArrayCallBacks ) ;
        mNavOptions.popupExtension = popup ;
        myData.menuitems = mNavOptions.popupExtension ;
        for ( size_t i = 0 ; i < myData.extensions.GetCount() ; ++i ) 
        {
            CFArrayAppendValue( popup , (CFStringRef) wxMacCFStringHolder( myData.name[i] ) ) ;
        }            
#else
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
#endif
    }
    if ( m_dialogStyle & wxSAVE )
    {
        myData.saveMode = true ;

        mNavOptions.dialogOptionFlags |= kNavDontAutoTranslate ;
        mNavOptions.dialogOptionFlags |= kNavDontAddTranslateItems ;
            
#if TARGET_CARBON
        err = NavCreatePutFileDialog( &mNavOptions , NULL , kNavGenericSignature , sStandardNavEventFilter ,
            &myData , &navDialogRef ) ;
        if ( err == noErr )
        {
            err = NavDialogRun( navDialogRef ) ;
            NavUserAction userAction = NavDialogGetUserAction( navDialogRef ) ;
            if ( userAction != kNavUserActionCancel && userAction != kNavUserActionNone )
            {
                NavDialogGetReply( navDialogRef, &mNavReply ) ;
            }
        }
#else
        err = ::NavPutFile(
                           &mDefaultLocation,
                           &mNavReply,
                           &mNavOptions,
                           sStandardNavEventFilter ,
                           NULL,
                           kNavGenericSignature,
                           &myData);                    // User Data
#endif
        m_filterIndex = myData.currentfilter ;
    }
    else
    {
        myData.saveMode = false ;

        mNavFilterUPP = NewNavObjectFilterUPP( CrossPlatformFilterCallback ) ;
        if ( m_dialogStyle & wxMULTIPLE )
            mNavOptions.dialogOptionFlags |= kNavAllowMultipleFiles ;
        else
            mNavOptions.dialogOptionFlags &= ~kNavAllowMultipleFiles ;
            
#if TARGET_CARBON
        err = NavCreateGetFileDialog( &mNavOptions , NULL , sStandardNavEventFilter ,
            mNavPreviewUPP , mNavFilterUPP , &myData , &navDialogRef ) ;
        if ( err == noErr )
        {
            err = NavDialogRun( navDialogRef ) ;
            NavUserAction userAction = NavDialogGetUserAction( navDialogRef ) ;
            if ( userAction != kNavUserActionCancel && userAction != kNavUserActionNone )
            {
                NavDialogGetReply( navDialogRef, &mNavReply ) ;
            }
        }
#else
        err = ::NavGetFile(
                           &mDefaultLocation,
                           &mNavReply,
                           &mNavOptions,
                           sStandardNavEventFilter ,
                           mNavPreviewUPP,
                           mNavFilterUPP,
                           NULL ,
                           &myData);
#endif
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

    if (mNavReply.validRecord) {
        
        FSSpec  outFileSpec ;
        AEDesc specDesc ;
        AEKeyword keyWord ;
            
        long count ;
        ::AECountItems( &mNavReply.selection , &count ) ;
        for ( long i = 1 ; i <= count ; ++i )
        {
            OSErr err = ::AEGetNthDesc( &mNavReply.selection , i , typeFSS, &keyWord , &specDesc);
            if ( err != noErr ) {
                m_path = wxT("") ;
                return wxID_CANCEL ;
            }            
            outFileSpec = **(FSSpec**) specDesc.dataHandle;
            if (specDesc.dataHandle != nil) {
                ::AEDisposeDesc(&specDesc);
            }
            m_path = wxMacFSSpec2MacFilename( &outFileSpec ) ;
#if TARGET_CARBON
            if ( m_dialogStyle & wxSAVE )
            {
                wxMacCFStringHolder cfString = NavDialogGetSaveFileName( navDialogRef ) ;
                m_path += wxFILE_SEP_PATH + cfString.AsString() ;
            }
#endif
            m_paths.Add( m_path ) ;
            m_fileName = wxFileNameFromPath(m_path);
            m_fileNames.Add(m_fileName);
        }
        // set these to the first hit
        m_path = m_paths[ 0 ] ;
        m_fileName = wxFileNameFromPath(m_path);
        m_dir = wxPathOnly(m_path);
        NavDisposeReply( &mNavReply ) ;
        if ( navDialogRef )
            NavDialogDispose( navDialogRef ) ;   

        return wxID_OK ;
    }
    if ( navDialogRef )
        NavDialogDispose( navDialogRef ) ;   

    return wxID_CANCEL;
}

