#include "wx/defs.h"
#include "wx/dc.h"
#include "wx/mac/uma.h"
#include "wx/mac/aga.h"

#ifdef __UNIX__
  #include <Carbon/Carbon.h>
#else
  #include <Navigation.h>
#endif

// init

static bool	sUMAHasAppearance = false ;
static long sUMAAppearanceVersion = 0 ;
extern int gAGABackgroundColor ;
bool UMAHasAppearance() { return sUMAHasAppearance ; }
long UMAGetAppearanceVersion() { return sUMAAppearanceVersion ; }

static bool	sUMAHasWindowManager = false ;
static long sUMAWindowManagerAttr = 0 ;

bool UMAHasWindowManager() { return sUMAHasWindowManager ; }
long UMAGetWindowManagerAttr() { return sUMAWindowManagerAttr ; }
void UMACleanupToolbox()
{
#if UMA_USE_APPEARANCE
	if ( sUMAHasAppearance )
	{
		UnregisterAppearanceClient() ;
	}
#endif
	if ( NavServicesAvailable() )
	{
		NavUnload() ;
	}
}
void UMAInitToolbox( UInt16 inMoreMastersCalls )
{
#if !TARGET_CARBON
	::MaxApplZone();
	for (long i = 1; i <= inMoreMastersCalls; i++)
		::MoreMasters();

	::InitGraf(&qd.thePort);
	::InitFonts();
	::InitMenus();
	::TEInit();
	::InitDialogs(0L);
	::FlushEvents(everyEvent, 0);
	::InitCursor();
	long total,contig;
	PurgeSpace(&total, &contig);
#else
	InitCursor();
#endif

#if UMA_USE_APPEARANCE
	long theAppearance ;
	if ( Gestalt( gestaltAppearanceAttr, &theAppearance ) == noErr )
	{
		sUMAHasAppearance = true ;
		RegisterAppearanceClient();
		if ( Gestalt( gestaltAppearanceVersion, &theAppearance ) == noErr )
		{
			sUMAAppearanceVersion = theAppearance ;
		}
		else
		{
			sUMAAppearanceVersion = 0x0100 ;
		}
	}
#endif // UMA_USE_APPEARANCE
#if UMA_USE_8_6
#if UMA_USE_WINDOWMGR
	if ( Gestalt( gestaltWindowMgrAttr, &sUMAWindowManagerAttr ) == noErr )
	{
		sUMAHasWindowManager = sUMAWindowManagerAttr & gestaltWindowMgrPresent ;
	}
#endif // UMA_USE_WINDOWMGR
#endif
		
#ifndef __UNIX__
#if TARGET_CARBON
// Call currently implicitely done :		InitFloatingWindows() ;
#else
	if ( sUMAHasWindowManager )
		InitFloatingWindows() ;
	else
		InitWindows();
#endif
#endif

	if ( NavServicesAvailable() )
	{
		NavLoad() ;
	}
}

// process manager
long UMAGetProcessMode() 
{
	OSErr err ; 
	ProcessInfoRec processinfo;
	ProcessSerialNumber procno ;
	
	procno.highLongOfPSN = NULL ;
	procno.lowLongOfPSN = kCurrentProcess ;
	processinfo.processInfoLength = sizeof(ProcessInfoRec);
	processinfo.processName = NULL;
	processinfo.processAppSpec = NULL;

	err = ::GetProcessInformation( &procno , &processinfo ) ;
	wxASSERT( err == noErr ) ;
	return processinfo.processMode ;
}

bool UMAGetProcessModeDoesActivateOnFGSwitch() 
{
	return UMAGetProcessMode() & modeDoesActivateOnFGSwitch ;
}

// menu manager

