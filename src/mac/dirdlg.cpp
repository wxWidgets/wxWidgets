/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.cpp
// Purpose:     wxDirDialog
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dirdlg.h"
#endif

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/dirdlg.h"

#include "wx/cmndata.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxDirDialog, wxDialog)
#endif

// the data we need to pass to our standard file hook routine
// includes a pointer to the dialog, a pointer to the standard
// file reply record (so we can inspect the current selection)
// and a copy of the "previous" file spec of the reply record
// so we can see if the selection has changed

struct UserDataRec {
	StandardFileReply	*sfrPtr;
	FSSpec				oldSelectionFSSpec;
	DialogPtr			theDlgPtr;
};
typedef struct UserDataRec
	UserDataRec, *UserDataRecPtr;

#if !TARGET_CARBON

enum {
	kSelectItem = 10, 			// select button item number
	kSFGetFolderDlgID = 250,	// dialog resource number
	kStrListID = 250,			// our strings
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


// SetButtonName sets the name of the Select button in the dialog
//
// To do this, we need to call the Script Manager to truncate the
// label in the middle to fit the button and to merge the button
// name with the word Select (possibly followed by quotes).  Using
// the Script Manager avoids all sorts of problems internationally.
//
// buttonName is the name to appear following the word Select
// quoteFlag should be true if the name is to appear in quotes

static void SetButtonName(DialogPtr theDlgPtr, short buttonID, StringPtr buttonName,
					Boolean quoteFlag)
{
	short 	buttonType;
	Handle	buttonHandle;
	Rect	buttonRect;
	short	textWidth;
	Handle	labelHandle;
	Handle	nameHandle;
	Str15	keyStr;
	Str255	labelStr;
	OSErr	err;
	
	nameHandle = nil;
	labelHandle = nil;
	
	// get the details of the button from the dialog
	
	GetDialogItem(theDlgPtr, buttonID, &buttonType, &buttonHandle, &buttonRect);
	
	// get the string for the select button label, "Select ^0" or "Select Ò^0Ó"
	
	GetLabelString(labelStr, (quoteFlag == kUseQuotes) ? kSelectStrNum : kSelectNoQuoteStrNum);
	
	// make string handles containing the select button label and the
	// file name to be stuffed into the button
	
	err = PtrToHand(&labelStr[1], &labelHandle, labelStr[0]);
	if (err != noErr) goto Bail;
	
	// cut out the middle of the file name to fit the button
	//
	// we'll temporarily use labelStr here to hold the modified button name
	// since we don't own the buttonName string storage space
	
	textWidth = (buttonRect.right - buttonRect.left) - StringWidth(labelStr);

	CopyPStr(buttonName, labelStr);
	(void) TruncString(textWidth, labelStr, smTruncMiddle);
	
	err = PtrToHand(&labelStr[1], &nameHandle, labelStr[0]);
	if (err != noErr) goto Bail;
	
	// replace the ^0 in the Select string with the file name
	
	CopyPStr("\p^0", keyStr);
	
	(void) ReplaceText(labelHandle, nameHandle, keyStr);
	
	labelStr[0] = (unsigned char) GetHandleSize(labelHandle);
	BlockMoveData(*labelHandle, &labelStr[1], labelStr[0]);
	
	// now set the control title, and re-validate the area
	// above the control to avoid a needless redraw
	
	SetControlTitle((ControlHandle) buttonHandle, labelStr);
	
	ValidRect(&buttonRect);

Bail:
	if (nameHandle)		DisposeHandle(nameHandle);
	if (labelHandle)	DisposeHandle(labelHandle);
	
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
											short *item, Ptr dataPtr)
{
#pragma unused (dataPtr)

	// make certain the proper dialog is showing, 'cause standard file
	// can nest dialogs but calls the same filter for each
	
	if (((WindowPeek) theDlgPtr)->refCon == sfMainDialogRefCon)
	{
		// check if the select button was hit
		
		if ((eventRec->what == keyDown)
			&& (eventRec->modifiers & cmdKey) 
			&& ((eventRec->message & charCodeMask) == GetSelectKey()))
		{
			*item = kSelectItem;
			FlashButton(theDlgPtr, kSelectItem);
			return true;
		}
	}
		
	return false;
}


// MyDlgHook is a hook routine that maps the select button to Open
// and sets the Select button name

static pascal short SFGetFolderDialogHook(short item, DialogPtr theDlgPtr, Ptr dataPtr)
{
	UserDataRecPtr	theUserDataRecPtr;
	long			desktopDirID;
	short			desktopVRefNum;
	FSSpec			tempSpec;
	Str63			desktopName;
	OSErr			err;
	
	// be sure Std File is really showing us the intended dialog,
	// not a nested modal dialog
	
	if (((WindowPeek) theDlgPtr)->refCon != sfMainDialogRefCon)
	{
		return item;
	}
	
	theUserDataRecPtr = (UserDataRecPtr) dataPtr;
	
	// map the Select button to Open
	
	if (item == kSelectItem)
	{
		item = sfItemOpenButton;
	}
	
	// find the desktop folder
	
	err = FindFolder(theUserDataRecPtr->sfrPtr->sfFile.vRefNum,
					kDesktopFolderType, kDontCreateFolder,
					&desktopVRefNum, &desktopDirID);
	
	if (err != noErr)
	{
		// for errors, get value that won't match any real vRefNum/dirID
		desktopVRefNum = 0;
		desktopDirID = 0;
	}
	
	// change the Select button label if the selection has changed or
	// if this is the first call to the hook
	
	if (item == sfHookFirstCall
		|| item == sfHookChangeSelection
		|| item == sfHookRebuildList
		|| ! SameFSSpec(&theUserDataRecPtr->sfrPtr->sfFile,
					&theUserDataRecPtr->oldSelectionFSSpec))
	{
		// be sure there is a file name selected
		
		if (theUserDataRecPtr->sfrPtr->sfFile.name[0] != '\0')
		{
			SetButtonName(theDlgPtr, kSelectItem, 
							theUserDataRecPtr->sfrPtr->sfFile.name, 
							kUseQuotes);	// true -> use quotes
		}
		else
		{
			// is the desktop selected?
			
			if (theUserDataRecPtr->sfrPtr->sfFile.vRefNum == desktopVRefNum
				&& theUserDataRecPtr->sfrPtr->sfFile.parID == desktopDirID)
			{
				// set button to "Select Desktop"
				
				GetLabelString(desktopName, kDesktopStrNum);
				SetButtonName(theDlgPtr, kSelectItem, 
								desktopName, kDontUseQuotes);	// false -> no quotes
			}
			else
			{
				// get parent directory's name for the Select button
				//
				// passing an empty name string to FSMakeFSSpec gets the
				// name of the folder specified by the parID parameter
				
				(void) FSMakeFSSpec(theUserDataRecPtr->sfrPtr->sfFile.vRefNum,
					theUserDataRecPtr->sfrPtr->sfFile.parID, "\p",
					&tempSpec);
				SetButtonName(theDlgPtr, kSelectItem, 
							tempSpec.name, kUseQuotes); // true -> use quotes
			}
		}
	}
	
	// save the current selection as the old selection for comparison next time
	//
	// it's not valid on the first call, though, or if we don't have a 
	// name available from standard file
	
	if (item != sfHookFirstCall || theUserDataRecPtr->sfrPtr->sfFile.name[0] != '\0')
	{
		theUserDataRecPtr->oldSelectionFSSpec = theUserDataRecPtr->sfrPtr->sfFile;
	}
	else
	{
		// on first call, empty string won't set the button correctly, 
		// so invalidate oldSelection
		
		theUserDataRecPtr->oldSelectionFSSpec.vRefNum = 999;
		theUserDataRecPtr->oldSelectionFSSpec.parID = 0;
	}
	
	return item;
}
#endif

void StandardGetFolder( ConstStr255Param message , ConstStr255Param path , FileFilterYDUPP fileFilter, StandardFileReply *theSFR)
{
	Point 				thePt;
	SFTypeList			mySFTypeList;
	UserDataRec			myData;
	FSSpec				tempSpec;
	Boolean				folderFlag;
	Boolean				wasAliasedFlag;
	DlgHookYDUPP		dlgHookUPP;
	ModalFilterYDUPP	myModalFilterUPP;
	OSErr				err;
	
	
	// presumably we're running System 7 or later so CustomGetFile is
	// available
	
	// set initial contents of Select button to a space
	
	memcpy(theSFR->sfFile.name, "\p ", 2);
	
	// point the user data parameter at the reply record so we can get to it later
	
	myData.sfrPtr = theSFR;
	
	// display the dialog
	
	#if !TARGET_CARBON
	
	dlgHookUPP = NewDlgHookYDProc(SFGetFolderDialogHook);
	myModalFilterUPP = NewModalFilterYDProc(SFGetFolderModalDialogFilter);
	
	thePt.h = thePt.v = -1;	// center dialog
	
	ParamText( message , NULL , NULL , NULL ) ;
	
	CustomGetFile(	fileFilter, 
					-1,					// show all types
					mySFTypeList,
					theSFR,
					kSFGetFolderDlgID,
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

static pascal Boolean OnlyVisibleFoldersCustomFileFilter(CInfoPBPtr myCInfoPBPtr, Ptr dataPtr)
{
#pragma unused (dataPtr)

	// return true if this item is invisible or a file

	Boolean visibleFlag;
	Boolean folderFlag;
	
	visibleFlag = ! (myCInfoPBPtr->hFileInfo.ioFlFndrInfo.fdFlags & kIsInvisible);
	folderFlag = (myCInfoPBPtr->hFileInfo.ioFlAttrib & 0x10);
	
	// because the semantics of the filter proc are "true means don't show
	// it" we need to invert the result that we return
	
	return !(visibleFlag && folderFlag);
}

wxDirDialog::wxDirDialog(wxWindow *parent, const wxString& message,
        const wxString& defaultPath,
        long style, const wxPoint& pos)
{
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
	m_path = defaultPath;
}

int wxDirDialog::ShowModal()
{
	{
		Str255				prompt ;
		Str255				path ;

		strcpy((char *)prompt, m_message) ;
		c2pstr((char *)prompt ) ;
	
		strcpy((char *)path, m_path ) ;
		c2pstr((char *)path ) ;

		StandardFileReply	reply ;
		FileFilterYDUPP 	invisiblesExcludedCustomFilterUPP = 0 ;
		#if !TARGET_CARBON
		invisiblesExcludedCustomFilterUPP = 
			NewFileFilterYDProc(OnlyVisibleFoldersCustomFileFilter);
		#endif

		StandardGetFolder( prompt , path , invisiblesExcludedCustomFilterUPP, &reply);
	
		#if !TARGET_CARBON
		DisposeRoutineDescriptor(invisiblesExcludedCustomFilterUPP);
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

