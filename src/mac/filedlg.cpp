/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.cpp
// Purpose:     wxFileDialog 
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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
IMPLEMENT_CLASS(wxFileDialog, wxDialog)
#endif
// begin wxmac
#include "wx/mac/private.h"
#include <Navigation.h>
#include "MoreFiles.h"
#include "MoreFilesExtras.h"
extern bool gUseNavServices ;
// the data we need to pass to our standard file hook routine
// includes a pointer to the dialog, a pointer to the standard
// file reply record (so we can inspect the current selection)
// and a copy of the "previous" file spec of the reply record
// so we can see if the selection has changed
struct OpenUserDataRec {
  int           currentfilter ;
  bool				saveMode ;
  wxArrayString      name ;
  wxArrayString      extensions ;
  wxArrayLong		filtermactypes ;
  NavMenuItemSpecArrayHandle menuitems ;
};
typedef struct OpenUserDataRec
	OpenUserDataRec, *OpenUserDataRecPtr;
static pascal void	NavEventProc(
								NavEventCallbackMessage		inSelector,
								NavCBRecPtr					ioParams,
								NavCallBackUserData			ioUserData);
#if TARGET_CARBON
		static NavEventUPP	sStandardNavEventFilter = NewNavEventUPP(NavEventProc);
#else
		static NavEventUPP	sStandardNavEventFilter = NewNavEventProc(NavEventProc);
#endif
static pascal void
NavEventProc(
	NavEventCallbackMessage		inSelector,
	NavCBRecPtr					ioParams,
	NavCallBackUserData	ioUserData	)
{
	OpenUserDataRec * data = ( OpenUserDataRec *) ioUserData ;
	if (inSelector == kNavCBEvent) {	
         wxTheApp->MacHandleOneEvent(ioParams->eventData.eventDataParms.event);
	} 
	else if ( inSelector == kNavCBStart )
	{
		if ( data->menuitems )
			NavCustomControl(ioParams->context, kNavCtlSelectCustomType, &(*data->menuitems)[data->currentfilter]);
	}
	else if ( inSelector == kNavCBPopupMenuSelect )
	{
		NavMenuItemSpec * menu = (NavMenuItemSpec *) ioParams->eventData.eventDataParms.param ;
		if ( menu->menuCreator == 'WXNG' )
		{
			data->currentfilter = menu->menuType ;
			if ( data->saveMode )
			{
				int i = menu->menuType ;
				wxString extension =  data->extensions[i].AfterLast('.') ;
				extension.MakeLower() ;
				Str255 filename ;
				// get the current filename
				NavCustomControl(ioParams->context, kNavCtlGetEditFileName, &filename);
				CopyPascalStringToC( filename , (char*) filename ) ;
				wxString sfilename( filename ) ;
				int pos = sfilename.Find('.',TRUE) ;
				if ( pos != wxNOT_FOUND )
				{
					sfilename = sfilename.Left(pos+1)+extension ;
					CopyCStringToPascal( sfilename.c_str() , filename ) ;
					NavCustomControl(ioParams->context, kNavCtlSetEditFileName, &filename);
				}
			}
	  	}
	}
}
const char * gfilters[] =
{
	"*.TXT" ,
	"*.TIF" ,
	"*.JPG" ,
	
	NULL 
} ;
OSType gfiltersmac[] =
{
	'TEXT' ,
	'TIFF' ,
	'JPEG' ,
	
	'****'
} ;