void UMASetMenuTitle( MenuRef menu , StringPtr title )
{
#if !TARGET_CARBON
	long 			size = GetHandleSize( (Handle) menu ) ;
	const long 		headersize = 14 ;
	int				oldlen = (**menu).menuData[0] + 1;
	int				newlen = title[0] + 1 ;
	
	if ( oldlen < newlen )
	{
		// enlarge before adjusting
		SetHandleSize( (Handle) menu , size + (newlen - oldlen ) );
	}

	if ( oldlen != newlen )
		memmove( (char*) (**menu).menuData + newlen , (char*) (**menu).menuData + oldlen , size - headersize - oldlen ) ;

	memcpy( (char*) (**menu).menuData , title ,  newlen ) ;
	if ( oldlen > newlen )
	{
		// shrink after
		SetHandleSize( (Handle) menu , size + (newlen - oldlen ) ) ;
	}
#else
	SetMenuTitle( menu , title ) ;
#endif
}

UInt32 UMAMenuEvent( EventRecord *inEvent )
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
		return MenuEvent( inEvent ) ;
	}
	else
#endif
	{
			if ( inEvent->what == keyDown && inEvent->modifiers & cmdKey)
			{
				return MenuKey( inEvent->message & charCodeMask ) ;
			}
		return NULL ;
	}
}

void 			UMAEnableMenuItem( MenuRef inMenu , MenuItemIndex inItem ) 
{
#if UMA_USE_8_6 || TARGET_CARBON
	EnableMenuItem( inMenu , inItem ) ;
#else
	EnableItem( inMenu , inItem ) ;
#endif
}

void 			UMADisableMenuItem( MenuRef inMenu , MenuItemIndex inItem ) 
{
#if UMA_USE_8_6 || TARGET_CARBON
	DisableMenuItem( inMenu , inItem ) ;
#else
	DisableItem( inMenu , inItem ) ;
#endif
}

void UMAAppendSubMenuItem( MenuRef menu , StringPtr l , SInt16 id ) 
{
	Str255 label ;
	memcpy( label , l , l[0]+1 ) ;
	// hardcoded adding of the submenu combination for mac

	int theEnd = label[0] + 1; 
	if (theEnd > 251) 
		theEnd = 251; // mac allows only 255 characters
	label[theEnd++] = '/';
	label[theEnd++] = hMenuCmd; 
	label[theEnd++] = '!';
	label[theEnd++] = id ; 
	label[theEnd] = 0x00;
	label[0] = theEnd;
	MacAppendMenu(menu, label);
}

void UMAInsertSubMenuItem( MenuRef menu , StringPtr l , MenuItemIndex item , SInt16 id  ) 
{
	Str255 label ;
	memcpy( label , l , l[0]+1 ) ;
	// hardcoded adding of the submenu combination for mac

	int theEnd = label[0] + 1; 
	if (theEnd > 251) 
		theEnd = 251; // mac allows only 255 characters
	label[theEnd++] = '/';
	label[theEnd++] = hMenuCmd; 
	label[theEnd++] = '!';
	label[theEnd++] = id; 
	label[theEnd] = 0x00;
	label[0] = theEnd;
	MacInsertMenuItem(menu, label , item);
}

void UMAAppendMenuItem( MenuRef menu , StringPtr l , SInt16 key, UInt8 modifiers ) 
{
	Str255 label ;
	memcpy( label , l , l[0]+1 ) ;
	if ( key )
	{
			int pos = label[0] ;
			label[++pos] = '/';
			label[++pos] = toupper( key );
			label[0] = pos ;
	}
	MacAppendMenu( menu , label ) ;
}

void UMAInsertMenuItem( MenuRef menu , StringPtr l , MenuItemIndex item , SInt16 key, UInt8 modifiers ) 
{
	Str255 label ;
	memcpy( label , l , l[0]+1 ) ;
	if ( key )
	{
			int pos = label[0] ;
			label[++pos] = '/';
			label[++pos] = toupper( key );
			label[0] = pos ;
	}
	MacInsertMenuItem( menu , label , item) ;
}

void UMADrawMenuBar() 
{
	DrawMenuBar() ;
}


void UMASetMenuItemText( MenuRef menu , MenuItemIndex item , StringPtr label ) 
{
	::SetMenuItemText( menu , item , label ) ;
}

