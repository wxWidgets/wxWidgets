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

#if !defined(__UNIX__)
  #include "PLStringFuncs.h"
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxFileDialog, wxDialog)
#endif

// begin wxmac

#if defined(__UNIX__)
  #include <Carbon/Carbon.h>
#else
  #include <Navigation.h>
#endif

#ifndef __UNIX__
  #include "morefile.h"
  #include "moreextr.h"
  #include "fullpath.h"
  #include "fspcompa.h"
  #include "PLStringFuncs.h"
#endif

extern bool gUseNavServices ;

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
	NavCallBackUserData			/* ioUserData */)
{
	if (inSelector == kNavCBEvent) {	
			// In Universal Headers 3.2, Apple changed the definition of
		/*
		#if UNIVERSAL_INTERFACES_VERSION >= 0x0320 // Universal Headers 3.2
			UModalAlerts::ProcessModalEvent(*(ioParams->eventData.eventDataParms.event));
			
		#else
			UModalAlerts::ProcessModalEvent(*(ioParams->eventData.event));
		#endif
		*/
		
         wxTheApp->MacHandleOneEvent(ioParams->eventData.eventDataParms.event);
	}
}

char * gfilters[] =
{
	"*.TXT" ,
	
	NULL 
} ;

OSType gfiltersmac[] =
{
	'TEXT' ,
	
	'****'
} ;

// the data we need to pass to our standard file hook routine
// includes a pointer to the dialog, a pointer to the standard
// file reply record (so we can inspect the current selection)
// and a copy of the "previous" file spec of the reply record
// so we can see if the selection has changed

const int kwxMacFileTypes = 10 ;

struct OpenUserDataRec {
	StandardFileReply	*sfrPtr;
	FSSpec				oldSelectionFSSpec;
	char				filter[kwxMacFileTypes][10] ;
	OSType				filtermactypes[kwxMacFileTypes] ;
	int					numfilters ;
	DialogPtr			theDlgPtr;
};
typedef struct OpenUserDataRec
	OpenUserDataRec, *OpenUserDataRecPtr;

#if !TARGET_CARBON

static void wxMacSetupStandardFile(short newVRefNum, long newDirID) 
{ 
	enum 
	{ SFSaveDisk = 0x214, CurDirStore = 0x398 };
	*(short *) SFSaveDisk = -1 * newVRefNum; 
	*(long *) CurDirStore = newDirID; 
}

static void wxMacSetupStandardFileFromPath( const char* s )
{
	Str255	volume ;
	Str255	path ;
	short	vRefNum ;
	long dirRef ;
	short	i,j ;
	Boolean isDirectory ;
	
	for (i=0 ; (s[i]!=0) && (s[i]!=':') ;i++)
		{
		volume[i]=s[i] ;
		}
	volume[i]=':' ;
	volume[i+1]=0 ;
	
	// then copy the rest of the filename
	
	for (j=0;(s[i]!=0);i++,j++)
		{
		path[j]=s[i] ;		
		}
	path[j]=0 ;
	
	c2pstr((Ptr) volume) ;
	c2pstr((Ptr) path) ;
	
	SetVol(volume, 0) ;
	GetVol( NULL, &vRefNum ) ;
		
	GetDirectoryID( vRefNum , fsRtDirID , path , &dirRef , &isDirectory ) ;
	wxMacSetupStandardFile(vRefNum, dirRef) 	;
}

enum {
	kSelectItem = 10, 			// select button item number
	kSFGetFileDlgID = 251,	// dialog resource number
	kStrListID = 251,			// our strings
	kSelectStrNum = 1,			// word 'Select: ' for button
	kDesktopStrNum = 2,			// word 'Desktop' for button
	kSelectNoQuoteStrNum = 3,	// word 'Select: ' for button
	
	kUseQuotes = true,			// parameter for SetButtonName
	kDontUseQuotes = false
};

static void GetLabelString(StringPtr theStr, short stringNum)
{
	GetIndString(theStr, kStrListID, stringNum);
}

static void CopyPStr(StringPtr src, StringPtr dest)
{
	BlockMoveData(src, dest, 1 + src[0]);
}