void MakeUserDataRec(OpenUserDataRec	*myData , const wxString& filter )
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
         current = "" ;
       }
       else
       {
        current += filter2.GetChar(i) ;
       }
    }
    // we allow for compatibility reason to have a single filter expression (like *.*) without
    // an explanatory text, in that case the first part is name and extension at the same time
      
    wxASSERT_MSG( filterIndex == 0 || !isName , "incorrect format of format string" ) ;
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
				if ( strcmp( myData->extensions[i] , gfilters[j] ) == 0 )
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
static Boolean CheckFile( ConstStr255Param name , OSType type , OpenUserDataRecPtr data)
{
    Str255 			filename ;
    
#if TARGET_CARBON
    p2cstrcpy((char *)filename, name) ;
#else
    PLstrcpy( filename , name ) ;
    p2cstr( filename ) ;
#endif
    wxString file(filename) ;
    file.MakeUpper() ;
    
    if ( data->extensions.GetCount() > 0 )
    {
	//for ( int i = 0 ; i < data->numfilters ; ++i )
	    int i = data->currentfilter ;
	    if ( data->extensions[i].Right(2) == ".*" )
	      return true ;
	
	    {
	      if ( type == (OSType)data->filtermactypes[i] )
		      return true ;
	    
	      wxStringTokenizer tokenizer( data->extensions[i] , ";" ) ;
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
		return !CheckFile( myCInfoPBPtr->hFileInfo.ioNamePtr , myCInfoPBPtr->hFileInfo.ioFlFndrInfo.fdType , data ) ;
	}	
		
	return false ;
}
#endif
// end wxmac
wxString wxFileSelector(const char *title,
                     const char *defaultDir, const char *defaultFileName,
                     const char *defaultExtension, const char *filter, int flags,
                     wxWindow *parent, int x, int y)
{
    // If there's a default extension specified but no filter, we create a suitable
    // filter.
    wxString filter2("");
    if ( defaultExtension && !filter )
        filter2 = wxString("*.") + wxString(defaultExtension) ;
    else if ( filter )
        filter2 = filter;
    wxString defaultDirString;
    if (defaultDir)
        defaultDirString = defaultDir;
    else
        defaultDirString = "";
    wxString defaultFilenameString;
    if (defaultFileName)
        defaultFilenameString = defaultFileName;
    else
        defaultFilenameString = "";
    wxFileDialog fileDialog(parent, title, defaultDirString, defaultFilenameString, filter2, flags, wxPoint(x, y));
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        return fileDialog.GetPath();
    }
    else
        return wxGetEmptyString();
}
WXDLLEXPORT wxString wxFileSelectorEx(const char *title,
                       const char *defaultDir,
                       const char *defaultFileName,
                       int* defaultFilterIndex,
                       const char *filter,
                       int       flags,
                       wxWindow* parent,
                       int       x,
                       int       y)
{
    wxFileDialog fileDialog(parent, title ? title : "", defaultDir ? defaultDir : "",
        defaultFileName ? defaultFileName : "", filter ? filter : "", flags, wxPoint(x, y));
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        *defaultFilterIndex = fileDialog.GetFilterIndex();
        return fileDialog.GetPath();
    }
    else
        return wxGetEmptyString();
}
wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
        const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
        long style, const wxPoint& pos)
{
	wxASSERT_MSG( NavServicesAvailable() , "Navigation Services are not running" ) ;
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
    m_path = "";
    m_fileName = defaultFileName;
    m_dir = defaultDir;
    m_wildCard = wildCard;
    m_filterIndex = 0;
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
		if (theItem->descriptorType == typeFSS && !theInfo->isFolder)
		{
		  FSSpec	spec;
		  memcpy( &spec , *theItem->dataHandle , sizeof(FSSpec) ) ;
		  display = CheckFile( spec.name , theInfo->fileAndFolder.fileInfo.finderInfo.fdType , data ) ;
		}
	}
	return display;
}
int wxFileDialog::ShowModal()
{
	NavDialogOptions		mNavOptions;
	NavObjectFilterUPP		mNavFilterUPP = NULL;
	NavPreviewUPP			mNavPreviewUPP = NULL ;
	NavReplyRecord			mNavReply;
	AEDesc					mDefaultLocation ;
	bool					mSelectDefault = false ;
		
	// setup dialog
	::NavGetDefaultDialogOptions(&mNavOptions);
	mNavFilterUPP	= nil;
	mNavPreviewUPP	= nil;
	mSelectDefault	= false;
	mNavReply.validRecord				= false;
	mNavReply.replacing					= false;
	mNavReply.isStationery				= false;
	mNavReply.translationNeeded			= false;
	mNavReply.selection.descriptorType = typeNull;
	mNavReply.selection.dataHandle		= nil;
	mNavReply.keyScript					= smSystemScript;
	mNavReply.fileTranslation			= nil;
	
	// Set default location, the location
	//   that's displayed when the dialog
	//   first appears
	
	FSSpec location ;
	wxMacFilename2FSSpec( m_dir , &location ) ;
	OSErr err = noErr ;
	
	mDefaultLocation.descriptorType = typeNull;
	mDefaultLocation.dataHandle     = nil;
	err = ::AECreateDesc(typeFSS, &location, sizeof(FSSpec), &mDefaultLocation );
	if ( mDefaultLocation.dataHandle ) {
		
		if (mSelectDefault) {
			mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
		} else {
			mNavOptions.dialogOptionFlags &= ~kNavSelectDefaultLocation;
		}
	}
	
#if TARGET_CARBON
	c2pstrcpy((StringPtr)mNavOptions.message, m_message) ;
#else
	strcpy((char *)mNavOptions.message, m_message) ;
	c2pstr((char *)mNavOptions.message ) ;
#endif
#if TARGET_CARBON
	c2pstrcpy((StringPtr)mNavOptions.savedFileName, m_fileName) ;
#else
	strcpy((char *)mNavOptions.savedFileName, m_fileName) ;
	c2pstr((char *)mNavOptions.savedFileName ) ;
#endif
	// zero all data
	
	m_path = wxEmptyString ;
	m_fileName = wxEmptyString ;
	m_paths.Empty();
	m_fileNames.Empty();

	OpenUserDataRec			myData;
	MakeUserDataRec( &myData , m_wildCard ) ;
	myData.currentfilter = m_filterIndex ;
  	if ( myData.extensions.GetCount() > 0 )
  	{
	  	mNavOptions.popupExtension = (NavMenuItemSpecArrayHandle) NewHandle( sizeof( NavMenuItemSpec ) * myData.extensions.GetCount() ) ;
		myData.menuitems = mNavOptions.popupExtension ;
	  	for ( int i = 0 ; i < myData.extensions.GetCount() ; ++i ) 
	  	{
	  	    (*mNavOptions.popupExtension)[i].version = kNavMenuItemSpecVersion ;
	  	    (*mNavOptions.popupExtension)[i].menuCreator = 'WXNG' ;
	  	    (*mNavOptions.popupExtension)[i].menuType = i ;
          #if TARGET_CARBON
          		c2pstrcpy((StringPtr)(*mNavOptions.popupExtension)[i].menuItemName, myData.name[i]) ;
          #else
          		strcpy((char *)(*mNavOptions.popupExtension)[i].menuItemName, myData.name[i]) ;
          		c2pstr((char *)(*mNavOptions.popupExtension)[i].menuItemName ) ;
          #endif
  	 	}
	}
	if ( m_dialogStyle & wxSAVE )
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
					&myData);					// User Data
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
				m_path = "" ;
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
}
// Generic file load/save dialog
static wxString
wxDefaultFileSelector(bool load, const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  char *ext = (char *)extension;
  
  char prompt[50];
  wxString str;
  if (load)
    str = "Load %s file";
  else
    str = "Save %s file";
  sprintf(prompt, wxGetTranslation(str), what);
  if (*ext == '.') ext++;
  char wild[60];
  sprintf(wild, "*.%s", ext);
  return wxFileSelector (prompt, NULL, default_name, ext, wild, 0, parent);
}
// Generic file load dialog
wxString
wxLoadFileSelector(const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  return wxDefaultFileSelector(TRUE, what, extension, default_name, parent);
}

// Generic file save dialog
wxString
wxSaveFileSelector(const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  return wxDefaultFileSelector(FALSE, what, extension, default_name, parent);
}