MenuRef	UMANewMenu( SInt16 menuid , StringPtr label ) 
{
	return ::NewMenu(menuid, label);
}

void UMADisposeMenu( MenuRef menu )
{
	DisposeMenu( menu ) ;
}
void UMADeleteMenu( SInt16 menuId ) 
{
	::DeleteMenu( menuId ) ;
}

void UMAInsertMenu( MenuRef insertMenu , SInt16 afterId ) 
{
	::InsertMenu( insertMenu , afterId ) ;
}


// quickdraw

int gPrOpenCounter = 0 ;

OSStatus UMAPrOpen() 
{
#if !TARGET_CARBON
	OSErr err = noErr ;
	++gPrOpenCounter ;
	if ( gPrOpenCounter == 1 )
	{
		PrOpen() ;
		err = PrError() ;
		wxASSERT( err == noErr ) ;
	}
	return err ;
#else
	OSStatus err = noErr ;
	++gPrOpenCounter ;
	if ( gPrOpenCounter == 1 )
	{
		err = PMBegin() ;
		wxASSERT( err == noErr ) ;
	}
	return err ;
#endif
}

OSStatus UMAPrClose() 
{
#if !TARGET_CARBON
	OSErr err = noErr ;
	wxASSERT( gPrOpenCounter >= 1 ) ;
	if ( gPrOpenCounter == 1 )
	{
		PrClose() ;
		err = PrError() ;
		wxASSERT( err == noErr ) ;
	}
	--gPrOpenCounter ;
	return err ;
#else
	OSStatus err = noErr ;
	wxASSERT( gPrOpenCounter >= 1 ) ;
	if ( gPrOpenCounter == 1 )
	{
		err = PMEnd() ;
	}
	--gPrOpenCounter ;
	return err ;
#endif
}

#if !TARGET_CARBON

pascal QDGlobalsPtr GetQDGlobalsPtr (void)
{
	return QDGlobalsPtr (* (Ptr*) LMGetCurrentA5 ( ) - 0xCA);
}

#endif

void UMAShowWatchCursor() 
{
	OSErr err = noErr;

	CursHandle watchFob = GetCursor (watchCursor);

	if (!watchFob)
		err = nilHandleErr;
	else
	{
	#if TARGET_CARBON
		Cursor preservedArrow;
		GetQDGlobalsArrow (&preservedArrow);
		SetQDGlobalsArrow (*watchFob);
		InitCursor ( );
		SetQDGlobalsArrow (&preservedArrow);
	#else
		SetCursor (*watchFob);	
	#endif
	}
}

void			UMAShowArrowCursor() 
{
#if TARGET_CARBON
	Cursor arrow;
	SetCursor (GetQDGlobalsArrow (&arrow));
#else
	SetCursor (&(qd.arrow));
#endif
}

// window manager

GrafPtr		UMAGetWindowPort( WindowRef inWindowRef ) 
{
	wxASSERT( inWindowRef != NULL ) ;
#if TARGET_CARBON 
	return GetWindowPort( inWindowRef ) ; 
#else
	return (GrafPtr) inWindowRef ;
#endif
}

void		 	UMADisposeWindow( WindowRef inWindowRef ) 
{
	wxASSERT( inWindowRef != NULL ) ;
	DisposeWindow( inWindowRef ) ;
}

void 			UMASetWTitleC( WindowRef inWindowRef , const char *title ) 
{
	Str255	ptitle ;
	strncpy( (char*)ptitle , title , 96 ) ;
	ptitle[96] = 0 ;
#if TARGET_CARBON
	c2pstrcpy( ptitle, (char *)ptitle ) ;
#else
	c2pstr( (char*)ptitle ) ;
#endif
	SetWTitle( inWindowRef , ptitle ) ;
}

void 			UMAGetWTitleC( WindowRef inWindowRef , char *title ) 
{
	GetWTitle( inWindowRef , (unsigned char*)title ) ;
#if TARGET_CARBON
	p2cstrcpy( title, (unsigned char *)title ) ;
#else
	p2cstr( (unsigned char*)title ) ;
#endif
}