static char GetSelectKey(void)
{
	// this is the key used to trigger the select button
	
	// NOT INTERNATIONAL SAVVY; should at least grab it from resources
	
	return 's';
}

// FlashButton briefly highlights the dialog button 
// as feedback for key equivalents

static void FlashButton(DialogPtr theDlgPtr, short buttonID)
{
	short	buttonType;
	Handle	buttonHandle;
	Rect	buttonRect;
	unsigned long	finalTicks;
	
	GetDialogItem(theDlgPtr, buttonID, &buttonType, &buttonHandle, &buttonRect);
	HiliteControl((ControlHandle) buttonHandle, kControlButtonPart);
	Delay(10, &finalTicks);
	HiliteControl((ControlHandle) buttonHandle, 0);
}

static Boolean SameFSSpec(FSSpecPtr spec1, FSSpecPtr spec2)
{
	return (spec1->vRefNum == spec2->vRefNum
			&& spec1->parID == spec2->parID
			&& EqualString(spec1->name, spec2->name, false, false));
}
// MyModalDialogFilter maps a key to the Select button, and handles
// flashing of the button when the key is hit

static pascal Boolean SFGetFolderModalDialogFilter(DialogPtr theDlgPtr, EventRecord *eventRec,
											short *item, void *dataPtr)
{
#pragma unused (dataPtr)

	// make certain the proper dialog is showing, 'cause standard file
	// can nest dialogs but calls the same filter for each
	
	if (((WindowPeek) theDlgPtr)->refCon == sfMainDialogRefCon)
	{
		// check if the select button was hit
		/*
		if ((eventRec->what == keyDown)
			&& (eventRec->modifiers & cmdKey) 
			&& ((eventRec->message & charCodeMask) == GetSelectKey()))
		{
			*item = kSelectItem;
			FlashButton(theDlgPtr, kSelectItem);
			return true;
		}
		*/
	}
		
	return false;
}
#endif !TARGET_CARBON

void ExtendedOpenFile( ConstStr255Param message , ConstStr255Param path , const char *filter , FileFilterYDUPP fileFilter, StandardFileReply *theSFR)
{
	Point 				thePt;
	OpenUserDataRec			myData;
	FSSpec				tempSpec;
	Boolean				folderFlag;
	Boolean				wasAliasedFlag;
	DlgHookYDUPP		dlgHookUPP;
	ModalFilterYDUPP	myModalFilterUPP;
	OSErr				err;
	SFTypeList			types ;
	
	
	// presumably we're running System 7 or later so CustomGetFile is
	// available
	
	// set initial contents of Select button to a space
	
	memcpy( theSFR->sfFile.name , "\p " , 2 ) ;
	
	// point the user data parameter at the reply record so we can get to it later
	
	myData.sfrPtr = theSFR;
	if ( filter && filter[0] )
	{
		myData.numfilters = 1 ;
		for ( int i = 0 ; i < myData.numfilters ; i++ )
		{
			int j ;
			
			strcpy( myData.filter[i] , filter ) ;
			for( j = 0 ; myData.filter[i][j] ; j++ )
			{
				myData.filter[i][j] = toupper( myData.filter[i][j] ) ;
			}
			for ( j = 0 ; gfilters[j] ; j++ )
			{
				if ( strcmp( myData.filter[i] , gfilters[j] ) == 0 )
				{
					myData.filtermactypes[i] = gfiltersmac[j] ;
					break ;
				}
			}
			if( gfilters[j] == NULL )
			{
				myData.filtermactypes[i] = '****' ;
			}
		}
	}
	else
	{
		myData.numfilters = 0 ;
	}
	// display the dialog

#if !TARGET_CARBON
	
	dlgHookUPP = NULL ;
//	dlgHookUPP = NewDlgHookYDProc(SFGetFolderDialogHook);
	myModalFilterUPP = NewModalFilterYDProc(SFGetFolderModalDialogFilter);
	
	thePt.h = thePt.v = -1;	// center dialog
	
	ParamText( message , NULL , NULL , NULL ) ;
	
	CustomGetFile(	fileFilter, 
					-1,					// show all types
					NULL,
					theSFR,
					kSFGetFileDlgID,
					thePt,				// top left point
					dlgHookUPP,
					myModalFilterUPP,
					nil,				// activate list
					nil,				// activate proc
					&myData);
					
	DisposeRoutineDescriptor(dlgHookUPP);
	DisposeRoutineDescriptor(myModalFilterUPP);
#else
#endif	
	// if cancel wasn't pressed and no fatal error occurred...
	
	if (theSFR->sfGood)
	{
		// if no name is in the reply record file spec,
		// use the file spec of the parent folder
		
		if (theSFR->sfFile.name[0] == '\0')
		{
			err = FSMakeFSSpec(theSFR->sfFile.vRefNum, theSFR->sfFile.parID,
								"\p", &tempSpec);
			if (err == noErr)
			{
				theSFR->sfFile = tempSpec;
			}
			else
			{
				// no name to return, forget it
				
				theSFR->sfGood = false;
			}
		}
		
		// if there is now a name in the file spec, check if it's
		// for a folder or a volume
		
		if (theSFR->sfFile.name[0] != '\0')
		{
			// the parID of the root of a disk is always fsRtParID == 1
			
			if (theSFR->sfFile.parID == fsRtParID)
			{
				theSFR->sfIsVolume = true;
				theSFR->sfIsFolder = false;	// it would be reasonable for this to be true, too
			}
			
			// we have a valid FSSpec, now let's make sure it's not for an alias file
			
			err = ResolveAliasFile(&theSFR->sfFile, true, &folderFlag, &wasAliasedFlag);
			if (err != noErr)
			{
				theSFR->sfGood = false;
			}
			
			// did the alias resolve to a folder?
			
			if (folderFlag  && ! theSFR->sfIsVolume)
			{
				theSFR->sfIsFolder = true;
			}
		}
	}
}