void			UMAShowWindow( WindowRef inWindowRef ) 
{
	ShowWindow( inWindowRef ) ;
}

void			UMAHideWindow( WindowRef inWindowRef ) 
{
	HideWindow( inWindowRef) ;
}

void 			UMASelectWindow( WindowRef inWindowRef ) 
{
	SelectWindow( inWindowRef ) ;
}

void			UMABringToFront( WindowRef inWindowRef ) 
{
	BringToFront( inWindowRef ) ;
}

void			UMASendBehind( WindowRef inWindowRef , WindowRef behindWindow ) 
{
	SendBehind( inWindowRef , behindWindow ) ;
}

void			UMACloseWindow(WindowRef inWindowRef) 
{
#if TARGET_CARBON
#else
	CloseWindow( inWindowRef ) ;
#endif
}

// appearance additions

void UMAActivateControl( ControlHandle inControl ) 
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	::ActivateControl( inControl ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   	AGAActivateControl( inControl ) ;
   }
#else
	{
	}
#endif
}

void UMADrawControl( ControlHandle inControl ) 
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	::DrawControlInCurrentPort( inControl ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   	AGADrawControl( inControl ) ;
   }
#else
	{
	}
#endif
}

void UMAMoveControl( ControlHandle inControl , short x , short y ) 
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
	  	::MoveControl( inControl , x , y ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   	AGAMoveControl( inControl , x ,y  ) ;
   }
#else
	{
	}
#endif
}

void UMASizeControl( ControlHandle inControl , short x , short y ) 
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
	  	::SizeControl( inControl , x , y ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   	AGASizeControl( inControl , x ,y  ) ;
   }
#else
	{
	}
#endif
}

void UMADeactivateControl( ControlHandle inControl ) 
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	::DeactivateControl( inControl ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   	 AGADeactivateControl( inControl ) ;
   }
#else
	{
	}
#endif
}

void			UMASetThemeWindowBackground		(WindowRef 				inWindow,
								 ThemeBrush 			inBrush,
								 Boolean 				inUpdate)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	::SetThemeWindowBackground( inWindow ,inBrush , inUpdate ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   	AGASetThemeWindowBackground( inWindow , inBrush , inUpdate ) ;
   }
#else
	{
	}
#endif
}

void			UMAApplyThemeBackground			(ThemeBackgroundKind 	inKind,
								 const Rect *			bounds,
								 ThemeDrawState 		inState,
								 SInt16 				inDepth,
								 Boolean 				inColorDev)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
	/*
		if ( sUMAAppearanceVersion >= 0x0110 )
   			::ApplyThemeBackground( inKind ,bounds , inState , inDepth , inColorDev ) ;
   */
   }
   else
#endif
#if !TARGET_CARBON
   {
   	AGAApplyThemeBackground( inKind ,bounds , inState , inDepth , inColorDev ) ;
   }
#else
	{
	}
#endif
}

ControlHandle UMANewControl(WindowPtr 				owningWindow,
								 const Rect *			boundsRect,
								 ConstStr255Param 		controlTitle,
								 Boolean 				initiallyVisible,
								 SInt16 				initialValue,
								 SInt16 				minimumValue,
								 SInt16 				maximumValue,
								 SInt16 				procID,
								 SInt32 				controlReference)
{
	ControlHandle theControl = NULL ;
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   theControl =  NewControl( owningWindow , boundsRect , controlTitle , initiallyVisible ,
   		initialValue , minimumValue , maximumValue , procID , controlReference ) ;
	}
	else
#endif
#if !TARGET_CARBON
	{
		theControl = AGANewControl( owningWindow , boundsRect , controlTitle , initiallyVisible ,
   		initialValue , minimumValue , maximumValue , procID , controlReference ) ;
	}
#else
	{
	}
#endif
	return theControl ;
}

void UMADisposeControl (ControlHandle 			theControl)
{
	if ( UMAHasAppearance() )
	{
   	::DisposeControl( theControl ) ;
   }
   else
   {
   	::DisposeControl( theControl ) ;
   }
}


void UMAHiliteControl	(ControlHandle 			theControl,
								 ControlPartCode 		hiliteState)
								 {
	if ( UMAHasAppearance() )
	{
   	::HiliteControl( theControl , hiliteState ) ;
   }
   else
   {
   	::HiliteControl( theControl , hiliteState ) ;
	}
}


void UMAShowControl						(ControlHandle 			theControl)
{
	if ( UMAHasAppearance() )
	{
   	::ShowControl( theControl ) ;
   }
   else
   {
   	::ShowControl( theControl ) ;
   }
}


void UMAHideControl						(ControlHandle 			theControl)
{
	if ( UMAHasAppearance() )
	{
   		::HideControl( theControl ) ;
   }
   else
   {
   		::HideControl( theControl ) ;
   }
}


void UMASetControlVisibility			(ControlHandle 			inControl,
								 Boolean 				inIsVisible,
								 Boolean 				inDoDraw)
								 {
	if ( UMAHasAppearance() )
	{
#if UMA_USE_APPEARANCE
   	::SetControlVisibility( inControl , inIsVisible, inDoDraw ) ;
#endif
   }
}



bool UMAIsControlActive					(ControlHandle 			inControl)
{
#if TARGET_CARBON
   	return IsControlActive( inControl ) ;
#else
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	return IsControlActive( inControl ) ;
   }
  else
#endif
  	return (**inControl).contrlHilite == 0 ;
#endif
}


bool UMAIsControlVisible				(ControlHandle 			inControl)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	return IsControlVisible( inControl ) ;
   }
#endif
  	return true ;
}

OSErr UMAGetBestControlRect				(ControlHandle 			inControl,
								 Rect *					outRect,
								 SInt16 *				outBaseLineOffset)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	return GetBestControlRect( inControl , outRect , outBaseLineOffset ) ;
   }
  else
#endif
#if !TARGET_CARBON
  {
  	return AGAGetBestControlRect( inControl , outRect , outBaseLineOffset ) ;
  }
#else
	{
		return noErr ;
	}
#endif
}


OSErr UMASetControlFontStyle				(ControlHandle 			inControl,
								 const ControlFontStyleRec * inStyle)	
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
		return ::SetControlFontStyle( inControl , inStyle ) ;
	}
	else
#endif
#if !TARGET_CARBON
		return AGASetControlFontStyle( inControl , inStyle ) ;
#else
	{
		return noErr ;
	}
#endif
}



// control hierarchy

OSErr UMACreateRootControl				(WindowPtr 				inWindow,
								 ControlHandle *		outControl)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	return CreateRootControl( inWindow , outControl  ) ;
   }
  else
#endif
#if !TARGET_CARBON
  	return AGACreateRootControl( inWindow , outControl ) ;
#else
	{
		return noErr ;
	}
#endif
}



OSErr UMAEmbedControl					(ControlHandle 			inControl,
								 ControlHandle 			inContainer)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	return EmbedControl( inControl , inContainer ) ;
   }
  else
#endif
#if !TARGET_CARBON
  	return AGAEmbedControl( inControl , inContainer ) ; ;
#else
	{
		return noErr ;
	}
#endif
}



// keyboard focus
OSErr UMASetKeyboardFocus				(WindowPtr 				inWindow,
								 ControlHandle 			inControl,
								 ControlFocusPart 		inPart)
{
	OSErr err = noErr;
	GrafPtr port ;
	GetPort( &port ) ;
#if TARGET_CARBON
	SetPort( GetWindowPort( inWindow ) ) ;
#else
	SetPort( inWindow ) ;
#endif
	SetOrigin( 0 , 0 ) ;
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	err = SetKeyboardFocus( inWindow , inControl , inPart ) ;
   }
  else
#endif
#if !TARGET_CARBON
  	err = AGASetKeyboardFocus( inWindow , inControl , inPart ) ;
#else
	{
	}
#endif
	SetPort( port ) ;
	wxDC::MacInvalidateSetup() ;
	return err ;
}