static pascal Boolean CrossPlatformFileFilter(CInfoPBPtr myCInfoPBPtr, void *dataPtr)
{	
	Str255 			filename ;
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
		if ( data->numfilters > 0 )
		{
			PLstrcpy( filename ,myCInfoPBPtr->hFileInfo.ioNamePtr ) ;
			if ( filename[0] >= 4 )
			{
				for( int j = 1 ; j <= filename[0] ; j++ )
				{
					filename[j] = toupper( filename[j] ) ;
				}
				for ( int i = 0 ; i < data->numfilters ; ++i )
				{
					if ( myCInfoPBPtr->hFileInfo.ioFlFndrInfo.fdType == data->filtermactypes[i] )
						return false ;

					if ( strncmp( (char*) filename + 1 + filename[0] - 4 , 
							& data->filter[i][ strlen(data->filter[i]) - 4 ] , 4 ) == 0 )
							return false ;
				}
			}
			return true ;				
		}
	}	
		
	return false ;
}

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
        strcpy(wxBuffer, (const char *)fileDialog.GetPath());
        return wxBuffer;
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
        strcpy(wxBuffer, (const char *)fileDialog.GetPath());
        return wxBuffer;
    }
    else
        return wxGetEmptyString();
}

wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
        const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
        long style, const wxPoint& pos)
{
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
    m_path = "";
    m_fileName = defaultFileName;
    m_dir = defaultDir;
    m_wildCard = wildCard;
    m_filterIndex = 1;
}