// events

ControlPartCode UMAHandleControlClick				(ControlHandle 			inControl,
								 Point 					inWhere,
								 SInt16 				inModifiers,
								 ControlActionUPP 		inAction)		 
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	return HandleControlClick( inControl , inWhere , inModifiers , inAction ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   	return AGAHandleControlClick( inControl , inWhere , inModifiers , inAction ) ;
   }
#else
	{
		return noErr ;
	}
#endif
}


SInt16 UMAHandleControlKey				(ControlHandle 			inControl,
								 SInt16 				inKeyCode,
								 SInt16 				inCharCode,
								 SInt16 				inModifiers)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	return HandleControlKey( inControl , inKeyCode , inCharCode , inModifiers ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   	return AGAHandleControlKey(inControl , inKeyCode , inCharCode , inModifiers ) ;
   }
#else
	{
		return noErr ;
	}
#endif
}


								 
void UMAIdleControls					(WindowPtr 				inWindow)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   		IdleControls( inWindow ) ;
   }
   else
#endif
#if !TARGET_CARBON
   {
   		AGAIdleControls( inWindow ) ;
   }
#else
	{
	}
#endif
}

void UMAUpdateControls( WindowPtr inWindow , RgnHandle inRgn ) 
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
		UpdateControls( inWindow , inRgn ) ;
	}
	else
#endif
#if !TARGET_CARBON
	{
		AGAUpdateControls( inWindow , inRgn ) ;
	}
#else
	{
	}
#endif
}

OSErr UMAGetRootControl( WindowPtr inWindow , ControlHandle *outControl ) 
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
		return GetRootControl( inWindow , outControl ) ;
	}
	else
#endif
#if !TARGET_CARBON
	{
		return AGAGetRootControl( inWindow , outControl ) ;
	}
#else
	{
		return noErr ;
	}
#endif
}


// handling control data

OSErr UMASetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inSize,
								 Ptr 					inData)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
   	 return SetControlData( inControl , inPart , inTagName , inSize , inData ) ;
   }
   else
#endif
#if !TARGET_CARBON
		return AGASetControlData( inControl , inPart , inTagName , inSize , inData ) ;
#else
	{
		return noErr ;
	}
#endif
}



OSErr UMAGetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inBufferSize,
								 Ptr 					outBuffer,
								 Size *					outActualSize)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
		return ::GetControlData( inControl , inPart , inTagName , inBufferSize , outBuffer , outActualSize ) ;
	}
	else
#endif
#if !TARGET_CARBON
	{
		return AGAGetControlData( inControl , inPart , inTagName , inBufferSize , outBuffer , outActualSize ) ;
	}
#else
	{
		return noErr ;
	}
#endif
}


OSErr UMAGetControlDataSize				(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size *					outMaxSize)
{
#if UMA_USE_APPEARANCE
	if ( UMAHasAppearance() )
	{
		return GetControlDataSize( inControl , inPart , inTagName , outMaxSize ) ;
	}
	else
#endif
#if !TARGET_CARBON
	{
		return AGAGetControlDataSize( inControl , inPart , inTagName , outMaxSize ) ;
	}
#else
	{
		return noErr ;
	}
#endif
}





// system 8.0 changes

short UMAFindWindow( Point inPoint , WindowRef *outWindow )
{
	// todo add the additional area codes
	return FindWindow( inPoint , outWindow ) ;
}

OSStatus UMAGetWindowFeatures( WindowRef inWindowRef , UInt32 *outFeatures )
{
#if UMA_USE_WINDOWMGR
	return GetWindowFeatures( inWindowRef , outFeatures ) ;
#else
	return 0 ;
#endif
}

OSStatus UMAGetWindowRegion( WindowRef inWindowRef , WindowRegionCode inRegionCode , RgnHandle ioWinRgn )
{
#if UMA_USE_WINDOWMGR
	return GetWindowRegion( inWindowRef , inRegionCode , ioWinRgn ) ;
#else
	return 0 ;
#endif
}