int wxFileDialog::ShowModal()
{
	#if !TARGET_CARBON
	if ( !gUseNavServices )
	{
	if ( m_dialogStyle & wxSAVE )
	{
		StandardFileReply	reply ;
		Str255				prompt ;
		Str255				filename ;

#if TARGET_CARBON
		c2pstrcpy((StringPtr)prompt, m_message) ;
#else
		strcpy((char *)prompt, m_message) ;
		c2pstr((char *)prompt ) ;
#endif
#if TARGET_CARBON
		c2pstrcpy((StringPtr)filename, m_fileName) ;
#else
		strcpy((char *)filename, m_fileName) ;
		c2pstr((char *)filename ) ;
#endif

		#if !TARGET_CARBON
		
		StandardPutFile( prompt , filename , &reply ) ;
	
		#else
		#endif
		if ( reply.sfGood == false )
		{
			m_path = "" ;
			return wxID_CANCEL ;
		}
		else
		{
			m_path = wxMacFSSpec2UnixFilename( &reply.sfFile ) ;
			return wxID_OK ;
		}
	}
	else
	{
		OSType types = '????' ;
		Str255				prompt ;
		Str255				path ;

#if TARGET_CARBON
		c2pstrcpy((StringPtr)prompt, m_message) ;
#else
		strcpy((char *)prompt, m_message) ;
		c2pstr((char *)prompt ) ;
#endif
#if TARGET_CARBON
		c2pstrcpy((StringPtr)path, m_dir ) ;
#else
		strcpy((char *)path, m_dir ) ;
		c2pstr((char *)path ) ;
#endif

		StandardFileReply	reply ;
		FileFilterYDUPP crossPlatformFileFilterUPP = 0 ;
		#if !TARGET_CARBON
		crossPlatformFileFilterUPP = 
			NewFileFilterYDProc(CrossPlatformFileFilter);
		#endif

		ExtendedOpenFile( prompt , path , m_wildCard , crossPlatformFileFilterUPP, &reply);
		#if !TARGET_CARBON
		DisposeFileFilterYDUPP(crossPlatformFileFilterUPP);
		#endif
		if ( reply.sfGood == false )
		{
			m_path = "" ;
			return wxID_CANCEL ;
		}
		else
		{
			m_path = wxMacFSSpec2UnixFilename( &reply.sfFile ) ;
			return wxID_OK ;
		}
	}
    return wxID_CANCEL;
}
	else
#endif
	{
		NavDialogOptions		mNavOptions;
		NavObjectFilterUPP		mNavFilterUPP = NULL;
		NavPreviewUPP			mNavPreviewUPP = NULL ;
		NavReplyRecord			mNavReply;
		AEDesc					mDefaultLocation ;
		bool					mSelectDefault = false ;
		
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
		wxUnixFilename2FSSpec( m_dir , &location ) ;
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

		if ( m_dialogStyle & wxSAVE )
		{
			
			mNavOptions.dialogOptionFlags |= kNavNoTypePopup ;
			mNavOptions.dialogOptionFlags |= kNavDontAutoTranslate ;
			mNavOptions.dialogOptionFlags |= kNavDontAddTranslateItems ;
			
			err = ::NavPutFile(
						&mDefaultLocation,
						&mNavReply,
						&mNavOptions,
						sStandardNavEventFilter ,
						'TEXT',
						'TEXT',
						0L);					// User Data
		}
		else
		{
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
						0L /*inFileTypes.TypeListHandle() */,
						0L);							// User Data
		}
		
		if ( mDefaultLocation.dataHandle != nil )
		{
			::AEDisposeDesc(&mDefaultLocation);
		}
		
		if ( (err != noErr) && (err != userCanceledErr) ) {
			m_path = "" ;
			return wxID_CANCEL ;
		}

		if (mNavReply.validRecord) {
		
			FSSpec  outFileSpec ;
			AEDesc specDesc ;
			
			long count ;
			::AECountItems( &mNavReply.selection , &count ) ;
			for ( long i = 1 ; i <= count ; ++i )
			{
				OSErr err = ::AEGetNthDesc( &mNavReply.selection , i , typeFSS, NULL , &specDesc);
				if ( err != noErr ) {
					m_path = "" ;
					return wxID_CANCEL ;
				}			
				outFileSpec = **(FSSpec**) specDesc.dataHandle;
				if (specDesc.dataHandle != nil) {
					::AEDisposeDesc(&specDesc);
				}

								
				// outFolderDirID = thePB.dirInfo.ioDrDirID;
				m_path = wxMacFSSpec2UnixFilename( &outFileSpec ) ;
				m_paths.Add( m_path ) ;
	            m_fileNames.Add(m_fileName);
	         }
	         // set these to the first hit
	         m_path = m_paths[ 0 ] ;
	         m_fileName = wxFileNameFromPath(m_path);
	         m_dir = wxPathOnly(m_path);
            
			return wxID_OK ;
		}
		return wxID_CANCEL;
	}
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