void UMADrawGrowIcon( WindowRef inWindowRef )
{
	DrawGrowIcon( inWindowRef ) ;
}

OSStatus UMACollapseWindow( WindowRef inWindowRef , Boolean inCollapseIt )
{
	return CollapseWindow( inWindowRef , inCollapseIt ) ;
}

OSStatus UMACollapseAllWindows( Boolean inCollapseEm )
{
	return CollapseAllWindows( inCollapseEm ) ;
}

Boolean UMAIsWindowCollapsed( WindowRef inWindowRef )
{
	return IsWindowCollapsed( inWindowRef ) ;
}

Boolean UMAIsWindowCollapsable( WindowRef inWindowRef )
{
	return IsWindowCollapsable( inWindowRef ) ;
}

// system 8.5 changes<MacWindows.h>
OSStatus 	UMACreateNewWindow( WindowClass windowClass , WindowAttributes attributes , const Rect *bounds,  WindowRef *outWindow ) 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return CreateNewWindow( windowClass , attributes, bounds, outWindow ) ;
	}
	else
#endif
	{
		short procID ;
		if ( UMAHasAppearance() )
		{
			switch( windowClass )
			{
				case kMovableModalWindowClass :
					procID = kWindowMovableModalDialogProc;
					break ;
				case kModalWindowClass :
					procID = kWindowShadowDialogProc;
					break ;
				case kFloatingWindowClass :
					if ( attributes & kWindowSideTitlebarAttribute )
					{
						if( ( attributes & kWindowResizableAttribute ) &&  
							( attributes & kWindowFullZoomAttribute ) )
						{
							procID = kWindowFloatSideFullZoomGrowProc ;
						}
						else if( attributes & kWindowFullZoomAttribute )
						{
							procID = kWindowFloatSideFullZoomProc;
						}
						else if ( attributes & kWindowResizableAttribute )
						{
							procID = kWindowFloatSideGrowProc;
						}
						else
						{
							procID = kWindowFloatSideProc;
						}
					}
					else
					{
						if( ( attributes & kWindowResizableAttribute ) &&  
							( attributes & kWindowFullZoomAttribute ) )
						{
							procID = kWindowFloatFullZoomGrowProc ;
						}
						else if( attributes & kWindowFullZoomAttribute )
						{
							procID = kWindowFloatFullZoomProc;
						}
						else if ( attributes & kWindowResizableAttribute )
						{
							procID = kWindowFloatGrowProc;
						}
						else
						{
							procID = kWindowFloatProc;
						}
					}
					break ;
				case kDocumentWindowClass :
				default :
					if( ( attributes & kWindowResizableAttribute ) &&  
						( attributes & kWindowFullZoomAttribute ) )
					{
						procID = kWindowFullZoomGrowDocumentProc;
					}
					else if( attributes & kWindowFullZoomAttribute )
					{
						procID = kWindowFullZoomDocumentProc;
					}
					else if ( attributes & kWindowResizableAttribute )
					{
						procID = kWindowGrowDocumentProc;
					}
					else
					{
						procID = kWindowDocumentProc;
					}
					break ;
			}
		}
		else
		{
			switch( windowClass )
			{
				case kMovableModalWindowClass :
					procID = movableDBoxProc;
					break ;
				case kModalWindowClass :
					procID = altDBoxProc;
					break ;
				case kFloatingWindowClass :
					if ( attributes & kWindowSideTitlebarAttribute )
					{
						if( ( attributes & kWindowResizableAttribute ) &&  
							( attributes & kWindowFullZoomAttribute ) )
						{
							procID = floatSideZoomGrowProc ;
						}
						else if( attributes & kWindowFullZoomAttribute )
						{
							procID = floatSideZoomProc;
						}
						else if ( attributes & kWindowResizableAttribute )
						{
							procID = floatSideGrowProc;
						}
						else
						{
							procID = floatSideProc;
						}
					}
					else
					{
						if( ( attributes & kWindowResizableAttribute ) &&  
							( attributes & kWindowFullZoomAttribute ) )
						{
							procID = floatZoomGrowProc ;
						}
						else if( attributes & kWindowFullZoomAttribute )
						{
							procID = floatZoomProc;
						}
						else if ( attributes & kWindowResizableAttribute )
						{
							procID = floatGrowProc;
						}
						else
						{
							procID = floatProc;
						}
					}
					break ;
				case kDocumentWindowClass :
				default :
					if( ( attributes & kWindowResizableAttribute ) &&  
						( attributes & kWindowFullZoomAttribute ) )
					{
						procID = zoomDocProc;
					}
					else if( attributes & kWindowFullZoomAttribute )
					{
						procID = zoomNoGrow;
					}
					else if ( attributes & kWindowResizableAttribute )
					{
						procID = documentProc;
					}
					else
					{
						procID = noGrowDocProc;
					}
					break ;
					break ;
			}
		}
		*outWindow = NewCWindow(nil, bounds, "\p", false, procID,  (WindowRef) -1 /*behind*/,
				attributes & kWindowCloseBoxAttribute , (long)NULL);
		return noErr ;		
	}
}

OSStatus 	UMAGetWindowClass( WindowRef inWindowRef , WindowClass *outWindowClass ) 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return GetWindowClass( inWindowRef , outWindowClass ) ;
	}
	else
#endif
		return kDocumentWindowClass ;
}

OSStatus	UMAGetWindowAttributes( WindowRef inWindowRef , WindowAttributes *outAttributes ) 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return GetWindowAttributes( inWindowRef , outAttributes ) ;
	}
#endif
	return kWindowNoAttributes ;
}

void UMAShowFloatingWindows() 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		ShowFloatingWindows() ;
	}
#endif
}

void UMAHideFloatingWindows() 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		HideFloatingWindows() ;
	}
#endif
}

Boolean 	UMAAreFloatingWindowsVisible() 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return AreFloatingWindowsVisible() ;
	}
#endif
	return false ;
}

WindowRef UMAFrontNonFloatingWindow()
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return FrontNonFloatingWindow() ;
	}
	else
#endif
	{
		return FrontWindow() ;
	}
}

WindowRef UMAFrontWindow()
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return FrontWindow() ;
	}
	else
#endif
	{
		return FrontWindow() ;
	}
}

WindowRef UMAGetActiveNonFloatingWindow()
{
	return NULL ;
}

bool UMAIsWindowFloating( WindowRef inWindow ) 
{
	WindowClass cl ;
	
	UMAGetWindowClass( inWindow , &cl ) ;
	return cl == kFloatingWindowClass ;
}

bool UMAIsWindowModal( WindowRef inWindow ) 
{
	WindowClass cl ;
	
	UMAGetWindowClass( inWindow , &cl ) ;
	return cl < kFloatingWindowClass ;
}

// others

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate )
{
	if ( inWindowRef )
	{
//		bool isHighlighted = IsWindowHighlited( inWindowRef ) ;
//		if ( inActivate != isHightlited )
		GrafPtr port ;
		GetPort( &port ) ;
#if TARGET_CARBON
		SetPort( GetWindowPort( inWindowRef ) ) ;
#else
		SetPort( inWindowRef ) ;
#endif
		SetOrigin( 0 , 0 ) ;
		HiliteWindow( inWindowRef , inActivate ) ;
		ControlHandle control = NULL ;
		UMAGetRootControl( inWindowRef , & control ) ;
		if ( control )
		{
			if ( inActivate )
				UMAActivateControl( control ) ;
			else
				UMADeactivateControl( control ) ;
		}	
		SetPort( port ) ;
		wxDC::MacInvalidateSetup() ;
	}
}
OSStatus UMADrawThemePlacard( const Rect *inRect , ThemeDrawState inState ) 
{
#if UMA_USE_APPEARANCE
  if ( UMAHasAppearance() )
  {
    ::DrawThemePlacard( inRect , inState ) ;
  }
  else
#endif
#if !TARGET_CARBON
  {
  }
#else
  {
  }
#endif
}

